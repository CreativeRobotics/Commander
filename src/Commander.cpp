#include "Commander.h"

//Initialise the array of internal commands with the constructor
Commander::Commander(): internalCommandArray ({ "U",
																								"X",
																								"?",
																								"help",
																								"echo",
																								"echox",
																								"errors"}){
	bufferString.reserve(bufferSize);
	ports.settings.reg = COMMANDER_DEFAULT_REGISTER_SETTINGS;
	commandState.reg = COMMANDER_DEFAULT_STATE_SETTINGS;
}
//==============================================================================================================
Commander::Commander(uint16_t reservedBuffer): internalCommandArray ({ 	"U",
																																				"X",
																																				"?",
																																				"help",
																																				"echo",
																																				"echox",
																																				"errors"}){
	//bufferString.reserve(bufferSize);
	bufferSize = reservedBuffer;
	bufferString.reserve(bufferSize);
	ports.settings.reg = COMMANDER_DEFAULT_REGISTER_SETTINGS;
	commandState.reg = COMMANDER_DEFAULT_STATE_SETTINGS;
}
//==============================================================================================================

void	Commander::begin(Stream *sPort){
	ports.inPort = sPort;
	ports.outPort = sPort;
	//portAttached = true;
	//ports.inPort->println("Port Attached");
	setup();
}
//==============================================================================================================
void	Commander::begin(Stream *sPort, const commandList_t *commands, uint32_t size){
	ports.inPort = sPort;
	ports.outPort = sPort;
	attachCommands(commands, size);
	//print("CMD: Size is: ");
	//println(size);
	resetBuffer();
}
//==============================================================================================================
void	Commander::begin(Stream *sPort, Stream *oPort, const commandList_t *commands, uint32_t size){
	ports.inPort = sPort;
	ports.outPort = oPort;
	attachCommands(commands, size);
	resetBuffer();
}
//==============================================================================================================

bool Commander::update(){
	if(!ports.inPort) return 0; //don't bother if there is no stream attached
	//Check if streamOn is true and process it if it is.
	if(commandState.bit.dataStreamOn) return streamData();
	//returns true if any bytes left to read
	if(commandState.bit.isCommandPending){
		//there is a command still in the buffer, process it now
		//print("Handling pending command. Buffer:");
		//println(bufferString);
		//printBuffer();
		commandState.bit.isCommandPending = false;
		commandState.bit.commandHandled = !handleCommand();
		return (bool)ports.inPort->available(); //return true if any bytes left to read
	}

	commandState.bit.commandHandled = false;
	if(ports.settings.bit.commandParserEnabled){
		while(ports.inPort->available()){
			int inByte = ports.inPort->read();
			echoPorts(inByte);
      if(processBuffer(inByte)) break; //break out of here - an end of line or reload was found so unpack and handle the command 
    }
    //copy any pending characters back from the alt ports to inPort
		if(ports.settings.bit.echoToAlt && ports.altPort && !ports.settings.bit.locked) while(ports.altPort->available()) { ports.outPort->write(ports.altPort->read()); }
		//If a newline was detected, try and handle the command
    if(commandState.bit.newLine == true){
			#if defined BENCHMARKING_ON
				benchmarkStartTime1 = micros();
			#endif
			//print("Handling new command. Buffer:");
			//println(bufferString);
			//printBuffer();
			commandState.bit.commandHandled = !handleCommand(); //returns true if there was a problem
		} 
	}	else bridgePorts();
	#if defined BENCHMARKING_ON
	  if(commandState.bit.commandHandled) {
			benchmarkTime1 = micros()-benchmarkStartTime1;
			println();
			print("Time (Start to Finish):\t");
			println( ((float)benchmarkTime1)/1000000.0, 6 );
			print("Time (Command Match):\t");
			println( ((float)benchmarkTime2)/1000000.0, 6); 
			print("Time (List query):\t");
			println( ((float)benchmarkTime3)/1000000.0, 6);
			print("Time (Handler):\t\t");
			println( ((float)benchmarkTime4)/1000000.0, 6);
			print("Counter:\t\t");
			println( benchmarkCounter );
			benchmarkCounter = 0;
		}
	#endif
	return (bool)ports.inPort->available(); //return true if any bytes left to read
}
//==============================================================================================================

bool Commander::streamData(){
	bufferString = "";//clear the buffer so we can fill it with any new chars
	bytesWritten = 0;
	commandState.bit.bufferFull = false;
	
	while(ports.inPort->available()){
		int inByte = ports.inPort->read();
		if(inByte == EOFChar && !ports.settings.bit.dataStreamMode){
			//println("EOF Found, tidying up");
			commandState.bit.dataStreamOn = false;
			//get rid of any newlines or CRs in the stream
			while(ports.inPort->peek() == '\n' || ports.inPort->peek() == '\r') ports.inPort->read();
			//call the handler again so it can clean up and close anything that needs closing
			commandState.bit.commandHandled = !handleCustomCommand();
			resetBuffer();
			printCommandPrompt();
			return (bool)ports.inPort->available(); //return true if any bytes left to read
		}
		//write incoming data to the buffer
		writeToBuffer(inByte);
		//echo to ports if configured
		echoPorts(inByte);
		//call the handler if you fill the buffer, then return so everything is reset
		if(bytesWritten == bufferSize-1 || !ports.inPort->available()) {
			
			//println("Buffer ready, calling handler");
			commandState.bit.commandHandled = !handleCustomCommand();
			
			//println("Clearing buffer");
			bufferString = "";//clear the buffer so we can fill it with any new chars
			bytesWritten = 0;
			resetBuffer();
			return (bool)ports.inPort->available(); //return true if any bytes left to read
		}
	}
	return (bool)ports.inPort->available(); //return true if any bytes left to read
}

//Echo incoming to out and alt ports
void Commander::echoPorts(int portByte){
	if(ports.settings.bit.locked) return;
	if(ports.settings.bit.echoTerminal) 							ports.outPort->write(portByte);
	if(ports.settings.bit.echoToAlt && ports.altPort) ports.altPort->write(portByte);
}
//==============================================================================================================

//copy data between ports
void Commander::bridgePorts(){
	if(!ports.settings.bit.commandParserEnabled && ports.settings.bit.echoToAlt && ports.altPort){
			//pass data between ports
			while(ports.altPort->available()) ports.outPort->write(ports.altPort->read());
			while(ports.inPort->available()) ports.altPort->write(ports.inPort->read());
	}
}



//==============================================================================================================


bool Commander::feed(Commander& Cmdr){
	//Feed the payload of a different commander object to this one
	//Copy the String buffer then handle the command
	
	bufferString = Cmdr.getPayload();

	commandPrompt(OFF); //dsiable the prompt so it doesn't print twice
	commandState.bit.commandHandled = !handleCommand(); //try and handle the command

	if( ports.settings.bit.multiCommanderMode == false ) commandPrompt(ON); //re-enable the prompt if in single commander mode so it prints on exit
	return commandState.bit.commandHandled;
}
//==============================================================================================================


bool Commander::hasPayload(){
	//returns true if there is anything except an end of line after the command
	if(bufferString.charAt(endIndexOfLastCommand) != endOfLineChar) return true;
	return false;
}
//==============================================================================================================
String Commander::getPayload(){
	return bufferString.substring(endIndexOfLastCommand+1);
}
//==============================================================================================================
String Commander::getPayloadString(){
	//return the payload minus any newline
	if(bufferString.substring(endIndexOfLastCommand+1).indexOf('\n') > -1){
		return bufferString.substring(endIndexOfLastCommand+1, bufferString.indexOf('\n'));
	}
	return bufferString.substring(endIndexOfLastCommand+1);
}
//==============================================================================================================

bool Commander::feedString(String newString){
	//Feed a string to commander and process it - bypassing any read of the serial ports
	bufferString = newString;
	if( !isEndOfLine(bufferString.charAt( bufferString.length()-1 ) ) ) bufferString += endOfLineChar;//append an end of line character if none there
	commandPrompt(OFF);
	commandState.bit.commandHandled = !handleCommand();
	if( ports.settings.bit.multiCommanderMode == false ) commandPrompt(ON); //re-enable the prompt if in single commander mode so it prints on exit
	return commandState.bit.commandHandled;
}//==============================================================================================================

void Commander::loadString(String newString){
	//Load a string to commander for processing the next time update() is called
	bufferString = newString;
	if( !isEndOfLine(bufferString.charAt( bufferString.length()-1 ) ) ) bufferString += endOfLineChar;//append an end of line character if none there
	commandState.bit.isCommandPending = true; 
	return;
}
//==============================================================================================================


bool Commander::endLine(){
	//add a newline to the buffer and process it - used for reading the last line of a file 
	bufferString+= endOfLineChar;
	commandState.bit.commandHandled = !handleCommand();
	return commandState.bit.commandHandled;
}
//==============================================================================================================


void Commander::transfer(Commander& Cmdr){
	//Transfer command FROM the attached object TO this one
	//the attached ports need to be copied to this one - we assume the user has backed them up first
	ports = Cmdr.getPortSettings();
	if( ports.settings.bit.multiCommanderMode ) Cmdr.commandPrompt(OFF); //disable the prompt for the other commander
  printCommandPrompt();
}
//==============================================================================================================

bool Commander::transferTo(const commandList_t *commands, uint32_t size, String newName){
	//Transfer command to the new command array
	attachCommands(commands, size);
	commanderName = newName;
	if( hasPayload() ){
    //Serial.println("handing payload to get command list");
		//bufferString = bufferString.substring(Cmdr.endIndexOfLastCommand+1);
		bufferString.remove(0, endIndexOfLastCommand+1);
		//Serial.print(bufferString);
		//keep this command prompt disabled if it wasn't already
		commandPrompt(OFF); //dsiable the prompt so it doesn't print twice
		commandState.bit.commandHandled = !handleCommand(); //try and handle the command
		if( ports.settings.bit.multiCommanderMode == false ) commandPrompt(ON); //re-enable the prompt if in single commander mode so it prints on exit
		return true;
  }
	return false;
}
//==============================================================================================================

void Commander::transferBack(const commandList_t *commands, uint32_t size, String newName){
	//Transfer command to the new command array
	attachCommands(commands, size);
	commanderName = newName;

	//return false;
}
//==============================================================================================================

void Commander::attachOutputPort(Stream *oPort){
	//Set the output port if you want it to be different than the input port
	ports.outPort = oPort;
}
//==============================================================================================================

void Commander::attachAltPort(Stream *aPort){
	ports.altPort = aPort;
}
//==============================================================================================================

void Commander::attachInputPort(Stream *iPort){
	ports.inPort = iPort;
}

//==============================================================================================================

void  Commander::setBuffer(uint16_t buffSize){
	bufferSize = buffSize;
	bufferString.reserve(bufferSize);
}
//==============================================================================================================


void Commander::attachCommands(const commandList_t *commands, uint32_t size){
	commandList = commands;
	//numOfCmds = sizeof(myCommands) /  sizeof(myCommands[0]); //calculate the number of commands so we know the array bounds
	commandListEntries = (uint16_t)(size / sizeof(commandList_t)); //size is the size of the whole array, not the individual entries
	computeLengths();
}
//==============================================================================================================
void Commander::quickSetHelp(){
	
	if( bufferString.indexOf("help") > -1 )	commandState.bit.quickHelp = true;
	else commandState.bit.quickHelp = false;
}
//==============================================================================================================

bool Commander::quickSet(String cmd, int& var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(qSetHelp(cmd)) return 0;
	String sub = bufferString.substring( qSetSearch(cmd)+1 );
	if(isNumber(sub)) {
		var = sub.toInt();
		return true;
	}
	return false;
}
//==============================================================================================================
bool Commander::quickSet(String cmd, float& var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(qSetHelp(cmd)) return 0;
	String sub = bufferString.substring( qSetSearch(cmd)+1 );
	if(isNumber(sub)) {
		var = sub.toFloat();
		return true;
	}
	return false;
}
//==============================================================================================================
bool Commander::quickSet(String cmd, double& var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(qSetHelp(cmd)) return 0;
	String sub = bufferString.substring( qSetSearch(cmd)+1 );
	if(isNumber(sub)) {
		//var = sub.toDouble(); //not working yet on ESP32
		var = sub.toFloat();
		return true;
	}
	return false;
}
//==============================================================================================================
int Commander::qSetSearch(String &cmd){
	int idx = bufferString.indexOf(cmd);
	if(idx < 0) return false;
	//idx += cmd.length();
	return idx + cmd.length();
	//if(cmd.length() == 1) idx++;
	//else 								idx = bufferString.indexOf(" ", idx+1); //find the next space
	//return idx;
}
//==============================================================================================================

bool Commander::qSetHelp(String &cmd){
	if(commandState.bit.quickHelp){
		print("  ");
		println(cmd);
		return 1;
	}
	return 0;
}
//==============================================================================================================

void  Commander::quickGet(String cmd, int var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(commandState.bit.quickHelp){
		print("  ");
		println(cmd);
		return;
	}
	if(bufferString.indexOf(cmd) > -1){
		print(cmd);
		print(" ");
		println(var);
	}
}
//==============================================================================================================
void  Commander::quickGet(String cmd, float var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(commandState.bit.quickHelp){
		print("  ");
		println(cmd);
		return;
	}
	if(bufferString.indexOf(cmd) > -1){
		print(cmd);
		print(" ");
		println(var);
	}
}
//==============================================================================================================
bool Commander::getFloat(float &myFloat){
	if(tryGet()){
		//Parse it to the variable
		String subStr = bufferString.substring(dataReadIndex);
		myFloat = subStr.toFloat();
		//if there is no space next, set dataReadIndex to zero and return true - you parsed an int, but next time it will fail.
		nextNumberDelimiter();
		return true; //nextSpace();

	}else return 0;
}
//==============================================================================================================

bool Commander::getDouble(double &myDouble){
	if(tryGet()){
		//Parse it to the variable
		String subStr = bufferString.substring(dataReadIndex);
		myDouble = subStr.toFloat();//subStr.toDouble(); //toDouble() not working on ESP32
		nextNumberDelimiter();
		return true; //nextSpace();
	}else return 0;
}
//==============================================================================================================

bool Commander::getString(String &myString){
	if(!hasPayload() || dataReadIndex < endIndexOfLastCommand) return 0;
	//myString = getPayloadString();
	dataReadIndex++;
	int start = dataReadIndex;
	//print("start= ");
	//println(start);
	if(nextTextDelimiter()){
		//print("end= ");
		//println(dataReadIndex);
		myString = bufferString.substring(start, dataReadIndex);
		
	}else myString = bufferString.substring(start, bufferString.length()-1);
	//print("String= ");
	//println(myString);
	return 1;
}
//==============================================================================================================

uint8_t Commander::countItems(){
	//Returns the number of items in the payload. Items are any substrings with a space, delimChar or endOfLineChar at each end.
	uint8_t items = 0;
	bool state = 0;
	if(!hasPayload()) return 0; //has payload returns true even if there is only a delimiter
	
	for(uint16_t n = endIndexOfLastCommand; n < bufferString.length(); n++){
		if( !isTextDelimiter(bufferString.charAt(n))  && !state ){
			//if NOT a delimiter or end of line and you are NOT in a valid string, you have found the start of a valid string, so cound it and change state
			if( bufferString.charAt(n) == endOfLineChar ) return items;
			items++;
			state = !state;
		}
		else if( isTextDelimiter(bufferString.charAt(n)) && state ){
			//if a delimiter and IN a valid string, go back to waiting for the end of a delimiter
			state = !state;
		}
	}
	return items;
}
//==============================================================================================================


String 		Commander::getCommandItem(uint16_t commandItem){
	if(commandItem >= commandListEntries ) return "";
	String line = "\t";
	line.concat(commandList[commandItem].commandString);
	line.concat(getWhiteSpace(longestCommand - commandLengths[commandItem]));
	line.concat("| ");
	line.concat(commandList[commandItem].manualString);
	return line;
}
//==============================================================================================================



String Commander::getInternalCommandItem(uint8_t internalItem){
	if(internalItem >= INTERNAL_COMMAND_ITEMS ) return "";
	String line = "\t";
	if(internalItem > 3){
		line.concat(internalCommandArray[internalItem]);
		line.concat(" (on/off)");
	}else line.concat(internalCommandArray[internalItem]);
	return line;
}
//==============================================================================================================



//Try to find the next numeral from where readIndex is
bool Commander::tryGet(){
	if(dataReadIndex < endIndexOfLastCommand){
		//println("tryget found no payload");
		return 0; //nothing to see here
	}
	int indx = findNumeral(dataReadIndex); //find the next valid number string start
	
	//print("findNumeral returned ");
	//println(indx);
	if(indx < 0){ //return if it doesn't exist
		//println("returning false ");
		dataReadIndex = 0;
		return 0;
	}
	dataReadIndex = indx;
	//println("returning true");
	return 1;
}
//==============================================================================================================
//find the next space, set readIndex to it
bool Commander::nextTextDelimiter(){
	for(uint16_t n = dataReadIndex; n < bufferSize; n++){
		if(isTextDelimiter(bufferString.charAt(n))) {
			//print("Next delimiter found at ");
			//println(n);
			dataReadIndex = n;
			return 1;
		}
	}
	dataReadIndex = 0;
	return 0;
	/*
	
	int indx = bufferString.indexOf(" ", dataReadIndex);
	if(indx > 0) {
		//if it exists set the read index to it for next time
		dataReadIndex = (uint16_t)indx;
		return 1;
	}else dataReadIndex = 0;
	return 0;*/
}
//==============================================================================================================
//find the next delimiter, set readIndex to it
//Delimiter is anything except - . and any numeral 0-9
bool Commander::nextNumberDelimiter(){
	for(uint16_t n = dataReadIndex; n < bufferSize; n++){
		if(isNumberDelimiter(bufferString.charAt(n))) {
			//print("Next delimiter found at ");
			//println(n);
			dataReadIndex = n;
			return 1;
		}
	}
	dataReadIndex = 0;
	return 0;
}
//==============================================================================================================
void Commander::printCommandPrompt(){
	if(!ports.settings.bit.commandPromptEnabled) return;
		print(commanderName);
		print(promptChar);
}
//==============================================================================================================
bool Commander::containsTrue(){
	if(bufferString.indexOf(" true") != -1) return true;
	else if(bufferString.indexOf(" TRUE") != -1) return true;
	return false;
}
//==============================================================================================================
bool Commander::containsOn(){
	if(bufferString.indexOf(" on") != -1) return true;
	else if(bufferString.indexOf(" ON") != -1) return true;
	return false;
}

//==============================================================================================================


void Commander::computeLengths(){
	//compute the length of each command
	if(commandListEntries == 0) return;
	//delete the current array of not NULL
	if(commandLengths) delete [] commandLengths;
	commandLengths = new uint8_t[commandListEntries];
	for(int n = 0; n < commandListEntries; n++){
		commandLengths[n] = getLength(n);
		if(commandLengths[n] > longestCommand) longestCommand = commandLengths[n];
	}
}

//==============================================================================================================


uint8_t Commander::getLength(uint8_t indx){
	uint8_t length = 0;
	for(uint8_t n = 0; n < 128; n++){
		if(commandList[indx].commandString[n] != NULL) length++;
		else return length;
	}
}
//==============================================================================================================
bool Commander::handleCommand(){
	//Handle command should return an error (true) if the command wasn't handled
	//ignore any stray end of line characters
	//This is handled when processing the buffer
	//if(bufferString.length() == 1 && bufferString.charAt(0) == endOfLineChar) return 0;
	if(ports.settings.bit.locked && ports.settings.bit.useHardLock){
		//if the command string starts with unlock then handle unlocking
		tryUnlock();
		resetBuffer();
		if(!ports.settings.bit.locked){
			println(unlockMessage);
			printCommandPrompt();
		}
		return 0;
	}
	 //write a newline if the command prompt is enabled so reply messages appear on a new line
	if(ports.settings.bit.commandPromptEnabled && !ports.settings.bit.echoTerminal) write('\n');
  
	if(ports.settings.bit.autoFormat) startFormatting();
	//Match command will handle internal commands 
	commandVal = matchCommand();
  bool returnVal = false;
	switch(commandVal){
	case INTERNAL_COMMAND:
		//Comment or internal comnand, nothing to see here, move along.
		//returnVal = 0;
		break;
	case CUSTOM_COMMAND:
			if(ports.settings.bit.locked == true) break;
			returnVal = handleCustomCommand();
			if(returnVal == 1) returnVal = handleUnknown();
		break;
	case COMMENT_COMMAND:
		handleComment();
		//returnVal = 0; //do nothing - comment lines are ignored
		break;
	case UNKNOWN_COMMAND:
		//Unknown command
		returnVal = handleUnknown(); //unknown command function
		break;
	default:
		if(ports.settings.bit.locked == true) break;
		//anything > -1 should be a command
		//user command
		endIndexOfLastCommand = commandLengths[commandVal];
		dataReadIndex = endIndexOfLastCommand;
		//call the appropriate function from the function list and return the result
		if(commandVal < commandListEntries){
			#if defined BENCHMARKING_ON
				benchmarkStartTime4 = micros();
			#endif
			returnVal = commandList[commandVal].handler(*this);
			#if defined BENCHMARKING_ON
				benchmarkTime4 = micros() - benchmarkStartTime4;
			#endif
		}
		break;
	}
  resetBuffer();
	//ports.settings.bit.commandPromptEnabled ? println("prompt on") : println("prompt off");
	printCommandPrompt();
  return returnVal;
}
//==============================================================================================================

bool Commander::handleUnknown(){
	if(defaultHandler != NULL) return defaultHandler(*this);
	
	if(ports.settings.bit.errorMessagesEnabled){
		print(F("Command: \'"));
		print(bufferString.substring(0, bufferString.length()-1));
		println(F("\' not recognised"));
	}
	return 0;
}
//==============================================================================================================

void Commander::tryUnlock(){
	//try and unlock commander
	if(bufferString.indexOf('U') == 0){
		//check passphrase:
		//println("Checking passphrase");
		if(passPhrase == NULL || checkPass()) unlock();
	}
}
//==============================================================================================================

bool Commander::checkPass(){
	if(bufferString.indexOf(*passPhrase))	return true;
	return false;
}
//==============================================================================================================

void Commander::handleComment(){
	//if comments are to be printed then print out the buffer
	if(ports.settings.bit.printComments){
		print(bufferString);
	}
}
//==============================================================================================================

bool  Commander::processBuffer(int dataByte){
  if(dataByte == -1) return false; //no actual data to process
	if(commandState.bit.bufferState == BUFFER_WAITING_FOR_START){
		//if you are waiting for the start of a line, and get an end of line character, or a CR character and these should be ignored, ignore it and return
		if(isEndOfLine(dataByte) || (dataByte == '\r' && ports.settings.bit.stripCR) ) return false;
    if(dataByte == reloadCommandChar){
			commandState.bit.newLine = true;
			if(ports.settings.bit.echoTerminal) ports.outPort->print(bufferString); //print the old buffer
			//if(ports.inPort->peek() == ) ports.inPort->read();
			//print("reloading: ");
			ports.outPort->print(bufferString);
			return true;
		}else {
			//println("Start buffering");
			commandState.bit.bufferState = BUFFER_BUFFERING_PACKET;
			bufferString = "";//clear the buffer
		}
	}
	//write('.');
  writeToBuffer(dataByte);
  if(commandState.bit.bufferFull) resetBuffer();//dump the buffer
  if(commandState.bit.newLine) 		return true; //return true because we have a newline
	return false;
}
//==============================================================================================================

void Commander::writeToBuffer(int dataByte){
	if(bytesWritten == bufferSize-1){
    commandState.bit.bufferFull = true; //buffer is full
		if(ports.settings.bit.errorMessagesEnabled) println("ERR: Buffer Overflow");
    return;
  }
  //if the character is not a cr, or if ignore cr is false, add it to the buffer
  if(dataByte != '\r' || !ports.settings.bit.stripCR) bufferString += (char)dataByte;
  if(dataByte == endOfLineChar) commandState.bit.newLine = true;
  bytesWritten++;
}
//==============================================================================================================

void Commander::resetBuffer(){
	bytesWritten = 0;
  commandState.bit.newLine = false;
  commandState.bit.bufferFull = false;
	commandState.bit.bufferState =  BUFFER_WAITING_FOR_START;
	commandState.bit.prefixMessage = false;
	commandState.bit.postfixMessage = false;
	commandState.bit.newlinePrinted = true;
}
//==============================================================================================================
//return the index of the command, or handle the internal commands
int Commander::matchCommand(){
	#if defined BENCHMARKING_ON
		benchmarkStartTime2 = micros();
		benchmarkStartTime3 = micros();
	#endif  //loop through the command list and see if it appears in the String
	int indexOfLongest = -1;
	uint8_t lastLength = 0;
	//first check comment char
	if(bufferString.charAt(0) == commentChar) {
		#if defined BENCHMARKING_ON
			benchmarkTime3 = micros()-benchmarkStartTime3;
			benchmarkTime2 = micros()-benchmarkStartTime2;
		#endif
		return COMMENT_COMMAND;
	}
	//see if it starts with an int - if so then use the number function
	if( isNumber(bufferString) ) {
		#if defined BENCHMARKING_ON
			benchmarkTime3 = micros()-benchmarkStartTime3;
			benchmarkTime2 = micros()-benchmarkStartTime2;
		#endif
		return CUSTOM_COMMAND;
	}
	//Scan the command array for a match
	#if defined BENCHMARKING_ON
		benchmarkStartTime3 = micros();
	#endif
  for(uint8_t n = 0; n < commandListEntries; n++){
		//if you get a match, check to see if the command is longer than the last stored length, if it is then update the last length and store the index
		//When you get a match, make sure it is followed by a space or newline - otherwise a command like 'st' will be triggered by any string that starts with 'st'
		if( commandLengths[n] > lastLength && checkCommand(n) ) {
			lastLength = commandLengths[n];
			indexOfLongest = (int)n;
		}
  }
	if(indexOfLongest > -1){
		#if defined BENCHMARKING_ON
			benchmarkTime3 = micros()-benchmarkStartTime3;
			benchmarkTime2 = micros()-benchmarkStartTime2;
		#endif
		return indexOfLongest;
	}
	//Now check any default commands - these can be overridden if found in the users command list
	if(!ports.settings.bit.internalCommandsEnabled) return UNKNOWN_COMMAND;
	#if defined BENCHMARKING_ON
		benchmarkStartTime3 = micros();
	#endif
	for(uint16_t n = 0; n < INTERNAL_COMMAND_ITEMS; n++){
		if(checkInternalCommand(n)){
			#if defined BENCHMARKING_ON
					benchmarkTime3 = micros()-benchmarkStartTime3;
					benchmarkTime2 = micros()-benchmarkStartTime2;
					benchmarkStartTime4 = micros();
					int rtv = handleInternalCommand(n);
					benchmarkTime4 = micros() - benchmarkStartTime4;
					return rtv;
				#endif
				return handleInternalCommand(n);
		}
	}
	#if defined BENCHMARKING_ON
		benchmarkTime3 = micros()-benchmarkStartTime3;
		benchmarkTime2 = micros()-benchmarkStartTime2;
	#endif
  return UNKNOWN_COMMAND;
}
//==============================================================================================================

//return true if the command is valid
bool Commander::checkCommand(uint16_t cmdIdx){
	//see if the incoming command matches the command in the array index cmdIdx
	#if defined BENCHMARKING_ON
		benchmarkCounter++;
	#endif
	
	if(bufferString.length() < commandLengths[cmdIdx]+1) return false; //no match if the buffer is shorter than the command+1 (buffer will have the end of line char)
	if(commandLengths[cmdIdx] == 1){
		//This command was a single char, if it is a match then return true if the next char in the buffer is an end of command char (newline, space or delim)
		if(bufferString.charAt(0) != commandList[cmdIdx].commandString[0]) return false;
		return isEndOfCommand(bufferString.charAt(1));
	}
	uint8_t y = commandLengths[cmdIdx]-1;
	for(uint8_t x = 0; x < commandLengths[cmdIdx]; x++){
		if(bufferString.charAt(y) != commandList[cmdIdx].commandString[y])return false;
		if(bufferString.charAt(x) != commandList[cmdIdx].commandString[x])return false;
		y--;
		if(x==y || x+1 == y) return ((bufferString.charAt(y) == commandList[cmdIdx].commandString[y]) & isEndOfCommand( bufferString.charAt(commandLengths[cmdIdx]) ));
	}
	return false; //failed check
}


//==============================================================================================================
bool Commander::checkInternalCommand(uint16_t cmdIdx){
	//see if the incoming command matches the command in the internal index cmdIdx
	#if defined BENCHMARKING_ON
		benchmarkCounter++;
	#endif
	/*	"U",	"X",	"?",	"help",	"echo",	"echox",	"errors"}	*/
	
	switch(cmdIdx){
	case 0:
		if(bufferString.charAt(0) != 'U') return false;
		return isEndOfCommand(bufferString.charAt(1));
	case 1:
		if(bufferString.charAt(0) != 'X') return false;
		return isEndOfCommand(bufferString.charAt(1));
	case 2:
		if(bufferString.charAt(0) != '?') return false;
		return isEndOfCommand(bufferString.charAt(1));
	case 3:
		if(bufferString.charAt(0) != 'h') return false;
		return isEndOfCommand(bufferString.charAt(4));
	case 4:
		if(bufferString.charAt(3) != 'o') return false;
		return isEndOfCommand(bufferString.charAt(4));
	case 5:
		if(bufferString.charAt(4) != 'x') return false;
		return isEndOfCommand(bufferString.charAt(5));
	case 6:
		if(bufferString.charAt(5) != 's') return false;
		return isEndOfCommand(bufferString.charAt(6));
	}
	return 0;
}
//==============================================================================================================

void Commander::printDiagnostics(){
	//print diagnostic data for debugging
	println("Command lengths");
	for(int n = 0; n < commandListEntries; n++){
		println(commandLengths[n]);
	}
	
	print("End Idx Last Cmd ");
	println(endIndexOfLastCommand);
	print("longest Cmd ");
	println(longestCommand);
	print("Num of Cmds ");
	println(commandListEntries);
}

int Commander::handleInternalCommand(uint16_t internalCommandIndex){
	switch(internalCommandIndex){
		case 0: //unlock
			unlock();
			if(ports.settings.bit.errorMessagesEnabled) println(unlockMessage);
			//Lock Command printCommandList();
			break;
		case 1: //?
		  lock();
			if(ports.settings.bit.errorMessagesEnabled) println(lockMessage);
			//Unlock Command printCommanderVersion();
			break;
		case 2: //?
			if( ports.settings.bit.helpEnabled ) printCommanderVersion();
			break;
		case 3: //help
			if( ports.settings.bit.helpEnabled ) printCommandList();
			break;
		case 4: //CMDR echo 
			ports.settings.bit.echoTerminal = containsOn();
			if(ports.settings.bit.errorMessagesEnabled){
				print(F("Echo Terminal "));
				ports.settings.bit.echoTerminal ? println("on") : println("off");
			}
			break;
		case 5: //CMDR echo alt 
			ports.settings.bit.echoToAlt = containsOn();
			if(ports.settings.bit.errorMessagesEnabled){
				print(F("Echo Alt "));
				ports.settings.bit.echoToAlt ? println("on") : println("off");
			}
			break;
		case 6: //CMDR enable error messages
			ports.settings.bit.errorMessagesEnabled = containsOn();
			if(ports.settings.bit.errorMessagesEnabled){
				print(F("Error Messages "));
				ports.settings.bit.errorMessagesEnabled ? println("on") : println("off");
			}
			break;
	}
	return INTERNAL_COMMAND;
}
//==============================================================================================================

bool  Commander::handleCustomCommand(){
	//if the function pointer is NULL then return
	//If not then call the function
	if(customHandler == NULL) return 1;
	return customHandler(*this);
}

//==============================================================================================================
int Commander::findNumeral(uint8_t startIdx){
	//return the index of the start of a number string
	//print("FindNumeral started from ");
	//println(startIdx);
	for(uint8_t n = startIdx; n < bufferSize; n++){
		if(bufferString.charAt(n) == endOfLineChar){
			//println("found EOL");
			return -1;
		}
		if( isNumeral( bufferString.charAt(n) ) ){
			//print("IsNumeral sent ");
			//print(bufferString.charAt(n));
			//print(" from buffer index ");
			//print(n);
			//print(" isNumeral returned true, returning index = ");
			//println(n);
			return n;
		}
		if( bufferString.charAt(n) == 45 && isNumeral( bufferString.charAt(n+1) ) ) {
			//print("isNumeral returned true, after a minus sign, returning index = ");
			//println(n);
			return n;
		}
	}
	return -1;
}
bool Commander::isNumber(String &str){
	//returns true if the first character is a valid number, or a minus sign followed by a number
	if( isNumeral( str.charAt(0) ) ) return true;
	if(str.charAt(0) == 45 && isNumeral( str.charAt(1) )) return true;
	return false;
}
bool Commander::isNumberDelimiter(char ch){
	//returns true if the first character is NOT valid number, minus sign, dot or NL/CR
	if( isNumeral( ch ) || ch == '.' || ch == '-' || ch == '\n' || ch == '\r' || ch == 0) return false;
	return true;
}

bool Commander::isTextDelimiter(char ch){
	//this should be called is delimiter//it looks for a space or the eoc character (=)
	if(ch == delimChar || ch == ' ') return 1;
	return 0;
}
//==============================================================================================================
bool Commander::isNumeral(char ch){
	if(ch > 47 && ch < 58) return true;
	return false;
}
//==============================================================================================================

bool Commander::isEndOfLine(char dataByte){
  if(dataByte == endOfLineChar) return true;
  return false;
}

bool Commander::isEndOfCommand(char dataByte){
	if(dataByte == endOfLineChar) return true;
	if(dataByte == ' ') return true;
	if(dataByte == delimChar) return true;
  return false;
}

//==============================================================================================================


void Commander::printCommandList(){
	  //Prints all the commands
  uint8_t n = 0;
	String cmdLine = " ";
	cmdLine.concat(commanderName);
	cmdLine.concat(F(" User Commands:"));
	println(cmdLine);
  for(n = 0; n < commandListEntries; n++) if(commandList[n].manualString[0] != CMD_HIDE_HELP) println(getCommandItem(n));
	print(F(" Reload character: "));
	println(String(reloadCommandChar));
	print(F(" Comment character: "));
	println(String(commentChar));
	
	if(!ports.settings.bit.internalCommandsEnabled || !ports.settings.bit.printInternalCommands) return;
  println(F(" Internal Commands:"));
	for(n = 0; n < INTERNAL_COMMAND_ITEMS; n++) println(getInternalCommandItem(n));

}

//==============================================================================================================

String Commander::getWhiteSpace(uint8_t spaces){
	String wSpace = " ";
	for(uint8_t n = 0; n < spaces; n++){
		wSpace.concat(' ');
		//write(' ');
	}
	return wSpace;
}
//==============================================================================================================

void Commander::printCommanderVersion(){
	
	print(F("\tCommander version "));
	print(majorVersion);
	print(".");
	print(minorVersion);
	print(".");
	println(subVersion);
	
	print(F("\tEcho terminal: "));
	ports.settings.bit.echoTerminal ? println("On") : println("Off");
	
	print(F("\tEcho to Alt: "));
	ports.settings.bit.echoToAlt ? println("On") : println("Off");
	
	print(F("\tAuto Format: "));
	ports.settings.bit.autoFormat ? println("On") : println("Off");
	
	print(F("\tError messages: "));
	ports.settings.bit.errorMessagesEnabled ? println("On") : println("Off");
	
	print(F("\tIn Port: "));
	ports.inPort ? println("OK") : println("NULL");
	
	print(F("\tOut Port: "));
	ports.outPort ? println("OK") : println("NULL");
	
	print(F("\tAlt Port: "));
	ports.altPort ? println("OK") : println("NULL");
	
	print(F("\tLocked: "));
	ports.settings.bit.locked ? println("Yes") : println("No");
	
	print(F("\tLock: "));
	ports.settings.bit.useHardLock ? println("Hard") : println("Soft");
	
	if(customHandler != NULL)	println( F("\tCustom Cmd OK"));
}
//==============================================================================================================


uint8_t Commander::getInternalCmdLength(const char intCmd[]){
	for(int n = 0; n < 8; n++){
		if(intCmd[n] == NULL) return n;
	}
	return 8;
}
	