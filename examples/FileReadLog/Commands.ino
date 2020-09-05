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


/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/

void initialiseCommander(){
  //Start Commander and attach the incoming port to the File stream
  //Attach the outgoing port to Serial
  //Attach the command list and the list size variable
  cmd.begin(&myFile, &myLogFile,  masterCommands, sizeof(masterCommands));
  //attach Serial to the alt port and enable echoing of messages to alt so we can see whats happening
  cmd.attachAltPort(&Serial);
  //enable printing of comment lines
  cmd.printComments(true);
  //Echo to alt will copy any characters arriving on the input port to the Alt port - any commands sent to Commander will be copied to the Alt port
  cmd.echoToAlt(true);
  //CopyReplyAlt will print any replies to the alt port - Anything sent using the internal print commands will be sent to the Alt port as well as the Output port
  cmd.copyRepyAlt(true);
  //Uncomment this to have the actual commands logged to the log file as well as just the responses
  //cmd.echo(true);
    /*
   * With both the options above enabled some things will appear twice on the alt port,
   * for example with comment printing enabled the comment will be sent to alt once as the command input (echoed to alt) 
   * and again as the response (copied reply to alt)
   * With normal echoing disabled the log file will only be sent responses to commands, not the actual commands themselves.
   */
}


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
