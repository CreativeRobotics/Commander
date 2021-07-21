//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------

String helpStr = "This is the normal help text";
String exHelpStr = "This is the extended help text for this command. It can be changed with another command.";
String cmdStr = "test";

commandList_t masterCommands[] = {
  {cmdStr.c_str(), helloHandler,           helpStr.c_str()},
  {"set help",            setStringHandler,       "Set help text for the first command"},
  {"set extra help",      setExtraStringHandler,  "Set extended help for first command"},
  {"set command",         setCommandHandler,      "Set command text for the first command"},
};


const char * extraHelp[] = {
  (char*)exHelpStr.c_str(),
  "'set help'\tSet the text that will display for the normal help of the first command.\nMake sure you don't exceed Commander buffer size!",
  "'set extra help'\tSet the text that will display for the extended help of the first command.\nMake sure you don't exceed Commander buffer size!",
  "'set command'\tThis will set the actual command text used for the first command in the list.",
};
/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/


//Initialisation function that avoids having to forward declare the command array and a size variable.
void initialiseCommander(){
  cmd.begin(&Serial, masterCommands, sizeof(masterCommands))
                                                            .commandPrompt(ON)
                                                            .echo(true)
                                                            .errorMessages(ON)
                                                            .autoChain(OFF)
                                                            .setUserString(deviceInfo)
                                                            .setExtraHelp(extraHelp);
}


//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}

bool setStringHandler(Commander &Cmdr){
  //String myString = "";

  if(Cmdr.hasPayload()){
    helpStr = Cmdr.getPayloadString();
    //The pointer may have changed so get a new one
    masterCommands[0].manualString = helpStr.c_str();
    Cmdr.print("Help string changed to:");
    Cmdr.println(helpStr);
  }
  else{
    Cmdr.println("No payload. String is still:");
    Cmdr.println(helpStr);
  }
  return 0;
}


bool setExtraStringHandler(Commander &Cmdr){
  //String myString = "";

  if(Cmdr.hasPayload()){
    exHelpStr = Cmdr.getPayloadString();
    //The pointer may have changed so get a new one
    extraHelp[0] = exHelpStr.c_str();
    Cmdr.print("Extended help string changed to:");
    Cmdr.println(exHelpStr);
  }
  else{
    Cmdr.println("No payload. String is still:");
    Cmdr.println(exHelpStr);
  }
  return 0;
}


bool setCommandHandler(Commander &Cmdr){
  //String myString = "";

  if(Cmdr.hasPayload()){
    cmdStr = Cmdr.getPayloadString();
    //The pointer may have changed so get a new one
    masterCommands[0].commandString = cmdStr.c_str();
    Cmdr.print("Command string changed to:");
    Cmdr.println(cmdStr);
    Cmdr.reloadCommands();
  }
  else{
    Cmdr.println("No payload. Command string is still:");
    Cmdr.println(cmdStr);
  }
  return 0;
}
