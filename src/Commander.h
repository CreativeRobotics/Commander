//Commander

#ifndef Commander_h
#define Commander_h

#include <Arduino.h>
#include <string.h>
class Commander;

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
		uint16_t newLine:1; //end of line was detected
    uint16_t bufferFull:1;  //buffer is full
    uint16_t isCommandPending:1; //another command is in the pending command string
		uint16_t bufferState:2; //the state of the command buffer
		uint16_t commandHandled:1; //flag set if a command was handled during the last update
		uint16_t commanderStarted:1; //flag set if a commander update function has run once
		uint16_t quickHelp:1; //flag set to true if quick help requested
		uint16_t prefixMessage:1; //flag to indicate that any replies should be prefixed with the prefix string
		uint16_t postfixMessage:1; //flag to indicate that any replies should be appended with the postfix string
		uint16_t newlinePrinted:1; //flag to indicate that a newline has been sent - used with the prefixMessage flag to ensure prefix string appears at the start of every line
		uint16_t autoFormat:1; //flag to indicate that all replies should be formatted with pre and postfix text
		uint16_t streamOn:1; //indicates that a data stream is incoming - all printing characters get routed somewhere until the EOFChar is found
		//uint16_t altCommandListMatch:1; //flag to indicate that a match was found in the alt command list
  } bit;        // used for bit  access  
  uint16_t reg;  //used for register access 
} cmdState_t;
#define BUFFER_WAITING_FOR_START 0
#define BUFFER_BUFFERING_PACKET 1
#define BUFFER_PACKET_RECEIVED 2

typedef union {
  struct {
    uint16_t echoTerminal:1; 					//0
    uint16_t echoToAlt:1;  						//1
    uint16_t copyResponseToAlt:1; 		//2
		uint16_t commandParserEnabled:1;	//3
		uint16_t errorMessagesEnabled:1;	//4
		uint16_t commandPromptEnabled:1;	//5
		uint16_t helpEnabled:1; 					//6 enable the help system
		uint16_t multiCommanderMode:1; 		//7 set to true when using multiple commander objects for multilayerd commands. Prevents multiple command prompts from appearing
		uint16_t printComments:1; 				//8 set to true and lines prefixed with the comment char will print to the out and alt ports
		uint16_t locked:1; 								//9 locks or unlocks commander
		uint16_t useHardLock:1; 					//10 use hard or soft lock (0 or 1)
		uint16_t stripCR:1; 							//11 Strip carriage returns from the buffer
  } bit;        // used for bit  access  
  uint16_t reg;  //used for register access 
} cmdSettings_t;
//default is 0b00000100001011000
//const String CommanderVersionNumber = "1.0.1";
const uint8_t majorVersion = 1;
const uint8_t minorVersion = 3;
const uint8_t subVersion   = 1;

typedef struct portSettings_t{
	Stream *inPort = NULL;
	Stream *outPort = NULL;
	
	Stream *altPort = NULL;
	
	cmdSettings_t settings;
} portSettings_t;

#define UNKNOWN_COMMAND 										32767
#define CUSTOM_COMMAND 										  -2
#define INTERNAL_COMMAND 										-1
#define COMMENT_COMMAND 										-3
#define INTERNAL_COMMAND_ITEMS 							8

#define COMMANDER_DEFAULT_REGISTER_SETTINGS 0x858
//Default settings:
//commandParserEnabled = true
//errorMessagesEnabled = true
//stripCR = true
#define COMMANDER_DEFAULT_STATE_SETTINGS 		0x0



	#define printAlt 		ports.altPort->print
	#define printAltln 	ports.altPort->println
	#define writeAlt 	  ports.altPort->write


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
	bool   updatePending();
	bool 	 printCommands() {printCommandList();}
	void	 setPassPhrase(String& phrase) {passPhrase = &phrase;}
	void   printPassPhrase() {print(*passPhrase);}
	void 	 lock() {ports.settings.bit.locked = true;}
	void 	 unlock() {ports.settings.bit.locked = false;}
	void 	 setHardLock(bool hlState) {ports.settings.bit.useHardLock = hlState;}
	bool	 isLocked() {return ports.settings.bit.locked;}
	bool	 hardLockEnabled() {return ports.settings.bit.useHardLock;}
	bool   feed(Commander& Cmdr);
	bool   hasPayload();
	String getPayload();
	String getPayloadString();
	bool   feedString(String newString);
	void   loadString(String newString);
	bool 	 endLine();
	void 	 startStream() {commandState.bit.streamOn = true;} //set the streaming function ON
	void 	 stopStream() {commandState.bit.streamOn = false;} //set the streaming function OFF
	void 	 setStream(bool streamState) {commandState.bit.streamOn = streamState;}
	bool 	 isStreaming() {return commandState.bit.streamOn;}
	void   transfer(Commander& Cmdr);
	bool   transferTo(const commandList_t *commands, uint32_t size, String newName);
	void   transferBack(const commandList_t *commands, uint32_t size, String newName);
	void   attachOutputPort(Stream *oPort);
	Stream* getOutputPort() 												{return ports.outPort;}
	
	void   attachAltPort(Stream *aPort);
	Stream* getAltPort() 														{return ports.altPort;}
	//void detachAltPort()														{ports.altPort = NULL;}//ports.altPort->flush();
	
	void   attachInputPort(Stream *iPort);
	Stream* getInputPort()  												{return ports.inPort;}
	void 	 deleteAltPort() {ports.altPort = NULL;}
	void   attachSpecialHandler(cmdHandler handler) {customHandler = handler;}
	void   setBuffer(uint16_t buffSize);
	void   attachCommands(const commandList_t *commands, uint32_t size);
	
	void 	 quickSetHelp();
	bool   quickSet(String cmd, int& var);
	bool   quickSet(String cmd, float& var);
	bool   quickSet(String cmd, double& var);
	void   quickGet(String cmd, int var);
	void   quickGet(String cmd, float var);
	
	size_t println() {
		
		if( ports.settings.bit.copyResponseToAlt ) printAltln();
		
		if(ports.outPort){ 
			return ports.outPort->println(); 
		}
		yield();
	}
	//Template functions for print, println and write
	template <class printType>
	size_t print(printType printableVariable){
		
		if( ports.settings.bit.copyResponseToAlt ) printAlt(printableVariable);
		
		if(ports.outPort){ 
			//#if defined (CMD_ENABLE_FORMATTING)
				doPrefix();
			//#endif
			return ports.outPort->print(printableVariable); 
		}
		yield();
	}
	template <class printType>
	size_t print(printType printableVariable, int fmt){
		
		if( ports.settings.bit.copyResponseToAlt ) printAlt(printableVariable, fmt);
		
		if(ports.outPort){ 
			//#if defined (CMD_ENABLE_FORMATTING)
				doPrefix();
			//#endif
			return ports.outPort->print(printableVariable, fmt); 
		}
		yield();
	}
	template <class printType>
	size_t println(printType printableVariable){
		
		if( ports.settings.bit.copyResponseToAlt ) printAltln(printableVariable);
		
		if(ports.outPort){ 
			//#if defined (CMD_ENABLE_FORMATTING)
				doPrefixln();
				if(commandState.bit.postfixMessage){
					ports.outPort->print(printableVariable); 
					return ports.outPort->println(postfixString);
				}
			//#endif
			return ports.outPort->println(printableVariable); 
		}
		yield();
	}
	template <class printType>
	size_t println(printType printableVariable, int fmt){
		
		if( ports.settings.bit.copyResponseToAlt ) printAltln(printableVariable, fmt);
		
		if(ports.outPort){ 
			//#if defined (CMD_ENABLE_FORMATTING)
				doPrefixln();
				if(commandState.bit.postfixMessage){
					ports.outPort->print(printableVariable, fmt); 
					return ports.outPort->println(postfixString);
				}
			//#endif
			return ports.outPort->println(printableVariable, fmt); 
		}
		yield();
	}
	template <class printType>
	size_t write(printType printableVariable)	{ 
		
		if( ports.settings.bit.copyResponseToAlt ) writeAlt(printableVariable);
		
		if(ports.outPort){ 
			//#if defined (CMD_ENABLE_FORMATTING)
				doPrefix();
				if(isNewline(printableVariable)) ports.outPort->print(postfixString);
			//#endif
			//add a check to see if the printable variable is a newline and apply postfix
			return ports.outPort->write(printableVariable); 
		}
		yield();
		return 0;
	}
	template <class printType>
	size_t write(printType printableVariable, int length)	{ 
		
		if( ports.settings.bit.copyResponseToAlt ) writeAlt(printableVariable, length);
		
		if(ports.outPort){ 
			//#if defined (CMD_ENABLE_FORMATTING)
				doPrefix();
				if(isNewline(printableVariable)) ports.outPort->print(postfixString);
			//#endif
			return ports.outPort->write(printableVariable, length); 
		}
		yield();
		return 0;
	}
	
	bool isNewline(char var) {		return (var == '\n') ? true : false;}
	bool isNewline(int var) {			return (var == '\n') ? true : false;}
	bool isNewline(char var[]) {	return false;} //fix this so it looks for a newline at the end of the string ...
	//set the prefix string and enable it for this command
	
//#if defined (CMD_ENABLE_FORMATTING)
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
		void setAutoFormat(bool state){ commandState.bit.autoFormat = state;}
		bool getAutoFormat()					{ return commandState.bit.autoFormat;}
	//#endif
	void printCommandPrompt();
	
	bool containsTrue();
	bool containsOn();
	
	void setCommentChar(char cmtChar)     {commentChar    = cmtChar;}
	void setReloadChar(char reloadChar)   {reloadCommandChar = reloadChar;}
	void setEndOfLineChar(char eol) 			{endOfLine         = eol;}
	
	void echo(bool sState) 								{ports.settings.bit.echoTerminal = sState;}
	void printComments(bool cState)				{ports.settings.bit.printComments = cState;}
	void echoToAlt(bool sState) 					{ if(ports.altPort) ports.settings.bit.echoToAlt = sState;} //only allow this if the altPort exists
	void copyRepyAlt(bool sState)  				{ if(ports.altPort) ports.settings.bit.copyResponseToAlt = sState; } //only allow this if the altPort exists
	
	void enableCommands(bool sState)  		{ports.settings.bit.commandParserEnabled = true;}
	void disableCommands(bool sState)  		{ports.settings.bit.commandParserEnabled = false;}
	void stripCR(bool sState) 						{ports.settings.bit.stripCR = sState;}
	void multiCommander(bool enable) 			{ports.settings.bit.multiCommanderMode = enable;}
	
	void enableErrors()  									{ports.settings.bit.errorMessagesEnabled = true;}
	void disableErrors()  								{ports.settings.bit.errorMessagesEnabled = false;}
	
	void enablePrompt()  									{ports.settings.bit.commandPromptEnabled = true;}
	void disablePrompt()  								{ports.settings.bit.commandPromptEnabled = false;}
	
	void enableHelp()  									  {ports.settings.bit.helpEnabled = true;}
	void disableHelp()  								  {ports.settings.bit.helpEnabled = false;}
	
	cmdSettings_t  getSettings() 											{return ports.settings;}
	void  				 setSettings(cmdSettings_t newSet)  {ports.settings = newSet;}
	
	portSettings_t getPortSettings() 											  {return ports;}
	void           setPortSettings(portSettings_t newPorts) {ports = newPorts;}
	
	
	
	//Useful functions for extracting values
	bool  getInt(int &myInt); //Returns trie if myInt was updated with a value parsed from the command String
	bool 	getFloat(float &myFloat);  //Returns true if myFloat was updated with a value parsed from the command String
	bool  getDouble(double &myDouble);  //Returns true if myDouble was updated with a value parsed from the command String
	//Both these internal commands can be invoked externally. Their commands handlers can be overridden by the user, whose own handler can then call these if required
	void printCommandList();
	void printCommanderVersion();
	void unknownCommand();
	
	String bufferString = ""; //the buffer - public so user functions can read it
	String commanderName = "CMD";
	char promptCharacter = '>';
	char eocCharacter = '='; //special end of command character - Is used IN ADDITION to the default space char to mark the end of a command
	//#if defined (CMD_ENABLE_FORMATTING)
	String prefixString = "";
	String postfixString = "";
	
	//#endif
	
private:
	void echoPorts(int portByte);
	void bridgePorts();
	//#if defined (CMD_ENABLE_FORMATTING)
		void doPrefix(){ //handle prefixes for command replies
			if(commandState.bit.prefixMessage && commandState.bit.newlinePrinted) ports.outPort->print(prefixString); 
			commandState.bit.newlinePrinted = false;
		}
		void doPrefixln(){ //handle prefixes for command replies with newlines
			if(commandState.bit.prefixMessage && commandState.bit.newlinePrinted) ports.outPort->print(prefixString); 
			commandState.bit.newlinePrinted = true;
		}
	//#endif
	bool qSetHelp(String &cmd);
	int qSetSearch(String &cmd);
	void computeLengths();
	uint8_t getLength(uint8_t indx);
	bool handleCommand();
	void tryUnlock();
  bool checkPass();
	void handleComment();
	bool processBuffer(int dataByte);
	void writeToBuffer(int dataByte);
	void resetBuffer();
	int  matchCommand();
	bool checkCommand(uint16_t cmdIdx);
	//bool checkAltCommand(uint16_t cmdIdx);
	int  handleInternalCommand(uint16_t internalCommandIndex);
	bool  handleCustomCommand();
	bool tryGet();
	bool nextSpace();
	bool nextDelimiter();
	int findNumeral(uint8_t startIdx);
	bool isNumber(String str);
	bool isDelimiter(char ch);
	bool isNumeral(char ch);
	bool isCommandChar(char dataByte);
	bool isCommandStart(char dataByte);
	bool isEndOfLine(char dataByte);
	String getWhiteSpace(uint8_t spaces);
	
	const commandList_t* commandList;
	//const commandList_t* altCommandList;
	//Add a second command list pointer and allow both to be used concurrently
	//One can be for 'common' commands shared between commander objects, the other can be for port specific ones, and for overriding shared commands ...
	//Need one pointer for the list, two length variables plus two control bits to indicate if a second command set is in use, and if a command match was made there ...
	//Primary list will override the secondary one
	uint16_t commandListEntries = 0;
	//uint16_t altCommandListEntries = 0;
	
  cmdHandler customHandler;
	cmdState_t commandState;
	portSettings_t ports;
	
  int commandVal = UNKNOWN_COMMAND;
	
	uint8_t *commandLengths;
	uint8_t endIndexOfLastCommand = 0;
	uint8_t longestCommand = 0;
	char commentChar = '#'; //marks a line as a comment - ignored by the command parser
	char reloadCommandChar = '/'; //send this character to automatically reprocess the old buffer - same as resending the last command from the users POV.
  uint16_t bytesWritten = 0; //overflow check for bytes written into the buffer

  //String pendingCommandString = "";
	uint16_t bufferSize = SBUFFER_DEFAULT;
	uint16_t dataReadIndex = 0; //for parsing many numbers
  
  uint8_t endOfLine = '\n';

	//const uint16_t internalCommandItems = INTERNAL_COMMAND_ITEMS;
	const char* internalCommands[INTERNAL_COMMAND_ITEMS];
	
	
	String *passPhrase = NULL;
};
	
#endif //Commander_h