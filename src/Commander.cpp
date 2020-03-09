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
void Commander::setDelimiters(String myDelims){
	if(myDelims != "")	delimiters = myDelims;
}
//==============================================================================================================
String Commander::getDelimiters(){
	return delimiters;
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
	//print("CMD: sizeof cmds: ");
	//println(sizeof(commands));
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
		if(ports.settings.bit.echoTerminal) 							print(bufferString);
		else if(ports.settings.bit.commandPromptEnabled) 	println();
		if(ports.settings.bit.echoToAlt && ports.altPort) printAlt(bufferString);
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
			
			//AutoReload
			//if(startOfNextItem)
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
	if(dataReadIndex > 0) return true;
	//if(bufferString.charAt(endIndexOfLastCommand) != endOfLineChar) return true;
	return false;
}
//==============================================================================================================
String Commander::getPayload(){
	return bufferString.substring(dataReadIndex);
	//return bufferString.substring(endIndexOfLastCommand+1);
}
//==============================================================================================================
String Commander::getPayloadString(){
	//return the payload minus any newline
	if(hasPayload()){
		return bufferString.substring(dataReadIndex, bufferString.indexOf('\n'));
	}
	return "";
	/*
	if(bufferString.substring(endIndexOfLastCommand+1).indexOf('\n') > -1){
		return bufferString.substring(endIndexOfLastCommand+1, bufferString.indexOf('\n'));
	}
	return bufferString.substring(endIndexOfLastCommand+1);*/
}
//==============================================================================================================

bool Commander::feedString(String newString){
	//Feed a string to commander and process it - bypassing any read of the serial ports
	if(newString.length() < 2) return commandState.bit.commandHandled; //return if string is not valid - too short for a command and endofline
	bufferString = newString;
	if( !isEndOfLine(bufferString.charAt( bufferString.length()-1 ) ) ) bufferString += endOfLineChar;//append an end of line character if none there
	commandPrompt(OFF);
	commandState.bit.commandHandled = !handleCommand();
	if( ports.settings.bit.multiCommanderMode == false ) commandPrompt(ON); //re-enable the prompt if in single commander mode so it prints on exit
	return commandState.bit.commandHandled;
}//==============================================================================================================

void Commander::loadString(String newString){
	//Load a string to commander for processing the next time update() is called
	if(newString.length() < 2) return; //return if string is not valid - too short for a command and endofline
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
	int sRes = qSetSearch(cmd);
	uint16_t tmp = dataReadIndex;
	if(sRes > 1){
		dataReadIndex = sRes;
		//now get next item
		if(findNextItem() && getInt(var)){
			dataReadIndex = tmp;
			return true;
		}
		dataReadIndex = tmp;
	}
	return false;
}
//==============================================================================================================
bool Commander::quickSet(String cmd, float& var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(qSetHelp(cmd)) return 0;
	int sRes = qSetSearch(cmd);
	uint16_t tmp = dataReadIndex;
	if(sRes > 1){
		dataReadIndex = sRes;
		//now get next item
		if(findNextItem() && getFloat(var)){
			dataReadIndex = tmp;
			return true;
		}
		dataReadIndex = tmp;
	}
	return false;
}
//==============================================================================================================
bool Commander::quickSet(String cmd, double& var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(qSetHelp(cmd)) return 0;
	int sRes = qSetSearch(cmd);
	uint16_t tmp = dataReadIndex;
	if(sRes > 1){
		dataReadIndex = sRes;
		//now get next item
		if(findNextItem() && getDouble(var)){
			dataReadIndex = tmp;
			return true;
		}
		dataReadIndex = tmp;
	}
	return false;
}
//==============================================================================================================
bool Commander::quickSet(String cmd, String& str){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	if(qSetHelp(cmd)) return 0;
	int sRes = qSetSearch(cmd);
	uint16_t tmp = dataReadIndex;
	if(sRes > 1){
		dataReadIndex = sRes;
		//now get next item
		if(findNextItem() && getString(str)){
			dataReadIndex = tmp;
			return true;
		}
		dataReadIndex = tmp;
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
	//Quicksets and gets cannot be chained so flag this so chaining will halt
	commandState.bit.quickSetCalled = true;
	if(commandState.bit.quickHelp){
		print("\t");
		println(cmd);
		return 1;
	}
	return 0;
}
//==============================================================================================================
void  Commander::quickGet(String cmd, int var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	findNextItem();
	if(commandState.bit.quickHelp){
		print("\t");
		println(cmd);
		return;
	}
	if(bufferString.indexOf(cmd) > -1){
		print(cmd);
		print("=");
		println(var);
	}
}
//==============================================================================================================
void  Commander::quickGet(String cmd, float var){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	findNextItem();
	if(commandState.bit.quickHelp){
		print("\t");
		println(cmd);
		return;
	}
	if(bufferString.indexOf(cmd) > -1){
		print(cmd);
		print("=");
		println(var);
	}
}
//==============================================================================================================
void  Commander::quickGet(String cmd, String str){
	//look for the string, if found try and parse an int
	//print help if help was triggered
	findNextItem();
	if(commandState.bit.quickHelp){
		print("\t");
		println(cmd);
		return;
	}
	if(bufferString.indexOf(cmd) > -1){
		print(cmd);
		print("=");
		println(str);
	}
}
//==============================================================================================================
bool Commander::getFloat(float &myFloat){
	if(tryGet()){
		//Parse it to the variable
		String subStr = bufferString.substring(dataReadIndex);
		myFloat = subStr.toFloat();
		if(!findNextItem()) dataReadIndex = 0; 
		return true; //nextSpace();

	}else return 0;
}
//==============================================================================================================
bool Commander::getDouble(double &myDouble){
	if(tryGet()){
		//Parse it to the variable
		String subStr = bufferString.substring(dataReadIndex);
		myDouble = subStr.toFloat(); //todouble not supported on some distributions
		if(!findNextItem()) dataReadIndex = 0; 
		return true; //nextSpace();
	}else return 0;
}
//==============================================================================================================
bool Commander::getString(String &myString){
	if(!hasPayload() || dataReadIndex < endIndexOfLastCommand) return 0;
	int start = 0;
	//New method:
	//println("get String:");
	
	start = dataReadIndex;
	if(bufferString.charAt(dataReadIndex) == '"'){
		findNextDelim();
		if(bufferString.charAt(dataReadIndex-1) == '"') myString = bufferString.substring(start+1, dataReadIndex-1);
		else myString = bufferString.substring(start+1, dataReadIndex);
		if(!findNextItem()) dataReadIndex = 0;
		return 1;
	}
	findNextDelim();
	myString = bufferString.substring(start, dataReadIndex);
	
	//print("Start=");
	//println(start);
	//print("DRIdx=");
	//println(dataReadIndex);
	
	if(!findNextItem()) dataReadIndex = 0;
	return 1;
}
//==============================================================================================================
uint8_t Commander::countItems(){
	//Returns the number of items in the payload. Items are any substrings with a space, delimChar or endOfLineChar at each end.
	uint8_t items = 0;
	bool state = 0;
	if(!hasPayload()) return items; //has payload returns true if an item was found after the command
	items++;
	while(findNextItem()) items++;
	rewind();
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
void Commander::printCommandPrompt(){
	if(!ports.settings.bit.commandPromptEnabled) return;
		print(commanderName);
		print(promptChar);
}
//==============================================================================================================
bool Commander::containsTrue(){
	uint16_t oldIdx = dataReadIndex;
	String str = "";
	getString(str);
	dataReadIndex = oldIdx;
	str.toLowerCase();
	if(str == "true") return true;
	return false;
}
//==============================================================================================================
bool Commander::containsFalse(){
	uint16_t oldIdx = dataReadIndex;
	String str = "";
	getString(str);
	dataReadIndex = oldIdx;
	str.toLowerCase();
	if(str == "false") return true;
	return false;
}
//==============================================================================================================
bool Commander::containsOn(){
	uint16_t oldIdx = dataReadIndex;
	String str = "";
	getString(str);
	dataReadIndex = oldIdx;
	str.toLowerCase();
	if(str == "on") return true;
	return false;
}
//==============================================================================================================
bool Commander::containsOff(){
	uint16_t oldIdx = dataReadIndex;
	String str = "";
	getString(str);
	dataReadIndex = oldIdx;
	str.toLowerCase();
	if(str == "off") return true;
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
	//commandType = matchCommand();
	commandState.bit.commandType =  matchCommand();
  bool returnVal = false;
	switch(commandState.bit.commandType){
	case INTERNAL_COMMAND:
		returnVal = handleInternalCommand(commandIndex);
			
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
	case USER_COMMAND:
		if(ports.settings.bit.locked == true){
			println();
			break;
		}
		//anything > -1 should be a command
		//user command
		endIndexOfLastCommand = commandLengths[commandIndex];
		dataReadIndex = endIndexOfLastCommand;
		if(!findNextItem()) dataReadIndex = 0;
		//call the appropriate function from the function list and return the result
		if(commandIndex < commandListEntries){
			#if defined BENCHMARKING_ON
				benchmarkStartTime4 = micros();
			#endif
			returnVal = commandList[commandIndex].handler(*this);
			#if defined BENCHMARKING_ON
				benchmarkTime4 = micros() - benchmarkStartTime4;
			#endif
		}
		break;
		default:
			returnVal = false;
			break;
	}
  resetBuffer();
	//ports.settings.bit.commandPromptEnabled ? println("prompt on") : println("prompt off");
	printCommandPrompt();
	commandState.bit.chaining = false;
	//return here if this is a comment - comments break chains
	if(commandState.bit.commandType == COMMENT_COMMAND || commandState.bit.quickSetCalled ){
		commandState.bit.quickSetCalled = false;
		return returnVal;
	}
	
	if(commandState.bit.chain || ports.settings.bit.autoChain){
		//startOfNextItem();
		if(dataReadIndex > 0){
			//if(ports.settings.bit.commandPromptEnabled) println();
			loadString(bufferString.substring(dataReadIndex) );
			commandState.bit.chaining = true;
		}
		commandState.bit.chain = false;
	}
  return returnVal;
}
//==============================================================================================================
bool Commander::handleUnknown(){
	if(	commandState.bit.chaining && ports.settings.bit.autoChainSurpressErrors ){
		if(ports.settings.bit.autoChain) findNextItem(); //move index to next delimiter so this item doesn't get chained again
		//print("idx=");
		//println(dataReadIndex);
		//println(bufferString);
		return 0; //do nothing if this was an attempt to chain a command
	}
	if(defaultHandler != NULL) return defaultHandler(*this);
	
	if(ports.settings.bit.errorMessagesEnabled){
		print(F("#Command: \'"));
		print(bufferString.substring(0, bufferString.length()-1));
		println(F("\' not recognised"));
	}
	return 0;
}
//==============================================================================================================
void Commander::tryUnlock(){
	//try and unlock commander
	println("Trying Unlock");
	if(bufferString.indexOf('U') == 0){
		//check passphrase:
		//println("Checking passphrase");
		commandState.bit.commandType = INTERNAL_COMMAND;
		if( checkPass() ) unlock();
	}
	println();
}
//==============================================================================================================
bool Commander::checkPass(){
	/*print("Passphrase: ");
	println(*passPhrase);
	print("Buffer: ");
	println(bufferString);*/
	
	if(passPhrase == NULL){
		//println("passphrase is null");
		return true;
	}
	if(bufferString.indexOf(*passPhrase) > -1){
		//println("Phrase OK");
		return true;
	}
	//println("Checkpass failed");
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
		if(ports.settings.bit.errorMessagesEnabled) println("#ERR: Buffer Overflow");
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
	int indexOfLongest = -1;
	uint8_t lastLength = 0;
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
		
		dataReadIndex = commandLengths[indexOfLongest];
		commandIndex = indexOfLongest;
		return USER_COMMAND;
	}
	//Now check any default commands - these can be overridden if found in the users command list
	if(!ports.settings.bit.internalCommandsEnabled) return UNKNOWN_COMMAND;
	#if defined BENCHMARKING_ON
		benchmarkStartTime3 = micros();
	#endif
	for(uint16_t n = 0; n < INTERNAL_COMMAND_ITEMS; n++){
		if(checkInternalCommand(n)){
			//got a match - return
			#if defined BENCHMARKING_ON
					benchmarkTime3 = micros()-benchmarkStartTime3;
					benchmarkTime2 = micros()-benchmarkStartTime2;
					benchmarkStartTime4 = micros();
					benchmarkTime4 = micros() - benchmarkStartTime4;
					//return rtv;
				#endif
				
			return INTERNAL_COMMAND;
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
		if(!isEndOfCommand( bufferString.charAt(1) ) ) return false;
		dataReadIndex = 1;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 0;
		return true;
	case 1:
		if(bufferString.charAt(0) != 'X') return false;
		if(!isEndOfCommand( bufferString.charAt(1) ) ) return false;
		dataReadIndex = 1;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 1;
		return true;
	case 2:
		if(bufferString.charAt(0) != '?') return false;
		if(!isEndOfCommand( bufferString.charAt(1) ) ) return false;
		dataReadIndex = 1;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 2;
		return true;
	case 3:
		if(bufferString.charAt(0) != 'h') return false;
		if(!isEndOfCommand(bufferString.charAt(4)) || !qcheckInternal(cmdIdx) ) return false;
		dataReadIndex = 4;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 3;
		return true;
	case 4:
		if(bufferString.charAt(3) != 'o') return false;
		if(!isEndOfCommand(bufferString.charAt(4)) || !qcheckInternal(cmdIdx) ) return false;
		dataReadIndex = 4;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 4;
		return true;
	case 5:
		if(bufferString.charAt(4) != 'x') return false;
		if( !isEndOfCommand(bufferString.charAt(5)) || !qcheckInternal(cmdIdx) ) return false;
		dataReadIndex = 5;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 5;
		return true;
	case 6:
		if(bufferString.charAt(5) != 's') return false;
		if(!isEndOfCommand(bufferString.charAt(6)) || !qcheckInternal(cmdIdx) ) return false;
		dataReadIndex = 6;
		endIndexOfLastCommand = dataReadIndex;
		findNextItem();
		commandIndex = 6;
		return true;
	}
	return 0;
}
//==============================================================================================================
bool Commander::qcheckInternal(uint8_t itm){
	//quick check of internal command match
	uint8_t n = 0; //0
	if(bufferString.charAt(n) != internalCommandArray[itm][n]) return false;
	n++; //1
	if(bufferString.charAt(n) != internalCommandArray[itm][n]) return false;
	n++; //2
	if(bufferString.charAt(n) != internalCommandArray[itm][n]) return false;
	n++; //3
	if(bufferString.charAt(n) != internalCommandArray[itm][n]) return false;
	if(itm < 5) return true; //match
	n++; //4
	if(bufferString.charAt(n) != internalCommandArray[itm][n]) return false;
	if(itm < 6) return true; //match
	n++; //5
	if(bufferString.charAt(n) != internalCommandArray[itm][n]) return false;
	return true; //match
}
//==============================================================================================================
//call and end of handler and it will treat the current command as the last in a chain
void Commander::unchain(){
	dataReadIndex = 0;
}
//==============================================================================================================
void Commander::printDiagnostics(){
	//print diagnostic data for debugging
	/*println("Command lengths");
	for(int n = 0; n < commandListEntries; n++){
		print(n);
		print("= ");
		println(commandLengths[n]);
	}*/
	println("DIAGNOSTICS-------------:");
	print("Cmd Idx ");					println(commandIndex);
	print("Data Read Idx ");		println(dataReadIndex);
	print("End Idx Last Cmd ");	println(endIndexOfLastCommand);
	//print("longest Cmd ");			println(longestCommand);
	//print("Num of Cmds ");			println(commandListEntries);
	print("Buffer:");						println(bufferString);
	print("Payload:");					println(getPayload());
	
	println("State bits");
	print("Newline: ");
	commandState.bit.newLine ? println("True") : println("false");
	
	print("bufferFull: ");
	commandState.bit.bufferFull ? println("True") : println("false");
	
	print("isCommandPending: ");
	commandState.bit.isCommandPending ? println("True") : println("false");
	
	print("bufferState: ");
	commandState.bit.bufferState ? println("True") : println("false");
	
	print("commandHandled: ");
	commandState.bit.commandHandled ? println("True") : println("false");
	
	print("quickHelp: ");
	commandState.bit.quickHelp ? println("True") : println("false");
	
	print("prefixMessage: ");
	commandState.bit.prefixMessage ? println("True") : println("false");
	
	print("postfixMessage: ");
	commandState.bit.postfixMessage ? println("True") : println("false");
	
	print("newlinePrinted: ");
	commandState.bit.newlinePrinted ? println("True") : println("false");
	
	print("dataStreamOn: ");
	commandState.bit.dataStreamOn ? println("True") : println("false");
	
	print("chain: ");
	commandState.bit.chain ? println("True") : println("false");
	
	print("chaining: ");
	commandState.bit.chaining ? println("True") : println("false");
	
	print("Cmd Type:");					println( (int)commandState.bit.commandType );
	
	println("-------------END");
}
//==============================================================================================================
int Commander::handleInternalCommand(uint16_t internalCommandIndex){
	String str = "";
	switch(internalCommandIndex){
		case 0: //unlock
			if(checkPass()){
				unlock();
				if(ports.settings.bit.errorMessagesEnabled) println(unlockMessage);
			}else println();
			//Lock Command printCommandList();
			return 0;
			break;
		case 1: //?
		  lock();
			if(ports.settings.bit.errorMessagesEnabled) println(lockMessage);
			//Unlock Command printCommanderVersion();
			return 0;
			break;
		case 2: //?
			if( ports.settings.bit.helpEnabled ) printCommanderVersion();
			break;
		case 3: //help
			if( ports.settings.bit.helpEnabled ) printCommandList();
			return 0;
			break;
		case 4: //CMDR echo 
			if(getString(str)){
				rewind();
				str.toLowerCase();
				if(str == "off") ports.settings.bit.echoTerminal  = false;
				if(str == "on") ports.settings.bit.echoTerminal  = true;
			}
			if(ports.settings.bit.errorMessagesEnabled){
				
				write(commentChar);
				print(F("Echo Terminal "));
				ports.settings.bit.echoTerminal ? println(onString) : println(offString);
			}
			return 0;
			break;
		case 5: //CMDR echo alt 
			if(getString(str)){
				rewind();
				str.toLowerCase();
				if(str == "off") ports.settings.bit.echoToAlt  = false;
				if(str == "on") ports.settings.bit.echoToAlt  = true;
			}
			if(ports.settings.bit.errorMessagesEnabled){
				write(commentChar);
				print(F("Echo Alt "));
				ports.settings.bit.echoToAlt ? println(onString) : println(offString);
			}
			return 0;
			break;
		case 6: //CMDR enable error messages
			if(getString(str)){
				rewind();
				str.toLowerCase();
				if(str == "off") ports.settings.bit.errorMessagesEnabled  = false;
				if(str == "on") ports.settings.bit.errorMessagesEnabled  = true;
			}
			if(ports.settings.bit.errorMessagesEnabled){
				write(commentChar);
				print(F("Error Messages "));
				ports.settings.bit.errorMessagesEnabled ? println(onString) : println(offString);
			}
			return 0;
			break;
	}
	//error
	return 1;
}
//==============================================================================================================
bool  Commander::handleCustomCommand(){
	//if the function pointer is NULL then return
	//If not then call the function
	if(customHandler == NULL) return 1;
	return customHandler(*this);
}
//==============================================================================================================
bool Commander::findNextDelim(){
	//find the next delimiter from wherever you are - ignoring a delimiter if you are already in one
	//Returns true if found or false if end of line
	if( isEndOfLine(bufferString.charAt(dataReadIndex)) ){
		if(DEBUG_INDEXER) println("Delim Search: END OF LINE AT START");
		return 0;
	}
	if(!isDelimiter(bufferString.charAt(dataReadIndex))){
		if(DEBUG_INDEXER) println("Delim Search: Started On item. looking for delim");
		return itemToNextDelim();
	}
	println("Delim Search: Started On delim. looking for item");
	//already in a delimiter, find item first
	if(delimToNextItem()) return itemToNextDelim();
	if(DEBUG_INDEXER) println("Delim Search: END Returning 0");
	return 0;
}
//==============================================================================================================
bool Commander::findNextItem(){
	//find the start of the next item from wherever you are - ignores the item you are alread on
	//Returns true if found or false if end of line
	if( isEndOfLine(bufferString.charAt(dataReadIndex)) ){
		if(DEBUG_INDEXER) println("Item Search: END OF LINE AT START");
		return 0;
	}
	if(isDelimiter(bufferString.charAt(dataReadIndex))){
		if(DEBUG_INDEXER) println("Item Search: Started On Delim. looking for item");
		return delimToNextItem();
	}
	//already in an item, find delim first
	
	if(DEBUG_INDEXER) println("Item Search: Started On Item. looking for delim");
	if(itemToNextDelim()) return delimToNextItem();
	if(DEBUG_INDEXER) println("Item Search: END Returning 0");
	return 0;
}
//==============================================================================================================
bool Commander::delimToNextItem(){
	//move dataReadIndex from a delimiter to the next item
	while(isDelimiter(bufferString.charAt(dataReadIndex))){
		if(isEndOfLine(bufferString.charAt(dataReadIndex))){
			
			if(DEBUG_INDEXER) println("delimToNextItem: END OF LINE");
			return 0;
		}
		dataReadIndex++;
	}
	if(DEBUG_INDEXER) print("delimToNextItem: Item Found at: ");
	if(DEBUG_INDEXER) println(dataReadIndex);
	return 1;
}
//==============================================================================================================
bool Commander::itemToNextDelim(){
		//move dataReadIndex from an item to the next delimiter
		//If you start on a quote, don't stop until you get another
	bool endOnQuote = false;
	if(!ports.settings.bit.ignoreQuotes){
		if(bufferString.charAt(dataReadIndex) == '"'){
			//print("Found open quote at ");
			//println(dataReadIndex);
			dataReadIndex++;
			while(bufferString.charAt(dataReadIndex) != '"'){
				if(isEndOfLine(bufferString.charAt(dataReadIndex))){
					return 0;
				}
				dataReadIndex++;
			}
		}
	}
	while(!isDelimiter(bufferString.charAt(dataReadIndex))){
		if(isEndOfLine(bufferString.charAt(dataReadIndex))){
			if(DEBUG_INDEXER) println("itemToNextDelim: END OF LINE");
			return 0;
		}
		dataReadIndex++;
	}
	if(DEBUG_INDEXER) print("itemToNextDelim: Item Found at: ");
	if(DEBUG_INDEXER) println(dataReadIndex);
	return 1;
}
//==============================================================================================================
bool Commander::isDelimiter(char ch){
	//this should be called is delimiter
	//it looks for a space or the eoc character (=)
	for(uint8_t n = 0; n < delimiters.length(); n++){
		if(ch == delimiters.charAt(n)) return 1;
	}
	//if(ch == delimChar || ch == ' ') return 1;
	return 0;
}

bool Commander::isItem(char ch){
	if(isDelimiter(ch)) return false;
	if(isEndOfLine(ch)) return false;
	if(ch == NULL) return false;
	return true;
}
//==============================================================================================================
void Commander::rewind(){
	//rewind dataReadIndex to the start of the first item
	dataReadIndex = endIndexOfLastCommand;
	findNextItem();
}
//==============================================================================================================
//Try to find the next numeral from where readIndex is
bool Commander::tryGet(){
	if(dataReadIndex < endIndexOfLastCommand){
		//println("tryget found no payload");
		return 0; //nothing to see here
	}
	else if( isNumeral(bufferString.charAt(dataReadIndex)) ) return 1;
	
	else if( bufferString.charAt(dataReadIndex) == '-' && isNumeral(bufferString.charAt(dataReadIndex+1)) ) return 1;
	return 0;
}
//==============================================================================================================
bool Commander::isNumber(String &str){
	//returns true if the first character is a valid number, or a minus sign followed by a number
	if( isNumeral( str.charAt(0) ) ) return true;
	if(str.charAt(0) == 45 && isNumeral( str.charAt(1) )) return true;
	return false;
}
//==============================================================================================================
bool Commander::isNumeral(char ch){
	if(ch > 47 && ch < 58) return true;
	return false;
}
//==============================================================================================================
bool Commander::isEndOfLine(char dataByte){
	return dataByte == endOfLineChar;
  //(dataByte == endOfLineChar) ? return true : return false;
}
//==============================================================================================================
bool Commander::isEndOfCommand(char dataByte){
	if(dataByte == endOfLineChar) return true;
	return isDelimiter(dataByte);
	//if(dataByte == ' ') return true;
	//if(dataByte == delimChar) return true;
  //return false;
}
//==============================================================================================================



void Commander::printCommandList(){
	  //Prints all the commands
  uint8_t n = 0;
	String cmdLine = String(commentChar);
	cmdLine.concat(commanderName);
	cmdLine.concat(F(" User Commands:"));
	println(cmdLine);
  for(n = 0; n < commandListEntries; n++) if(commandList[n].manualString[0] != CMD_HIDE_HELP) {
		write(commentChar);
		println(getCommandItem(n));
	}
	write(commentChar);
	print(F(" Reload character: "));
	println(String(reloadCommandChar));
	write(commentChar);
	print(F(" Comment character: "));
	println(String(commentChar));
	write(commentChar);
	print(F(" Delimiters: "));
	println(delimiters);
	if(!ports.settings.bit.internalCommandsEnabled || !ports.settings.bit.printInternalCommands) return;
	write(commentChar);
  println(F(" Internal Commands:"));
	for(n = 0; n < INTERNAL_COMMAND_ITEMS; n++){
		write(commentChar);
		println(getInternalCommandItem(n));
	}

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
	
	write(commentChar);
	print(F("\tCommander version "));
	print(majorVersion);
	print(".");
	print(minorVersion);
	print(".");
	println(subVersion);
	
	write(commentChar);
	print(F("\tEcho terminal: "));
	ports.settings.bit.echoTerminal ? println("On") : println("Off");
	
	write(commentChar);
	print(F("\tEcho to Alt: "));
	ports.settings.bit.echoToAlt ? println("On") : println("Off");
	
	write(commentChar);
	print(F("\tAuto Format: "));
	ports.settings.bit.autoFormat ? println("On") : println("Off");
	
	write(commentChar);
	print(F("\tError messages: "));
	ports.settings.bit.errorMessagesEnabled ? println("On") : println("Off");
	
	write(commentChar);
	print(F("\tIn Port: "));
	ports.inPort ? println("OK") : println("NULL");
	
	write(commentChar);
	print(F("\tOut Port: "));
	ports.outPort ? println("OK") : println("NULL");
	
	write(commentChar);
	print(F("\tAlt Port: "));
	ports.altPort ? println("OK") : println("NULL");
	
	write(commentChar);
	print(F("\tLocked: "));
	ports.settings.bit.locked ? println("Yes") : println("No");
	
	write(commentChar);
	print(F("\tLock: "));
	ports.settings.bit.useHardLock ? println("Hard") : println("Soft");
	
	if(customHandler != NULL){
		
		write(commentChar);
		println( F("\tCustom Cmd OK"));
	}
}
//==============================================================================================================


uint8_t Commander::getInternalCmdLength(const char intCmd[]){
	for(int n = 0; n < 8; n++){
		if(intCmd[n] == NULL) return n;
	}
	return 8;
}
	