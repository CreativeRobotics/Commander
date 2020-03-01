//Commander help tags for auto GUI generation
/*
Argument tags can be placed at the start of a help string and indicate what type of argument should come with the command
This can be used to either generate extended help, or to generate user interface elements based on each command
For example a line of HTML can be generated from a command that includes a button, and data entry field, for issuing a command

Tags MUST be at the start of a help message and enclosed in square brackets
Tags can have a number after them to indicate the number of arguments
For int and float arguments this indicates the number of values, EG [I3] indicates an argument with three ints
For text artuments it indicates the number of words or phrases, for example a tag [T2] indicates two text fields (for example for setting an SSID and password)

By default, no tag should result in any GUI generator producing a button with a generic text entry field because arguments appearing after commands that should not have an argument are still handled (the argument is ditched)
The [X] tag explicitly marks a command as having no arguments and therefore any GUI element should have no text field.
Tags can be added together, for example [TS] indicates a toggle for streaming data.
*/
#ifndef CommandHelpTags_h
#define CommandHelpTags_h


#include <Arduino.h>
#include <string.h>

//Hide command when printing help
#define CMD_HIDE_HELP				'-'
//Command is chainable
#define CMD_CHAINABLE		'C'

#define CMD_ARG_START_BRACKET '['
#define CMD_ARG_END_BRACKET 	']'
//No arguments for this command
#define CMD_ARG_NONE       	'X'
//n Int arguments (defaults to 1 if no number is included with the tag)
#define CMD_ARG_INT        	'I'
//n Float arguments (defaults to 1 if no number is included after the tag)
#define CMD_ARG_FLOAT      	'F'
//String argument. A number after the string indicates the number of words
#define CMD_ARG_STRING     	'S'
//Binary argument consisting of the word 'true' or 'false'
#define CMD_ARG_BINARY     	'B'
//Binary argument consisting of the word 'on' or 'off'
#define CMD_ARG_ONOFF      	'O'
//Command will toggle a setting when issued
#define CMD_ARG_TOGGLE     	'T'
//Command is associated with streaming data
#define CMD_ARG_DATASTREAM 	'D'



typedef enum cmdArgType_t{
	CMD_NO_TAGS = 0,
	CMD_NO_ARGS,
	CMD_INT,
	CMD_FLOAT,
	CMD_STRING,
	CMD_BOOL,
	CMD_ONOFF,
	CMD_TOGGLE,
	CMD_DATASTREAM,
} cmdArgType_t;

//data strtucture to hold an argument type
typedef struct cmdArgs_t{
	uint8_t numberOfArguments = 0;
	cmdArgType_t argumentType = CMD_NO_TAGS;
	bool chainable = false;
}cmdArgs_t;

bool getCommandArgCode(char helpText[], cmdArgs_t commandArguments);

#endif //CommandHelpTags_h
