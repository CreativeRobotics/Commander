//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",       helloHandler,      "Say hello"},
  {"set int 1",   setInt1Handler,    "Set an int"},
  {"get int 1",   getInt1Handler,    "Get an int"},
  {"set int 2",   setInt2Handler,    "Set an int"},
  {"get int 2",   getInt2Handler,    "Get an int"},
  {"set float 1",  setFloat1Handler, "Set a float"},
  {"get float 1",  getFloat1Handler, "Get a float"},
  {"set float 2",  setFloat2Handler, "Set a float"},
  {"get float 2",  getFloat2Handler, "Get a float"},
  {"stop log",     closeLogFile,     "Stop command log"},
  {"print log",    printLogFile,     "Print the command log"}
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
bool setInt1Handler(Commander &Cmdr){
  if(Cmdr.getInt(myInt1)){
    Cmdr.print("Set Int 1 to ");
    Cmdr.println(myInt1);
  }else Cmdr.println("Set Int 1 failed");
  return 0;
}
bool getInt1Handler(Commander &Cmdr){
  Cmdr.print("Int 1 is ");
  Cmdr.println(myInt1);
  return 0;
}
bool setInt2Handler(Commander &Cmdr){
  if(Cmdr.getInt(myInt2)){
    Cmdr.print("Set Int 2 to ");
    Cmdr.println(myInt2);
  }else Cmdr.println("Set Int 2 failed");
  return 0;
}
bool getInt2Handler(Commander &Cmdr){
  Cmdr.print("Int 2 is ");
  Cmdr.println(myInt2);
  return 0;
}
bool setFloat1Handler(Commander &Cmdr){
  if(Cmdr.getFloat(myFloat1)){
    Cmdr.print("Set Float 1 to ");
    Cmdr.println(myFloat1);
  }else Cmdr.println("Set Float 1 failed");
  return 0;
}
bool getFloat1Handler(Commander &Cmdr){
  Cmdr.print("Float 1 is ");
  Cmdr.println(myFloat1);
  return 0;
}
bool setFloat2Handler(Commander &Cmdr){
  if(Cmdr.getFloat(myFloat2)){
    Cmdr.print("Set Float 2 to ");
    Cmdr.println(myFloat2);
  }else Cmdr.println("Set Float 2 failed");
  return 0;
}
bool getFloat2Handler(Commander &Cmdr){
  Cmdr.print("Float 2 is ");
  Cmdr.println(myFloat2);
  return 0;
}

bool closeLogFile(Commander &Cmdr){
  //!!! This crashes at the moment ...
  Cmdr.println("Closing Log File ...");
  myLogFile.close();
  //Detach the port and disable alt port copying
  //cmd.detachAltPort();
  cmd.echoToAlt(false);
  cmd.copyRepyAlt(false);
  return 0;
}


bool printLogFile(Commander &Cmdr){
  Cmdr.println("Printing Log File:");
  if(myLogFile){
    myLogFile.close();
    cmd.echoToAlt(false);
    cmd.copyRepyAlt(false);
  }
  if(!SD.exists("log.txt")){
    Cmdr.println("No log file found");
    return 0;
  }
  myLogFile = SD.open("log.txt", O_RDONLY);
  Cmdr.println("===================== LOG FILE START ===================");
  while(myLogFile.available()){
    Cmdr.write(myLogFile.read());
  }
  myLogFile.close();
  Cmdr.println("====================== LOG FILE END ====================");
  return 0;
}
