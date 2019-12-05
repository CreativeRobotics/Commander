//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"status", sdStatusHandler, "Check SD Card status"},
  {"SD", sdFileHandler, "Open SD explorer"},
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
bool sdStatusHandler(Commander &Cmdr){
  if(!SDOK){
    Cmdr.println("No SD Card");
  }else{
    Cmdr.println("SD Card detected");
    //Cmdr.println(Cmdr.getPayloadString());
  }
  return 0;
}

bool sdFileHandler(Commander &Cmdr){
  if(!SDOK){
    Cmdr.println("No SD Card");
    return 0;
  }
  //For the moment - block commands if they are chained
  if(Cmdr.hasPayload()){
    Cmdr.println("Chained commands are disabled");
    //This is blocking when using coolterm ...?
    return 0;
  }
  Cmdr.println("Opening SD Navigator");
  //Cmdr.attachCommands(getCommands, numOfGetCmds);
  cmdName = "SD:";
  if(Cmdr.transferTo(fileCommands, numOfFileCommands, cmdName)){
    //commander returns true if it is passing back control;
    Cmdr.transferBack(masterCommands, numOfMasterCmds, prompt);
  }
  return 0;
}
