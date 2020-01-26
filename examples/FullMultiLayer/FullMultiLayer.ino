/*Commander example - multi object, multi layer command structure
 * Demonstrates how to use many commander objects each with its own command array to create a layerd command structure
 * Top level commands can be invoked by typing them
 * Lower commands can be directly invoked from the top layer by typing the top command followed by the lower command
 * For example 'get help' calls the 'help' function in the command set called 'get'
 * Lower command structures can be entered by typing their command, and then the lower level commands can be directly invoked
 * An exit command will return control to the top level
 * For example, 'get' will transfer control to the 'get' command set. 'help' will then call the help function for the 'get' commands.
 */
 #include <Commander.h>

extern const commandList_t masterCommands[]; //forward declare the master command list
extern const uint16_t numOfMasterCmds;       //A forward declaration so the compiler knows we are going to initialise this variable properly later
extern const commandList_t getCommands[];    //forward declare the get command list
extern const uint16_t numOfGetCmds;          //A forward declaration so the compiler knows we are going to initialise this variable properly later
extern const commandList_t setCommands[];    //forward declare the set command list
extern const uint16_t numOfSetCmds;          //A forward declaration so the compiler knows we are going to initialise this variable properly later

portSettings_t savedSettings;                //Create a portSettings_t variable to store Commander settings in when we switch to a different command object.

//Declare the three commander objects
Commander masterCmd;
Commander getcmd;
Commander setcmd;

//Create a pointer to a commander object, and point it to the master object
//We will change this later to point to a sub command object when we transfer control.
Commander* activeCommander = &masterCmd;

//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  //initialise the commander objects
  masterCmd.begin(&Serial, masterCommands, numOfMasterCmds);
  getcmd.begin(&Serial, getCommands, numOfGetCmds);
  setcmd.begin(&Serial, setCommands, numOfSetCmds);
  //give each one a name
  masterCmd.commanderName = "Master commander";
  getcmd.commanderName = "get commander";
  setcmd.commanderName = "set commander";
  //enable command prompts for the master command object
  masterCmd.commandPrompt(ON);
  //enable multi commander mode for all command objects
  masterCmd.multiCommander(true);//enable multicommander so command prompts work properly with multiple commander objects
  getcmd.multiCommander(true);//enable multicommander so command prompts work properly with multiple commander objects
  setcmd.multiCommander(true);//enable multicommander so command prompts work properly with multiple commander objects
  //enable echoing - any characters we send will be echoed back
  //We only need to do this for the master because the settings will be copied to any sub command object we use
  masterCmd.echo(true);
  //wait for a serial port to open
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  //print the command prompt
  masterCmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  //Call the update function using the activeCommander pointer
  //This will call the update() function on whatever commander object it is currently pointing to.
  activeCommander->update();
}
