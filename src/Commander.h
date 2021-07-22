//Commander

#ifndef Commander_h
#define Commander_h

#include <Arduino.h>
#include <string.h>
#include "utilities/CommandHelpTags.h"

class Commander;

const uint8_t majorVersion = 4;
const uint8_t minorVersion = 3;
const uint8_t subVersion   = 0;


//#define BENCHMARKING_ON

#define DEBUG_INDEXER false

#ifndef ON
	#define ON true
#endif
#ifndef OFF
	#define OFF false
#endif
#ifndef ENABLED
	#define ENABLED true
#endif
#ifndef DISABLED
	#define DISABLED false
#endif
typedef bool (*cmdHandler)(Commander& Cmdr); //command handler function pointer type
//Command handler array type - contains command string and function pointer

typedef struct commandList_t{
	const char* commandString;
  cmdHandler handler;
	const char* manualString;
} commandList_t;

//extern const commandList_t myCommands[];
	
typedef union {
  struct {
		uint16_t newLine:1; 					//end of line was detected
    uint16_t bufferFull:1;  			//buffer is full
    uint16_t isCommandPending:1; 	//another command is in the pending command string
		uint16_t bufferState:1; 			//the state of the command buffer
		uint16_t commandHandled:1; 		//flag set if a command was handled during the last update
		uint16_t quickHelp:1; 				//flag set to true if quick help requested
		uint16_t quickSetCalled:1; 		//Flags that the current command is a quickset - chaining must break here
		uint16_t prefixMessage:1; 		//flag to indicate that any replies should be prefixed with the prefix string
		uint16_t postfixMessage:1; 		//flag to indicate that any replies should be appended with the postfix string
		uint16_t newlinePrinted:1; 		//flag to indicate that a newline has been sent - used with the prefixMessage flag to ensure prefix string appears at the start of every line
		uint16_t dataStreamOn:1; 			//indicates that a data stream is active and in one of two modes
		uint16_t chain:1; 						//Chain commands - reload the buffer after a command to see if there are more commands
		uint16_t chaining:1; 					//Flags that the current command is an attempt to chain and so errors should be surpressed.
		uint16_t commandType:3;				//Indicates which command type was last identified
  } bit;        // used for bit  access  
  uint16_t reg;  //used for register access 
} cmdState_t;
#define STREAM_MODE_EOF false
#define STREAM_MODE_PURE true

#define BUFFER_WAITING_FOR_START 0
#define BUFFER_BUFFERING_PACKET 1
//#define BUFFER_PACKET_RECEIVED 2


typedef union {
  struct {
    uint32_t echoTerminal:1; 						//0
    uint32_t echoToAlt:1;  							//1
    uint32_t copyResponseToAlt:1; 			//2
		uint32_t commandParserEnabled:1;		//3
		uint32_t errorMessagesEnabled:1;		//4
		uint32_t commandPromptEnabled:1;		//5
		uint32_t helpEnabled:1; 						//6 enable the help system
		uint32_t internalCommandsEnabled:1;	//7 disables the internal commands - stops them working
		uint32_t printInternalCommands:1;		//8 enable printing of internal commands with help
		uint32_t multiCommanderMode:1; 			//9 set to true when using multiple commander objects for multilayerd commands. Prevents multiple command prompts from appearing
		uint32_t printComments:1; 					//10 set to true and lines prefixed with the comment char will print to the out and alt ports
		uint32_t dataStreamMode:1;					//11 indicates the stream mode. mode 0 looks for an end of line, 1 is pure stream mode and cannot be terminated using a command char.
		uint32_t useHardLock:1; 						//12 use hard or soft lock (0 or 1)
		uint32_t locked:1; 									//13 Indicates if Commander is in a locked or unlocked state
		uint32_t stripCR:1; 								//14 Strip carriage returns from the buffer
		uint32_t streamType:2;							//15-16 A two bit code indicating the stream type (Serial, File, HTML, OTHER)
		uint32_t autoFormat:1; 							//17 bflag to indicate that all replies should be formatted with pre and postfix text
		uint32_t useDelay:1; 								//18 Insert a delay before println
		uint32_t autoChain:1; 							//19 Automatically chain commands, and to hell with the consequences
		uint32_t autoChainSurpressErrors:1;	//20 Prevent error messages when chaining commands
		uint32_t ignoreQuotes:1;						//21 don't treat items in quotes as special
  } bit;        // used for bit  access  
  uint32_t reg;  //used for register access 
} cmdSettings_t; 
//Settings register:
//							31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
//default is 	0b 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  1  1  1  0  1  1  0  0  0


typedef enum streamType_t{
	UNDEFINED_STREAM 	= 0,
	SERIAL_STREAM 		= 1,
	FILE_STREAM 			= 2,
	WEB_STREAM 				= 3,
} streamType_t;

typedef struct portSettings_t{
	Stream *inPort = NULL;
	Stream *outPort = NULL;
	
	Stream *altPort = NULL;
	
	cmdSettings_t settings;
} portSettings_t;

/*
#define INTERNAL_U												-20
#define INTERNAL_X												-19
#define INTERNAL_?												-18
#define INTERNAL_help											-17
#define INTERNAL_echo											-16
#define INTERNAL_echox										-15
#define INTERNAL_errors										-14
*/
#define UNKNOWN_COMMAND 										0
#define USER_COMMAND											  1
#define CUSTOM_COMMAND 										  2
#define INTERNAL_COMMAND 										3
#define COMMENT_COMMAND 										4


#define INTERNAL_COMMAND_ITEMS 							7

#define COMMANDER_DEFAULT_REGISTER_SETTINGS 0b00000000000000000100010111011000
//Default settings:
//commandParserEnabled = true
//errorMessagesEnabled = true
//stripCR = true
#define COMMANDER_DEFAULT_STATE_SETTINGS 		0x0
#define DEFAULT_DELIMITER_ITEMS 6
// =:\/|


	#define printAlt 		ports.altPort->print
	#define printAltln 	ports.altPort->println
	#define writeAlt 	  ports.altPort->write

#define HARD_LOCK true
#define SOFT_LOCK false
const uint16_t SBUFFER_DEFAULT = 128;
	
//some const strings for common messages
const String onString = "on";
const String offString = "off";
const String okString = "OK";
const String nullString = "NULL";
const String lockMessage = "Locked";
const String unlockMessage = "Unlocked";

const char EOFChar = 4; //end of file character for terminal

//Commander Class ===========================================================================================
class Commander : public Stream {
public:
	Commander();
	Commander(uint16_t reservedBuffer);
	Commander&   begin(Stream *sPort);
	Commander&	 begin(Stream *sPort, const commandList_t *commands, uint32_t size);
	Commander&	 begin(Stream *sPort, Stream *oPort, const commandList_t *commands, uint32_t size);
	Commander&	 begin(const commandList_t *commands, uint32_t size);
	bool   update();
	Commander&	 setPassPhrase(String& phrase) 	{passPhrase = &phrase; return *this;}
	Commander&   printPassPhrase() 							{print(*passPhrase); return *this;}
	Commander&	 setUserString(String& str) 		{userString = &str; return *this;}
	Commander&   printUserString() 							{print(*userString); return *this;}
	Commander&	 setExtraHelp(const char* ptr[]){extraHelp = ptr; return *this;}
	Commander& 	 lock() 												{ports.settings.bit.locked = true; return *this;}
	Commander& 	 unlock() 											{ports.settings.bit.locked = false; return *this;}
	Commander& 	 setLockType(bool hlState) 			{ports.settings.bit.useHardLock = hlState; return *this;}
	bool	 				isLocked() 										{return ports.settings.bit.locked;}
	bool	 				getLockType() 								{return ports.settings.bit.useHardLock;}
	bool   				feed(Commander& Cmdr);
	bool   				hasPayload();
	String 				getPayload();
	String 				getPayloadString();
	bool   				feedString(String newString);
	Commander&   	loadString(String newString);
	Commander&   	setPending(bool pState)									{commandState.bit.isCommandPending = pState; return *this;} //sets the pending command bit - used if manually writing to the buffer
	Commander&	 	add(uint8_t character) 								{bufferString += character; return *this;}
	bool 	 				endLine();
	Commander& 	 	startStreaming() 												{commandState.bit.dataStreamOn = true; return *this;} //set the streaming function ON
	Commander& 	 	stopStreaming() 												{commandState.bit.dataStreamOn = false; return *this;} //set the streaming function OFF
	Commander& 	 	setStreaming(bool streamState) 					{commandState.bit.dataStreamOn = streamState; return *this;}
	bool 	 				isStreaming() 													{return commandState.bit.dataStreamOn;}
	Commander& 	 	setStreamingMode(bool dataStreamMode) 	{ports.settings.bit.dataStreamMode = dataStreamMode; return *this;}
	bool 	 				getStreamingMode() 													{return ports.settings.bit.dataStreamMode;}
	Commander&   	transfer(Commander& Cmdr);
	bool   				transferTo(const commandList_t *commands, uint32_t size, String newName);
	Commander&   	transferBack(const commandList_t *commands, uint32_t size, String newName);
	Commander&   	attachOutputPort(Stream *oPort)							{ports.outPort = oPort; return *this;}
	Stream* 			getOutputPort() 														{return ports.outPort;}
	Commander&   	attachAltPort(Stream *aPort)								{ports.altPort = aPort; return *this;} 
	Stream* 			getAltPort() 																{return ports.altPort;}
	//void detachAltPort()														{ports.altPort = NULL;}//ports.altPort->flush();
	
	Commander&   	attachInputPort(Stream *iPort)						{ports.inPort = iPort; return *this;}
	Stream* 			getInputPort()  													{return ports.inPort;}
	Commander& 	 	deleteAltPort() 													{ports.altPort = NULL; return *this;}
	Commander&   	attachSpecialHandler(cmdHandler handler) 	{customHandler = handler; return *this;}
	Commander& 	 	attachDefaultHandler(cmdHandler handler) 	{defaultHandler = handler; return *this;}
	Commander&   	setBuffer(uint16_t buffSize);
	Commander&  	attachCommands(const commandList_t *commands, uint32_t size);
	Commander&  	attachCommandArray(const commandList_t *commands, uint32_t length);
	Commander&   	setStreamType(streamType_t newType) 			{ports.settings.bit.streamType = (uint16_t)newType; return *this;}
	streamType_t 	getStreamType() 													{return (streamType_t)ports.settings.bit.streamType;}
	
	Commander&    reloadCommands() 											  	{computeLengths(); return *this;}
	
	int 	 				quick(String cmd);
	Commander& 	 	quickSetHelp();
	bool   				quickSet(String cmd, int& var);
	bool   				quickSet(String cmd, float& var);
	bool   				quickSet(String cmd, double& var);
	bool 	 				quickSet(String cmd, String& str);
	Commander&   	quickGet(String cmd, int var);
	Commander&   	quickGet(String cmd, float var);
	Commander&  	quickGet(String cmd, double var);
	Commander& 	 	quickGet(String cmd, String str);
		
	size_t write(uint8_t b) {
		yield();
		if( ports.settings.bit.copyResponseToAlt ) writeAlt(b);
		yield();
		if(ports.outPort){ 
				doPrefix();
				if(isNewline(b)) ports.outPort->print(postfixString);
			return ports.outPort->write(b); 
		}
		return 0;
	}

	int available() { return bufferString.length(); }

	int read() {
		if(!bufferString.length())
			return -1;
		char retchar = bufferString.charAt(0);
		bufferString.remove(0, 1);
		return retchar;
	}

	int peek() {
		if(!bufferString.length())
			return -1;
		return bufferString.charAt(0);
	}
	
	int availableForWrite() {
#if !defined(ESP8266) && !defined(ESP32)
		return ports.outPort ? ports.outPort->availableForWrite() : 0;
#else
		// availableForWrite() is not part of the Stream class on ESP32 (and probably ESP8266)
		return 0;
#endif
	}
	
	

	//int availableForWrite() { return ports.outPort ? ports.outPort->availableForWrite() : 0; }

	void flush() { if(ports.outPort) ports.outPort->flush(); }

	using Print::write; // pull in write(String) and write(buf, size) from Print

	
	
	Commander& setPrefix(String prfx){
		prefixString = prfx;
		commandState.bit.prefixMessage = true;
		commandState.bit.newlinePrinted = true;
		return *this;
	}
	//enable prefix for this command with whatever String is already set
	Commander& startPrefix(){
		commandState.bit.prefixMessage = true;
		commandState.bit.newlinePrinted = true;
		return *this;
	}
	//set postfix String for the command
	Commander& setPostfix(String pofx){
		postfixString = pofx;
		commandState.bit.postfixMessage = true;
		return *this;
		//commandState.bit.newlinePrinted = true;
	}
	//enable Postfix for this command with whatever String is already set
	Commander& startPostfix(){
		commandState.bit.postfixMessage = true;
		return *this;
		//commandState.bit.newlinePrinted = true;
	}
	Commander& startFormatting(){
		commandState.bit.prefixMessage = true;
		commandState.bit.newlinePrinted = true;
		commandState.bit.postfixMessage = true;
		return *this;
	}
	Commander&  autoFormat(bool state){ ports.settings.bit.autoFormat = state; return *this;}
	bool autoFormat()									{ return ports.settings.bit.autoFormat;}

	Commander& printCommandPrompt();
	
	bool containsTrue();
	bool containsFalse();
	bool containsOn();
	bool containsOff();
	
	Commander&  delimiters(String myDelims)				{ if(myDelims != "")	delimiterChars = myDelims; return *this;}
	String 			delimiters()											{return delimiterChars;}
	Commander& 	addDelimiter(char newDelim) 			{delimiterChars += newDelim; return *this;}
	
	Commander&  commentChar(char cmtChar)     		{commentCharacter    = cmtChar; return *this;}
	char 				commentChar()     								{return commentCharacter;}
	Commander&  reloadChar(char reloadChar)   		{reloadCommandCharacter = reloadChar; return *this;}
	char 				reloadChar()     									{return reloadCommandCharacter;}
	Commander&  endOfLineChar(char eol);
	char 				endOfLineChar()     							{return endOfLineCharacter;}
	Commander&  promptChar(char eol) 			    		{promptCharacter         = eol; return *this;}
	char  			promptChar() 			    						{return promptCharacter;}
	//void setDelimChar(char eol) 			    {delimChar         = eol;}
	
	Commander& echo(bool sState) 								{ports.settings.bit.echoTerminal = sState; return *this;}
	bool echo() 																{return ports.settings.bit.echoTerminal;}
	Commander& printComments(bool cState)				{ports.settings.bit.printComments = cState; return *this;}
	bool printComments()												{return ports.settings.bit.printComments;}
	Commander& echoToAlt(bool sState) 					{if(ports.altPort) ports.settings.bit.echoToAlt = sState; return *this;} //only allow this if the altPort exists
	bool echoToAlt() 					      						{return ports.settings.bit.echoToAlt;} //only allow this if the altPort exists
	Commander& copyRepyAlt(bool sState)  				{if(ports.altPort) ports.settings.bit.copyResponseToAlt = sState;  return *this;} //only allow this if the altPort exists
	bool copyRepyAlt()  				      					{return ports.settings.bit.copyResponseToAlt;} //only allow this if the altPort exists
	
	Commander& commandProcessor(bool state)  		{ports.settings.bit.commandParserEnabled = state; return *this;}
	bool commandProcessor() 										{return ports.settings.bit.commandParserEnabled;}
	
	Commander& stripCR(bool sState) 						{ports.settings.bit.stripCR = sState; return *this;}
	bool stripCR() 															{return ports.settings.bit.stripCR;}
	
	Commander& multiCommander(bool enable) 			{ports.settings.bit.multiCommanderMode = enable; return *this;}
	bool multiCommander() 											{return ports.settings.bit.multiCommanderMode;}
	
	Commander& errorMessages(bool state)				{ports.settings.bit.errorMessagesEnabled = state; return *this;}
	bool errorMessages() 												{return ports.settings.bit.errorMessagesEnabled;}
	
	Commander& commandPrompt(bool state)				{ports.settings.bit.commandPromptEnabled = state; return *this;}
	bool commandPrompt() 												{return ports.settings.bit.commandPromptEnabled;}
	
	Commander& showHelp(bool state)				    	{ports.settings.bit.helpEnabled = state; return *this;}
	bool showHelp() 														{return ports.settings.bit.helpEnabled;}

	Commander& internalCommands(bool state)			{ports.settings.bit.internalCommandsEnabled = state; return *this;}
	bool internalCommands() 										{return ports.settings.bit.internalCommandsEnabled;}
	
	Commander& showInternalCommands(bool state) {ports.settings.bit.printInternalCommands = state; return *this;}
	bool showInternalCommands() 								{return ports.settings.bit.printInternalCommands;}
	
	Commander& autoChain(bool state)						{ports.settings.bit.autoChain = state; return *this;}
	bool autoChain()														{return ports.settings.bit.autoChain;}
	
	Commander& autoChainErrors(bool state)			{ports.settings.bit.autoChainSurpressErrors = state; return *this;}
	bool autoChainErrors()											{return ports.settings.bit.autoChainSurpressErrors;}
	
	
	Commander& chain()								 					{commandState.bit.chain = true; return *this;}
	Commander& unchain();
	
	cmdSettings_t  	settings() 											{return ports.settings;}
	Commander&  	 	settings(cmdSettings_t newSet)  {ports.settings = newSet; return *this;}
	
	portSettings_t 	portSettings() 											  {return ports;}
	Commander&     	portSettings(portSettings_t newPorts) {ports = newPorts; return *this;}
	
	Commander& 			printDelayTime(uint8_t dTime) 	{primntDelayTime = dTime; return *this;}
	uint8_t 				printDelayTime() 								{return primntDelayTime;}
	
	Commander& printDelay(bool enable) 							{ports.settings.bit.useDelay = enable; return *this;}
	bool printDelay() 															{return ports.settings.bit.useDelay;}
	
	Commander& printDiagnostics();

	template <class iType>
	bool getInt(iType &myIvar)	{ 
		if(tryGet()){
			//Parse it to the variable
			String subStr = bufferString.substring(dataReadIndex);
			myIvar = (iType)subStr.toInt();
			//if there is no space next, set dataReadIndex to zero and return true - you parsed an int, but next time it will fail.
			if(!findNextItem()) dataReadIndex = 0;//nextNumberDelimiter();
			return true; //nextSpace();
		}else return 0;
	}
	bool getFloat(float &myFloat);  //Returns true if myFloat was updated with a value parsed from the command String
	bool getDouble(double &myDouble);  //Returns true if myDouble was updated with a value parsed from the command String
	bool getString(String &myString); //returns the next string in the payload - determined by the delimiters space and special char
	uint8_t countItems(); //Returns the number of items in the payload. An item is any string with a space or delimiterChar at each end (or the end of line)
	uint16_t getCommandListLength() {return commandListEntries;} //returns the number of commands
	const commandList_t* getCommandList() {return commandList;}
	String 	getCommandItem(uint16_t commandItem); //returns a String containing the specified command and help text
	uint8_t getInternalCommandLength() {return INTERNAL_COMMAND_ITEMS;}
	String getInternalCommandItem(uint8_t internalItem);
	uint16_t getReadIndex() {return dataReadIndex;}
	Commander& rewind();
	Commander& printCommandList();
	Commander& printCommanderVersion();
	int16_t getCommandIndex()										{return commandIndex;}
	String bufferString = ""; //the buffer - public so user functions can read it
	String commanderName = "CMD";
	
	#if defined BENCHMARKING_ON
		unsigned long benchmarkStartTime1 = 0, benchmarkStartTime2 = 0, benchmarkStartTime3 = 0, benchmarkStartTime4 = 0;
		unsigned long benchmarkTime1 = 0, benchmarkTime2 = 0, benchmarkTime3 = 0, benchmarkTime4 = 0;
		int benchmarkCounter = 0;
	#endif
	
private:
	bool processPending();
	bool streamData();
	void echoPorts(int portByte);
	void bridgePorts();
		void doPrefix(){ //handle prefixes for command replies
			if(commandState.bit.prefixMessage && commandState.bit.newlinePrinted) ports.outPort->print(prefixString); 
			commandState.bit.newlinePrinted = false;
		}
		void doPrefixln(){ //handle prefixes for command replies with newlines
			if( ports.settings.bit.useDelay ) delay(primntDelayTime);
			if(commandState.bit.prefixMessage && commandState.bit.newlinePrinted) ports.outPort->print(prefixString); 
			commandState.bit.newlinePrinted = true;
		}
	bool qSetHelp(String &cmd);
	int qSetSearch(String &cmd);
	void computeLengths();
	uint8_t getLength(uint8_t indx);
	bool handleCommand();
	bool handleUnknown();
	void tryUnlock();
  bool checkPass();
	void handleComment();
	bool processBuffer(int dataByte);
	void writeToBuffer(int dataByte);
	void resetBuffer();
	int  matchCommand();
	bool checkCommand(uint16_t cmdIdx);
	bool checkInternalCommand(uint16_t cmdIdx);
	bool qcheckInternal(uint8_t itm);
	int  handleInternalCommand(uint16_t internalCommandIndex);
	bool handleCustomCommand();
	bool tryGet();
	bool findNextDelim();
	bool findNextItem();
	bool delimToNextItem();
	bool itemToNextDelim();
	bool isDelimiter(char ch);
	bool isItem(char ch);
	bool isNumber(String &str);
	bool isNumeral(char ch);
	bool isEndOfLine(char dataByte);
	bool isEndOfCommand(char dataByte);
	//bool isStartOfItem(char dataByte);
	bool isNewline(char var) {		return (var == '\n') ? true : false;}
	bool isNewline(int var) {			return (var == '\n') ? true : false;}
	//bool isNewline(char var[]) {	return false;} //fix this so it looks for a newline at the end of the string ...
	String getWhiteSpace(uint8_t spaces);
	uint8_t getInternalCmdLength(const char intCmd[]);
	//utility to print the buffer contents as integer values
	void printBuffer(){
		for(uint32_t n = 0; n < bufferString.length(); n++){
			write('[');
			print((int)bufferString.charAt(n));
			write(']');
		}
	}
	String prefixString = "";
	String postfixString = "";
	const commandList_t* commandList;
	uint8_t commandListEntries = 0;
  cmdHandler customHandler;
  cmdHandler defaultHandler;
	cmdState_t commandState;
	portSettings_t ports;
  //int8_t commandType = UNKNOWN_COMMAND;
  int16_t commandIndex = -1;
	uint8_t* commandLengths;
	uint8_t endIndexOfLastCommand = 0;
	const char** extraHelp;
	uint8_t longestCommand = 0;
	char commentCharacter = '#'; //marks a line as a comment - ignored by the command parser
	char reloadCommandCharacter = '/'; //send this character to automatically reprocess the old buffer - same as resending the last command from the users POV.	
	char promptCharacter = '>';
	//char* delimiters;
	String delimiterChars = "= :,\t\\/|";
	//char delimChar = '='; //special delimiter character - Is used IN ADDITION to the default space char to mark the end of a command or seperation between items
	char endOfLineCharacter = '\n';
  uint16_t bytesWritten = 0; //overflow check for bytes written into the buffer
	uint16_t bufferSize = SBUFFER_DEFAULT;
	uint16_t dataReadIndex = 0; //for parsing many numbers
	//const char* internalCommandArray[INTERNAL_COMMAND_ITEMS];
	const char* internalCommandArray[INTERNAL_COMMAND_ITEMS] = { "U", "X", "?", "help", "echo", "echox", "errors"};
	String *passPhrase = NULL;
	String *userString = NULL;
	uint8_t primntDelayTime = 0; //
};
	
#endif //Commander_h