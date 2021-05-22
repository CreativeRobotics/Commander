//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",       helloHandler,     "hello"},
  {"get int",     getIntHandler,    "get an int"},
  {"set int",     setIntHandler,    "set an int"},
  {"get float",   getFloatHandler,  "get a float"},
  {"set float",   setFloatHandler,  "set a float"},
  {"hidden1",     hiddenHandler,    "-Command hidden from help"},
  {"myint",       setIntHandler,    "try myint=23"},
  {"myfloat",     setFloatHandler,  "try myfloat=23.5"},
  {"set ints",    setIntsHandler,   "set up to four ints"},
  {"set floats",  setFloatsHandler, "set up to four floats"},
  {"set strings", setStringsHandler,"set up to four Strings"},
  {"hidden2",     hiddenHandler,    "-Command hidden from help"},
};

/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/
void initialiseCommander(){
  cmd.begin(&Serial, masterCommands, sizeof(masterCommands));
  cmd.commandPrompt(ON); //enable the command prompt
  cmd.echo(true);     //Echo incoming characters to theoutput port
  cmd.errorMessages(ON); //error messages are enabled - it will tell us if we issue any unrecognised commands
  cmd.autoChain(ON);
  cmd.setUserString(deviceInfo);
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
bool getIntHandler(Commander &Cmdr){
  Cmdr.print("myInt = ");
  Cmdr.println(myInt);
  //Cmdr.printDiagnostics();
  return 0;
}

bool setIntHandler(Commander &Cmdr){
  if(Cmdr.getInt(myInt)){
    Cmdr.print("myInt set to ");
    Cmdr.println(myInt);
  }
  //Cmdr.printDiagnostics();
  return 0;
}
bool getFloatHandler(Commander &Cmdr){
  Cmdr.print("myFloat = ");
  Cmdr.println(myFloat);
  
  //Cmdr.printDiagnostics();
  return 0;
}

bool setFloatHandler(Commander &Cmdr){
  if(Cmdr.getFloat(myFloat)){
    Cmdr.print("myFloat set to ");
    Cmdr.println(myFloat, 4); //print with 4 decimal places
  }
  //Cmdr.printDiagnostics();
  return 0;
}

bool setIntsHandler(Commander &Cmdr){
  //create an array to store any values we find
  int values[4] = {0,0,0,0};

  int itms = Cmdr.countItems();
  Cmdr.print("There are ");
  Cmdr.print(itms);
  Cmdr.println(" items in the payload");
  
  for(int n = 0; n < 4; n++){
    //try and unpack an int, if it fails there are no more left so exit the loop
    if(Cmdr.getInt(values[n])){
      Cmdr.print("unpacked ");
      Cmdr.println(values[n]);
    }else break;
  }
  //print it out
  Cmdr.println("Array contents after processing:");
  for(int n = 0; n < 4; n++){
    Cmdr.print(n);
    Cmdr.print(" = ");
    Cmdr.println(values[n]);
  }
  //Cmdr.chain();
  //Cmdr.printDiagnostics();
  return 0;
}

bool setFloatsHandler(Commander &Cmdr){
  float values[4] = {0.0,0.0,0.0,0.0};
  int itms = Cmdr.countItems();
  Cmdr.print("There are ");
  Cmdr.print(itms);
  Cmdr.println(" items in the payload");
  
  for(int n = 0; n < 4; n++){
    if(Cmdr.getFloat(values[n])){
      Cmdr.print("unpacked ");
      Cmdr.println(values[n]);
    }else break;
  }
  Cmdr.println("Array contents after processing:");
  for(int n = 0; n < 4; n++){
    Cmdr.print(n);
    Cmdr.print(" = ");
    Cmdr.println(values[n]);
  }
  //Cmdr.chain();
  //Cmdr.printDiagnostics();
  return 0;
}

bool setStringsHandler(Commander &Cmdr){
  String myString = "";
  int itms = Cmdr.countItems();
  Cmdr.print("There are ");
  Cmdr.print(itms);
  Cmdr.println(" items in the payload");
  for(int n = 0; n < itms; n++){
    if(Cmdr.getString(myString)){
      Cmdr.print("String ");
      Cmdr.print(n);
      Cmdr.print(" = ");
      Cmdr.println(myString);
    }else Cmdr.println("Operation failed");
  }
  //Cmdr.chain();
  //Cmdr.printDiagnostics();
  return 0;
}

bool hiddenHandler(Commander &Cmdr){
  Cmdr.println("This command is hidden from the help system");
  //Cmdr.printDiagnostics();
  return 0;
}
