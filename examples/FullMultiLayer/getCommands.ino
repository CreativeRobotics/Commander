//All commands for 'get'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t getCommands[] = {
  {"hello",    getHelloHandler, "Say hello"},
  {"int",      getIntVariable,  "get my int variable"},
  {"float",    getFloatVariable,"get my float variable"},
  {"exit",     exitGet,         "Exit get command mode"},
};
 /*
  * This needs to be passed to the commander object so it knows how big the array of commands is, but this happens earlier in setup().
  * This has already been forward declared before setup() so the compiler knows it exists
  */
/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/
//initialise the numOfGetCmds variable after creating the getCommands[] array - numOfGetCmds now indicates the length of the array
const uint16_t numOfGetCmds = sizeof(getCommands);

//COMMAND HANDLERS ---------------------------------------------------------------------------

bool getHelloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool getIntVariable(Commander &Cmdr){
  Cmdr.print("my int is ");
  Cmdr.println(myInt);
  return 0;
}

bool getFloatVariable(Commander &Cmdr){
  Cmdr.print("My float is ");
  Cmdr.println(myFloat);
  return 0;
}

bool exitGet(Commander &Cmdr){
  //had over control to the sub commander
  Cmdr.println("Passing control back to main command handler");
  //Set the Commander pointer to the sub commander 1 object
  activeCommander = &masterCmd;
  //Transfer this Cmdr to the new active commander
  activeCommander->transfer(Cmdr);
  //Restore this commanders settings from the backup before we exit
  Cmdr.portSettings(savedSettings);
  return 0;
}
