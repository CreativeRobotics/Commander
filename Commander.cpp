//Commander
//Size of command array = 84
//Size of command struct = 28
//Size of array = 3
//
#include <commander.h>

//Initialise the array of internal commands with the constructor
Commander::Commander(): internalCommands ({ "help",
																						"?",
																						"CMDR echo ",
																						"CMDR echo alt ",
																						"CMDR enable ",
																						"CMDR errors "}){
	bufferString.reserve(bufferSize);
}
//==============================================================================================================

void	Commander::begin(Stream *sPort){
	inPort = sPort;
	outPort = sPort;
	//portAttached = true;
	//inPort->println("Port Attached");
	setup();
}
//==============================================================================================================

void Commander::setup(){
	resetBuffer();
	
}
//==============================================================================================================

bool Commander::update(){
	//returns true if a command was handled
	bool retStatus = false;
	
	if(commandParserEnabled && inPort){
		while(inPort->available()){
			//inPort->println("Reading Data");
			int inByte = inPort->read();
			if(echoTerminal) outPort->write(inByte);
			if(echoToAlt && altPort) {
				altPort->write(inByte);
			}
      processBuffer(inByte);
    }
    //copy any pending characters back from the alt inPort
		if(echoToAlt && altPort) while(altPort->available()) { outPort->write(altPort->read()); }
		//If a newline is detected, try and handle the command
    if(newLine == true) retStatus =  handleCommand(); //returns true if data was unpacked
	}	else if(!commandParserEnabled && echoToAlt && inPort && altPort){
			//pass data between ports
			while(altPort->available()) outPort->write(altPort->read());
			while(inPort->available()) altPort->write(inPort->read());
	}
  return retStatus;
}
//==============================================================================================================

void Commander::setOutputPort(Stream *oPort){
	//Set the output port if you want it to be different than the input port
	outPort = oPort;
}

void  Commander::setBuffer(uint16_t buffSize){
	bufferSize = buffSize;
	bufferString.reserve(bufferSize);
}
//==============================================================================================================


void Commander::attachCommands(const commandList_t *commands, int size){
	commandList = commands;
	//numOfCmds = sizeof(myCommands) /  sizeof(myCommands[0]); //calculate the number of commands so we know the array bounds
	commandListEntries = size / sizeof(commandList_t); //size is the size of the whole array, not the individual entries
	computeLengths();
}
//==============================================================================================================
void Commander::attachAltPort(Stream *aPort){
	altPort = aPort;
	//bool altPortAttached = true;
}
//==============================================================================================================


int Commander::getInt(){
	String subStr = bufferString.substring(endIndexOfLastCommand);
	return subStr.toInt();
	
}
float Commander::getFloat(){
	String subStr = bufferString.substring(endIndexOfLastCommand);
	return subStr.toFloat();
	
}
double Commander::getDouble(){
	String subStr = bufferString.substring(endIndexOfLastCommand);
	return subStr.toDouble();
	
}
//==============================================================================================================



bool Commander::containsTrue(){
	if(bufferString.indexOf(" true") != -1) return true;
	return false;
}
//==============================================================================================================
bool Commander::containsOn(){
	if(bufferString.indexOf(" on") != -1) return true;
	return false;
}


//==============================================================================================================


void Commander::computeLengths(){
	//compute the length of each command
	if(commandListEntries == 0) return;
	commandLengths = new uint8_t[commandListEntries];
	for(int n = 0; n < commandListEntries; n++){
		commandLengths[n] = getLength(n);
		if(commandLengths[n] > longestCommand) longestCommand = commandLengths[n];
	}
}

uint8_t Commander::getLength(uint8_t indx){
	uint8_t length = 0;
	for(uint8_t n = 0; n < 128; n++){
		if(commandList[indx].commandString[n] != NULL) length++;
		else return length;
	}
}
//==============================================================================================================
bool Commander::handleCommand(){
  commandVal = matchCommand();
	
  bool returnVal = false;
  if(commandVal == UNKNOWN_COMMAND) {
    //Unknown command
		unknownCommand();
    returnVal = 0; //call an unknown command function
  }
  else if(commandVal < 0){
		//Internal command - for controlling commander
    returnVal = 0;
  }
  else {
		
		endIndexOfLastCommand = commandLengths[commandVal];
		//call the appropriate function from the function list and return the result
    returnVal = commandList[commandVal].handler(*this);
  }
  resetBuffer();
  return returnVal;
}
//==============================================================================================================
void  Commander::processBuffer(int dataByte){
  if(dataByte == -1) return; //no actual data to process
  switch(parseState){
    case WAITING_FOR_START:
      //DEBUG.println("Waiting for Start");
      if( isCommandStart(dataByte) ) {
        parseState = BUFFERING_PACKET;
        writeToBuffer(dataByte);
        if(bufferFull) resetBuffer();
      }
      break;
    case BUFFERING_PACKET:
      //DEBUG.println("Buffering");
      writeToBuffer(dataByte);
      if(bufferFull) resetBuffer();//dump the buffer
      if(newLine == true) {
        parseState = PACKET_RECEIVED;
        //DEBUG.println("Newline Detected");
      } //unpack the data
      break;
    case PACKET_RECEIVED:
      //DEBUG.println("Packet received ...?");
      //if you get here then the packet has not been unpacked and more data is coming in. It may be a stray newline ... do nothing
      break;  
  }
}
//==============================================================================================================

void Commander::writeToBuffer(int dataByte){
	if(bytesWritten == bufferSize-1){
    bufferFull = true; //buffer is full
    return;
  }
  //buf[bytesWritten] = dataByte;
  bufferString += (char)dataByte;
  if(dataByte == endOfLine) newLine = true;
  bytesWritten++;
}
//==============================================================================================================

void Commander::resetBuffer(){
	bytesWritten = 0;
  newData = false;
  newLine = false;
  bufferFull = false;
  parseState = WAITING_FOR_START;
  if(isCommandPending){
    bufferString = pendingCommandString;
    isCommandPending = false;
    pendingCommandString = "";
    
  }else bufferString = "";
}
//==============================================================================================================

int Commander::matchCommand(){
  //loop through the command list and see if it appears in the String
	int indexOfLongest = -1;
	uint8_t lastLength = 0;
  for(uint8_t n = 0; n < commandListEntries; n++){
		//check each line for a match,
		//if you get a match, check to see if the command is longer than the last stored length
		//if it is then update the last length and store the index
		if( bufferString.startsWith( commandList[n].commandString ) && commandLengths[n] > lastLength) {
			lastLength = commandLengths[n];
			indexOfLongest = (int)n;
		}
  }
	if(indexOfLongest > -1){
		return (int)indexOfLongest;
	}
	//Now check any default commands - these can be overridden if found in the command list
	for(uint16_t n = 0; n < internalCommandItems; n++){
		//String intCmdLine = 
		if(bufferString.startsWith( internalCommands[n] )){
			//call the internal command function
			return handleInternalCommand(n);
		}
	}
  return UNKNOWN_COMMAND;
}
//==============================================================================================================


int Commander::handleInternalCommand(uint16_t internalCommandIndex){
	switch(internalCommandIndex){
		case 0: //help
			printCommandList();
			break;
		case 1: //?
			printCommanderVersion();
			break;
		case 2: //CMDR echo 
			echoTerminal = containsTrue();
			print("Echo Terminal ");
			echoTerminal ? println("true") : println("false");
			break;
		case 3: //CMDR echo alt 
			echoToAlt = containsTrue();
			print("Echo Alt ");
			echoToAlt ? println("true") : println("false");
			break;
		case 4: //CMDR enable
			commandParserEnabled = containsTrue();
			print("Commander Enabled ");
			commandParserEnabled ? println("true") : println("false");
			break;
		case 5: //CMDR enable
			errorMessagesEnabled = containsTrue();
			print("Error Messages Enabled ");
			errorMessagesEnabled ? println("true") : println("false");
			break;
	}
	return -1;
}

//==============================================================================================================
void Commander::unknownCommand(){
	if(errorMessagesEnabled){
		println("ERR: Unknown Command");
	}
}
//==============================================================================================================

bool Commander::isCommandChar(char dataByte){
  //Command chars are letters and a few other characters
  //DEBUG.print("Databyte is ");
  //DEBUG.println(dataByte);
  if(dataByte < 63 || dataByte > 126) return false;
  return true;
}

//==============================================================================================================

bool Commander::isCommandStart(char dataByte){
  //The start of the command must be a typeable character
  if( dataByte > 31 &&  dataByte < 127) return true;
  return false;
}

//==============================================================================================================

bool Commander::isEndOfLine(char dataByte){
  if(dataByte == 13 || dataByte == 10) return true;
  return false;
}

//==============================================================================================================


bool Commander::printCommandList(){
	  //Prints all the commands
  uint8_t n = 0;
  //int length1 = 0;
  println("User Commands:");
  for(n = 0; n < commandListEntries; n++){
		write(' ');
    print(commandList[n].commandString);
		write(' ');
    //length1 = commands[n].commandString.length();
    //for(int i = 0; i < (32-commandList[n].commandString.length()); i++){
      //add whitespace
			uint8_t whiteSpaces = longestCommand - commandLengths[n];
			for(int ws = 0; ws < whiteSpaces; ws++)  write(' ');
    //}
    write('|');
		write(' ');
    println(commandList[n].manualString);
  }
  println("Internal Commands:");
	for(n = 0; n < internalCommandItems; n++){
		write(' ');
    println(internalCommands[n]);
  }
  return 0;
}
//==============================================================================================================

bool Commander::printCommanderVersion(){
	println(CommanderVersionNumber);
	return 0;
}
//==============================================================================================================
