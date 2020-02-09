/*Commander example - single object, multi layer command structure
 * Demonstrates how to use one commander object with multiple command arrays to create a layerd command structure
 * Top level commands can be invoked by typing them
 * Lower commands can be directly invoked from the top layer by typing the top command followed by the lower command
 * For example 'get help' calls the 'help' function in the command set called 'get'
 * Lower command structures can be entered by typing their command, and then the lower level commands can be directly invoked
 * An exit command will return control to the top level
 * For example, 'get' will transfer control to the 'get' command set. 'help' will then call the help function for the 'get' commands.
 */
#include <Commander.h>
extern const uint16_t numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const uint16_t numOfGetCmds; //This is a forward declaration so the compiler knows we are going to declare this variable properly later
extern const uint16_t numOfSetCmds; //This is a forward declaration so the compiler knows we are going to declare this variable properly later
extern const commandList_t masterCommands[]; //forward declare the master command list
extern const commandList_t getCommands[]; //forward declare the get command list
extern const commandList_t setCommands[]; //forward declare the set command list
//portSettings_t savedSettings;
Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;

//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  cmd.begin(&Serial, masterCommands, numOfMasterCmds);
  cmd.commanderName = "Cmd";
  cmd.commandPrompt(ON);
  cmd.echo(true);
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  //Call the update functions using the activeCommander pointer
  cmd.update();
}
