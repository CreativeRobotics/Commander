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

typedef enum bufferState_t{
  WAITING_FOR_START = 0,
  BUFFERING_PACKET,
  PACKET_RECEIVED
} bufferState_t;

const String CommanderVersionNumber = "Commander V0.1A";

#define UNKNOWN_COMMAND 32767
#define INTERNAL_COMMAND_ITEMS 6

//Commander Class ===========================================================================================
class Commander{
public:
	Commander();
	void begin(Stream *sPort);
	bool update();
	void setOutputPort(Stream *oPort);
	
	void setBuffer(uint16_t buffSize);
	void attachCommands(const commandList_t *commands, int size);
	void attachAltPort(Stream *aPort);
	//Need a methof of appending commands to outgoing messages, for example when sending to another command handler
	void print(String line)   		{ if(outPort) outPort->print(line);   if( copyResponseToAlt && altPort ) altPort->print(line); }
	void println(String line) 		{ if(outPort) outPort->println(line); if( copyResponseToAlt && altPort ) altPort->print(line); }
	void write(char data) 				{ if(outPort) outPort->write(data);   if( copyResponseToAlt && altPort ) altPort->write(data); }
	void altPrint(String line)   	{ if(altPort) altPort->print(line);   }
	void altPrintln(String line) 	{ if(altPort) altPort->println(line); }
	void altWrite(char data) 			{ if(altPort) altPort->write(data);   }
	
	bool containsTrue();
	bool containsOn();
	
	void setCommentString(String cmtString) {commentString = cmtString;}
	void setEndOfLineChar(char eol) 				{endOfLine = eol;}
	
	
	//Useful functions for extracting values
	int getInt(); //returns an int if there is one after the command
	float getFloat();  //returns a float if there is one after the command
	double getDouble();  //returns a double if there is one after the command
	
	
	
  bool echoTerminal = false;
	bool echoToAlt = false; //set true to copy incoming data to the alt inPort
	bool copyResponseToAlt = false; //set true to copy any responses or error messages to the alt port
	bool commandParserEnabled = true; //disable to prevent commands being handled - when using as a pass through
	bool errorMessagesEnabled = true; //enables replies such as "ERR: Unknown Command"
	
	String bufferString = ""; //the buffer - public so user functions can read it
private:
	void setup();
	void computeLengths();
	uint8_t getLength(uint8_t indx);
	bool handleCommand();
	void processBuffer(int dataByte);
	void writeToBuffer(int dataByte);
	void resetBuffer();
	int matchCommand();
	int handleInternalCommand(uint16_t internalCommandIndex);
	void unknownCommand();
	bool isCommandChar(char dataByte);
	bool isCommandStart(char dataByte);
	bool isEndOfLine(char dataByte);
	bool printCommandList();
	bool printCommanderVersion();
	
	const commandList_t* commandList;
	int commandListEntries = 0;
	
  Stream *inPort = NULL;
  Stream *outPort = NULL;
	Stream *altPort = NULL;//&Serial; //for pass through - initialised to something sensible
  int commandVal = UNKNOWN_COMMAND;
	
	uint8_t *commandLengths;
	uint8_t endIndexOfLastCommand = 0;
	uint8_t longestCommand = 0;
  
	String commentString = "#"; //marks a line as a comment - ignored by the command parser
	
  unsigned long bytesWritten = 0;
  bool newData = false;
  bool newLine = false;
  bool bufferFull = false;
	
	const uint16_t SBUFFER = 256;
	uint16_t bufferSize = SBUFFER;

  bufferState_t parseState = WAITING_FOR_START;
  
  uint8_t endOfLine = '\n';

  String pendingCommandString = "";
  bool isCommandPending = false;
	
	const uint16_t internalCommandItems = INTERNAL_COMMAND_ITEMS;
	const char* internalCommands[INTERNAL_COMMAND_ITEMS];
};



	
	
#endif //Commander_h