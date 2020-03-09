//Commander

#ifndef Commander_h
#define Commander_h

#include <Arduino.h>
#include <string.h>
#include "utilities\CommandHelpTags.h"

class Commander;

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
	char* commandString;
  cmdHandler handler;
	char* manualString;
} commandList_t;

extern const commandList_t myCommands[];
	
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
		uint32_t ignoreQuotes:1;						//20 don't treat items in quotes as special
  } bit;        // used for bit  access  
  uint32_t reg;  //used for register access 
} cmdSettings_t; 
//Settings register:
//							31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
//default is 	0b 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  1  1  1  0  1  1  0  0  0
//const String CommanderVersionNumber = "3.0.0";
const uint8_t majorVersion = 3;
const uint8_t minorVersion = 1;
const uint8_t subVersion   = 0;

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
class Commander{
public:
	Commander();
	Commander(uint16_t reservedBuffer);
	void   begin(Stream *sPort);
	void	 begin(Stream *sPort, const commandList_t *commands, uint32_t size);
	void	 begin(Stream *sPort, Stream *oPort, const commandList_t *commands, uint32_t size);
	bool   update();
	void	 setPassPhrase(String& phrase) {passPhrase = &phrase;}
	void   printPassPhrase() {print(*passPhrase);}
	void 	 lock() {ports.settings.bit.locked = true;}
	void 	 unlock() {ports.settings.bit.locked = false;}
	void 	 setLockType(bool hlState) {ports.settings.bit.useHardLock = hlState;}
	bool	 isLocked() {return ports.settings.bit.locked;}
	bool	 getLockType() {return ports.settings.bit.useHardLock;}
	bool   feed(Commander& Cmdr);
	bool   hasPayload();
	String getPayload();
	String getPayloadString();
	bool   feedString(String newString);
	void   loadString(String newString);
	bool 	 endLine();
	void 	 startStreaming() {commandState.bit.dataStreamOn = true;} //set the streaming function ON
	void 	 stopStreaming() {commandState.bit.dataStreamOn = false;} //set the streaming function OFF
	void 	 setStreaming(bool streamState) {commandState.bit.dataStreamOn = streamState;}
	bool 	 isStreaming() {return commandState.bit.dataStreamOn;}
	void 	 setStreamingMode(bool dataStreamMode) {ports.settings.bit.dataStreamMode = dataStreamMode;}
	bool 	 getStreamingMode() {return ports.settings.bit.dataStreamMode;}
	void   transfer(Commander& Cmdr);
	bool   transferTo(const commandList_t *commands, uint32_t size, String newName);
	void   transferBack(const commandList_t *commands, uint32_t size, String newName);
	void   attachOutputPort(Stream *oPort)					{ports.outPort = oPort;}
	Stream* getOutputPort() 												{return ports.outPort;}
	
	void   attachAltPort(Stream *aPort)						  {ports.altPort = aPort;} 
	Stream* getAltPort() 														{return ports.altPort;}
	//void detachAltPort()														{ports.altPort = NULL;}//ports.altPort->flush();
	
	void   attachInputPort(Stream *iPort)						{ports.inPort = iPort;}
	Stream* getInputPort()  												{return ports.inPort;}
	void 	 deleteAltPort() {ports.altPort = NULL;}
	void   attachSpecialHandler(cmdHandler handler) {customHandler = handler;}
	void 	 attachDefaultHandler(cmdHandler handler) {defaultHandler = handler;}
	void   setBuffer(uint16_t buffSize);
	void   attachCommands(const commandList_t *commands, uint32_t size);
	void   setStreamType(streamType_t newType) {ports.settings.bit.streamType = (uint16_t)newType;}
	streamType_t getStreamType() {return (streamType_t)ports.settings.bit.streamType;}
	void 	 quickSetHelp();
	bool   quickSet(String cmd, int& var);
	bool   quickSet(String cmd, float& var);
	bool   quickSet(String cmd, double& var);
	bool 	 quickSet(String cmd, String& str);
	void   quickGet(String cmd, int var);
	void   quickGet(String cmd, float var);
	void   quickGet(String cmd, double var);
	void 	 quickGet(String cmd, String str);
	
	size_t println() {
		yield();
		if( ports.settings.bit.copyResponseToAlt ) printAltln();
		yield();
		if(ports.outPort){ 
			return ports.outPort->println(); 
		}
		return 0;
	}
	//Template functions for print, println and write
	template <class printType>
	size_t print(printType printableVariable){
		yield();
		if( ports.settings.bit.copyResponseToAlt ) printAlt(printableVariable);
		yield();
		if(ports.outPort){
			doPrefix();
			return ports.outPort->print(printableVariable); 
		}
		return 0;
	}
	
	template <class printType>
	size_t print(printType printableVariable, int fmt){
		yield();
		if( ports.settings.bit.copyResponseToAlt ) printAlt(printableVariable, fmt);
		yield();
		if(ports.outPort){ 
			doPrefix();
			return ports.outPort->print(printableVariable, fmt); 
		}
		return 0;
	}
	
	template <class printType>
	size_t println(printType printableVariable){
		yield();
		if( ports.settings.bit.copyResponseToAlt ) printAltln(printableVariable);
		yield();
		if(ports.outPort){
				doPrefixln();
				if(commandState.bit.postfixMessage){
					ports.outPort->print(printableVariable); 
					return ports.outPort->println(postfixString);
				}
			return ports.outPort->println(printableVariable); 
		}
		return 0;
	}
	
	template <class printType>
	size_t println(printType printableVariable, int fmt){
		yield();
		if( ports.settings.bit.copyResponseToAlt ) printAltln(printableVariable, fmt);
		yield();
		if(ports.outPort){ 
				doPrefixln();
				if(commandState.bit.postfixMessage){
					ports.outPort->print(printableVariable, fmt); 
					return ports.outPort->println(postfixString);
				}
			return ports.outPort->println(printableVariable, fmt); 
		}
		return 0;
	}
	
	template <class printType>
	size_t write(printType printableVariable)	{ 
		yield();
		if( ports.settings.bit.copyResponseToAlt ) writeAlt(printableVariable);
		yield();
		if(ports.outPort){ 
				doPrefix();
				if(isNewline(printableVariable)) ports.outPort->print(postfixString);
			return ports.outPort->write(printableVariable); 
		}
		return 0;
	}
	
	template <class printType>
	size_t write(printType printableVariable, int length)	{ 
		yield();
		if( ports.settings.bit.copyResponseToAlt ) writeAlt(printableVariable, length);
		yield();
		if(ports.outPort){ 
				doPrefix();
				if(isNewline(printableVariable)) ports.outPort->print(postfixString);
			return ports.outPort->write(printableVariable, length); 
		}
		return 0;
	}
	
	
	void setPrefix(String prfx){
		prefixString = prfx;
		commandState.bit.prefixMessage = true;
		commandState.bit.newlinePrinted = true;
	}
	//enable prefix for this command with whatever String is already set
	void startPrefix(){
		commandState.bit.prefixMessage = true;
		commandState.bit.newlinePrinted = true;
	}
	//set postfix String for the command
	void setPostfix(String pofx){
		postfixString = pofx;
		commandState.bit.postfixMessage = true;
		//commandState.bit.newlinePrinted = true;
	}
	//enable Postfix for this command with whatever String is already set
	void startPostfix(){
		commandState.bit.postfixMessage = true;
		//commandState.bit.newlinePrinted = true;
	}
	void startFormatting(){
		commandState.bit.prefixMessage = true;
		commandState.bit.newlinePrinted = true;
		commandState.bit.postfixMessage = true;
	}
	void autoFormat(bool state){ ports.settings.bit.autoFormat = state;}
	bool autoFormat()					{ return ports.settings.bit.autoFormat;}

	void printCommandPrompt();
	
	bool containsTrue();
	bool containsFalse();
	bool containsOn();
	bool containsOff();
	
	void   setDelimiters(String myDelims);
	String getDelimiters();
	void 	 addDelimiter(char newDelim) {delimiters += newDelim;}
	
	void setCommentChar(char cmtChar)     {commentChar    = cmtChar;}
	void setReloadChar(char reloadChar)   {reloadCommandChar = reloadChar;}
	void setEndOfLineChar(char eol) 			{endOfLineChar         = eol;}
	void setPromptChar(char eol) 			    {promptChar         = eol;}
	//void setDelimChar(char eol) 			    {delimChar         = eol;}
	
	void echo(bool sState) 								{ports.settings.bit.echoTerminal = sState;}
	void printComments(bool cState)				{ports.settings.bit.printComments = cState;}
	void echoToAlt(bool sState) 					{if(ports.altPort) ports.settings.bit.echoToAlt = sState;} //only allow this if the altPort exists
	void copyRepyAlt(bool sState)  				{if(ports.altPort) ports.settings.bit.copyResponseToAlt = sState; } //only allow this if the altPort exists
	
	void commandProcessor(bool state)  		{ports.settings.bit.commandParserEnabled = state;}
	bool commandProcessor() 							{return ports.settings.bit.commandParserEnabled;}
	
	void stripCR(bool sState) 						{ports.settings.bit.stripCR = sState;}
	bool stripCR() 												{return ports.settings.bit.stripCR;}
	
	void multiCommander(bool enable) 			{ports.settings.bit.multiCommanderMode = enable;}
	bool multiCommander() 								{return ports.settings.bit.multiCommanderMode;}
	
	void errorMessages(bool state)				{ports.settings.bit.errorMessagesEnabled = state;}
	bool errorMessages() 									{return ports.settings.bit.errorMessagesEnabled;}
	
	void commandPrompt(bool state)				{ports.settings.bit.commandPromptEnabled = state;}
	bool commandPrompt() 									{return ports.settings.bit.commandPromptEnabled;}
	
	void showHelp(bool state)				    	{ports.settings.bit.helpEnabled = state;}
	bool showHelp() 											{return ports.settings.bit.helpEnabled;}

	void internalCommands(bool state)			{ports.settings.bit.internalCommandsEnabled = state;}
	bool internalCommands() 							{return ports.settings.bit.internalCommandsEnabled;}
	
	void showInternalCommands(bool state) {ports.settings.bit.printInternalCommands = state;}
	bool showInternalCommands() 					{return ports.settings.bit.printInternalCommands;}
	
	void autoChain(bool state)						{ports.settings.bit.autoChain = state;}
	bool autoChain()											{return ports.settings.bit.autoChain;}
	
	void autoChainErrors(bool state)			{ports.settings.bit.autoChainSurpressErrors = state;}
	bool autoChainErrors()								{return ports.settings.bit.autoChainSurpressErrors;}
	
	
	void chain()								 					{commandState.bit.chain = true;}
	void unchain();
	
	cmdSettings_t  getSettings() 											{return ports.settings;}
	void  				 setSettings(cmdSettings_t newSet)  {ports.settings = newSet;}
	
	portSettings_t getPortSettings() 											  {return ports;}
	void           setPortSettings(portSettings_t newPorts) {ports = newPorts;}
	
	void 		setPrintDelay(uint8_t dTime) 	{primntDelayTime = dTime;}
	uint8_t getPrintDelay() 							{return primntDelayTime;}
	
	void printDelay(bool enable) 			{ports.settings.bit.useDelay = enable;}
	bool printDelay() 								{return ports.settings.bit.useDelay;}
	
	void printDiagnostics();

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
	String 	getCommandItem(uint16_t commandItem); //returns a String containing the specified command and help text
	uint8_t getInternalCommandLength() {return INTERNAL_COMMAND_ITEMS;}
	String getInternalCommandItem(uint8_t internalItem);
	uint16_t getReadIndex() {return dataReadIndex;}
	void rewind();
	void printCommandList();
	void printCommanderVersion();
	
	String bufferString = ""; //the buffer - public so user functions can read it
	String commanderName = "CMD";
	
	#if defined BENCHMARKING_ON
		unsigned long benchmarkStartTime1 = 0, benchmarkStartTime2 = 0, benchmarkStartTime3 = 0, benchmarkStartTime4 = 0;
		unsigned long benchmarkTime1 = 0, benchmarkTime2 = 0, benchmarkTime3 = 0, benchmarkTime4 = 0;
		int benchmarkCounter = 0;
	#endif
	
private:
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
		for(int n = 0; n < bufferString.length(); n++){
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
	uint8_t longestCommand = 0;
	char commentChar = '#'; //marks a line as a comment - ignored by the command parser
	char reloadCommandChar = '/'; //send this character to automatically reprocess the old buffer - same as resending the last command from the users POV.	
	char promptChar = '>';
	//char* delimiters;
	String delimiters = "= :,\t\\/|";
	//char delimChar = '='; //special delimiter character - Is used IN ADDITION to the default space char to mark the end of a command or seperation between items
	char endOfLineChar = '\n';
  uint16_t bytesWritten = 0; //overflow check for bytes written into the buffer
	uint16_t bufferSize = SBUFFER_DEFAULT;
	uint16_t dataReadIndex = 0; //for parsing many numbers
	const char* internalCommandArray[INTERNAL_COMMAND_ITEMS];
	String *passPhrase = NULL;
	
	uint8_t primntDelayTime = 0; //
};
	
#endif //Commander_h