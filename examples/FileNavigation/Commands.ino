//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"mkdir", makeDirectory, "make a sub directory"},
  {"chdir", changeDirectory, "Change directory - use / for root"},
  {"ls", printDirectory, "Print directory"},
  {"remove", removeFile, "Delete a file"},
  {"removedir", removeDirectory, "Delete an empty directory"},
  {"rename", renameFile, "rename a file or sub directory"},
  {"print", printFileContents, "Print file contents to terminal"},
  {"status", noCardError, "check card status"},
   
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
bool makeDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.mkdir( Cmdr.getPayloadString().c_str() )){
    Cmdr.print("Created: ");
    Cmdr.println(Cmdr.getPayloadString());
  }
  return 0;
}
bool changeDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.chdir( Cmdr.getPayloadString().c_str() )){
    Cmdr.print("In: ");
    Cmdr.println(Cmdr.getPayloadString());
    if(Cmdr.getPayloadString() == "/" ) Cmdr.commanderName = cmdName + Cmdr.getPayloadString();
    else Cmdr.commanderName = cmdName + " /" + Cmdr.getPayloadString();
  }else Cmdr.println("Error - no such directory");
  return 0;
}
bool printDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);

  SD.ls(Cmdr.getOutputPort(), LS_R);
  return 0;
}

bool removeFile(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.remove( Cmdr.getPayloadString().c_str())){
    Cmdr.print("Removed: ");
    Cmdr.println(Cmdr.getPayloadString());
  }else{
    Cmdr.println("operation failed");
  }
  return 0;
}
bool removeDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.rmdir( Cmdr.getPayloadString().c_str())){
    Cmdr.print("Removed: ");
    Cmdr.println(Cmdr.getPayloadString());
  }else{
    Cmdr.println("operation failed");
  }
  return 0;
}
bool renameFile(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  String pld = Cmdr.getPayloadString(); //get the payload without any newline
  int idx = pld.indexOf(" "); //find the first space
  if(idx == -1){
    Cmdr.println("Error, unable to handle command");
    return 0;
  }
  String sub1 = pld.substring(0, idx); //get the first word.
  String sub2 = pld.substring(idx+1, pld.length());
  Cmdr.print("Changing ");
  Cmdr.print(sub1);
  Cmdr.print(" to ");
  Cmdr.println(sub2);
  if(SD.rename(sub1.c_str(), sub2.c_str())){
    Cmdr.println("File or directory has been renamed");
  }else   Cmdr.println("Error: Operation failed");
  return 0;
}

bool printFileContents(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  File tmpFile = SD.open(Cmdr.getPayloadString().c_str() , O_RDONLY);
  while(tmpFile.available()){
    Cmdr.write(tmpFile.read());
  }
  Cmdr.write('\n');
  tmpFile.close();
  return 0;
}

bool noCardError(Commander &Cmdr){
  if(!SDOK) Cmdr.println("ERR: No SD Card");
  else  Cmdr.println("Card detected");
  return 0;
}
