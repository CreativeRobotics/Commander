//All commands for 'set'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t setCommands[] = {
  {"hello",    setHelloHandler, "Say hello"},
  {"int",      setIntVariable,     "set my int variable"},
  {"float",    setfloatVariable,     "set my float variable"},
  {"exit",     exitSet,         "Exit sub command mode"},
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
//initialise the numOfSetCmds variable after creating the setCommands[] array - numOfSetCmds now indicates the length of the array
const uint16_t numOfSetCmds = sizeof(setCommands);

//COMMAND HANDLERS ---------------------------------------------------------------------------

bool setHelloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool setIntVariable(Commander &Cmdr){
  if(Cmdr.getInt(myInt)){
    Cmdr.print("Setting my int to ");
    Cmdr.println(myInt);
  }else Cmdr.println("Error");
  return 0;
}

bool setfloatVariable(Commander &Cmdr){
  if(Cmdr.getFloat(myFloat)){
    Cmdr.print("Setting my float to ");
    Cmdr.println(myFloat);
  }else Cmdr.println("Error");
  return 0;
}

bool exitSet(Commander &Cmdr){
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
