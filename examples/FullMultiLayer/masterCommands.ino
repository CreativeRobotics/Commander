//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler, "Say hello"},
  {"get",        getHandler,   "call get commands"},
  {"set",        setHandler,   "call set commands"},
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

//initialise the numOfMasterCmds variable after creating the masterCommands[] array - numOfMasterCmds now indicates the length of the array
const uint16_t numOfMasterCmds = sizeof(masterCommands);


//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool getHandler(Commander &Cmdr){
  //This handles commands that have a list of 'get' commands.
  //sending just the command will pass control to the get command handler
  //Sending the command, plus one of the 'get' commands will pass this command to the get command handler, 
  //the command will be carried out, and control will return to the master.
  //For example:
  //Sending 'get' passes control to get. Sending 'help' will then call gets help command.
  //Sending 'get help' will call gets help command, but if you then send 'help', it will call masters help command because we never transferred control to get
  bool retVal = 0;
  if(Cmdr.hasPayload()){
    Cmdr.println("handing payload to get command object");
    retVal  = getcmd.feed(Cmdr);
  }else{
    Cmdr.println("handing control to get command object");
    //backup the port setting from the commander we are about to transfer control to.
    savedSettings = getcmd.portSettings();
    //Set the Commander pointer to the sub commander 1 object
    activeCommander = &getcmd;
    //Transfer this Cmdr to the new active commander
    activeCommander->transfer(Cmdr);
  }
  return retVal;
}

bool setHandler(Commander &Cmdr){
  //This handles commands that have a list of 'set' commands.
  //sending just the command will pass control to the set command handler
  //Sending the command, plus one of the 'set' commands will pass this command to the cub command handler, 
  //the command will be carried out, and control will return to the master.
  //For example:
  //Sending 'set' passes control to set. Sending 'help' will then call sets help command.
  //Sending 'set help' will call sets help command, but if you then send 'help', it will call masters help command because we never transferred control to set
  bool retVal = 0;
  if(Cmdr.hasPayload()){
    Cmdr.println("handing payload to set command object");
    retVal  = setcmd.feed(Cmdr);
  }else{
    Cmdr.println("handing control to set command object");
    //backup the port setting from the commander we are about to transfer control to.
    savedSettings = setcmd.portSettings();
    //Set the Commander pointer to the sub commander 1 object
    activeCommander = &setcmd;
    //Transfer this Cmdr to the new active commander
    activeCommander->transfer(Cmdr);
  }
  return retVal;
}
