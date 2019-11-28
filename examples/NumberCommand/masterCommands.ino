//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler,    "hello"},
  {"get int",    getIntHandler,   "get an int"},
  {"set int",    setIntHandler,   "set an int"},
  {"get float",  getFloatHandler, "get a float"},
  {"set float",  setFloatHandler, "set a float"},
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



bool numberHandler(Commander &Cmdr){
  if(uploadTarget == INTRO) handleInNumber(Cmdr);
  else handleExNumber(Cmdr);
  return 0;
}
//----------------------------------------------------------------------------------
void handleInNumber(Commander &Cmdr){
  if(introvert.writeIndex < 0){
    Cmdr.println("Number handler inactive"); //do nothing, you are inactive
    uploading = false;
    Cmdr.echo(true);
  }
  else if(introvert.writeIndex < ARRAY_LENGTH){
    introvert.data[introvert.writeIndex++] = Cmdr.bufferString.toInt();
  }
  if(introvert.writeIndex >= ARRAY_LENGTH){
    //end of array reached - disable;
    introvert.writeIndex = -1;
    Cmdr.println("Finished uploading Introvert table");
    Cmdr.enablePrompt();
    uploading = false;
    Cmdr.echo(true);
  }
}



//----------------------------------------------------------------------------------
void handleExNumber(Commander &Cmdr){
  if(extrovert.writeIndex < 0){
    Cmdr.println("Number handler inactive"); //do nothing, you are inactive
    uploading = false;
    Cmdr.echo(true);
  }
  else if(extrovert.writeIndex < ARRAY_LENGTH){
    extrovert.data[extrovert.writeIndex++] = Cmdr.bufferString.toInt();
  }
  if(extrovert.writeIndex >= ARRAY_LENGTH){
    //end of array reached - disable;
    extrovert.writeIndex = -1;
    Cmdr.println("Finished uploading Extrovert table");
    Cmdr.enablePrompt();
    uploading = false;
    Cmdr.echo(true);
  }
}


//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool getIntHandler(Commander &Cmdr){
  Cmdr.print("myInt = ");
  Cmdr.println(myInt);
  return 0;
}

bool setIntHandler(Commander &Cmdr){
  if(Cmdr.getInt(myInt)){
    Cmdr.print("myInt set to ");
    Cmdr.println(myInt);
  }
  return 0;
}
bool getFloatHandler(Commander &Cmdr){
  Cmdr.print("myFloat = ");
  Cmdr.println(myFloat);
  return 0;
}

bool setFloatHandler(Commander &Cmdr){
  if(Cmdr.getFloat(myFloat)){
    Cmdr.print("myFloat set to ");
    Cmdr.println(myFloat, 4); //print with 4 decimal places
  }
  return 0;
}
