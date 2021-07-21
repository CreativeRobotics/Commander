
#include "CommandHelpTags.h"


bool getCommandArgCode(char helpText[], cmdArgs_t commandArguments){
	//look for brackets:
	int16_t firstBracket = -1;
	int16_t lastBracket = -1;
	uint8_t idx = 0;
	//find the opening and closing brackets
	while(helpText[idx] != '\0'){
		//step through the help string
		if(helpText[idx] 			== CMD_ARG_START_BRACKET){
			if(idx > 1) break; //if the bracket is not in the 1st or 2nd place, exit.
			firstBracket = idx; //first bracket found
		}
		else if(helpText[idx] == CMD_ARG_END_BRACKET){
			lastBracket = idx;
			break;
		}
		idx++;
	}
	//Tags MUST appear at the start of a line, or directly after a hide character ('-') return false because there were no tags
	if(firstBracket !=0 || lastBracket < 1 || firstBracket != 1) return 0;
	switch(helpText[firstBracket+1]){
		case CMD_ARG_NONE:
			commandArguments.argumentType = CMD_NO_ARGS;
			break;
		case CMD_ARG_INT:
			commandArguments.argumentType = CMD_INT;
			break;
		case CMD_ARG_FLOAT:
			commandArguments.argumentType = CMD_FLOAT;
			break;
		case CMD_ARG_STRING:
			commandArguments.argumentType = CMD_STRING;
			break;
		case CMD_ARG_BINARY:
			commandArguments.argumentType = CMD_BOOL;
			break;
		case CMD_ARG_ONOFF:
			commandArguments.argumentType = CMD_ONOFF;
			break;
		case CMD_ARG_TOGGLE:
			commandArguments.argumentType = CMD_TOGGLE;
			break;
		case CMD_ARG_DATASTREAM:
			commandArguments.argumentType = CMD_DATASTREAM;
			break;
	}
	commandArguments.numberOfArguments = 1;
	if( helpText[firstBracket+2] > 47 && helpText[firstBracket+2] < 58 ){
		commandArguments.numberOfArguments = helpText[firstBracket+2] -48 ;
		if(helpText[firstBracket+3] < 48 || helpText[firstBracket+3] > 57 ) commandArguments.numberOfArguments = (commandArguments.numberOfArguments*10) + (helpText[idx]-48);
	}
	if( helpText[lastBracket] == CMD_CHAINABLE ) commandArguments.chainable = true;
	return true;
}

