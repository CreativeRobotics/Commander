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
  {"get float 2",  getFloat2Handler, "Get a float"}
};


/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/

void initialiseCommander(){
   //Start Commander and attach the file stream object, and the serial port for replies
  //begin(Stream object for input, stream object for output, command array, size of command array)
  //Attach the outgoing port to Serial
  //Attach the command list and the list size variable
  cmd.begin(&myFile, &Serial,  masterCommands, sizeof(masterCommands));
  //enable printing of comment lines
  cmd.printComments(true);
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
