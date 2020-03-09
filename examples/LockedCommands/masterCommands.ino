//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler,    "hello"},
  {"get int",    getIntHandler,   "get an int"},
  {"set int",    setIntHandler,   "set an int"},
  {"get float",  getFloatHandler, "get a float"},
  {"set float",  setFloatHandler, "set a float"},
  {"myint",      setIntHandler,   "try myint=23"},
  {"myfloat",    setFloatHandler, "try myfloat=23.5"},
  {"set password",   setPassHandler,   "set password"},
  {"toggle hl",   toggleHLHandler,   "toggle hard lock"},
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
  cmd.setPassPhrase(myPassphrase);
}

//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
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

bool setPassHandler(Commander &Cmdr){
  if(Cmdr.hasPayload()){
    myPassphrase = Cmdr.getPayloadString();
    Cmdr.print("Set password to: ");
    Cmdr.println(myPassphrase);
  }
  return 0;
}

bool toggleHLHandler(Commander &Cmdr){
  Cmdr.setLockType(!Cmdr.getLockType());
  Cmdr.getLockType() ? Cmdr.println("Hard Locking enabled") : Cmdr.println("Hard locking disabled");
  return 0;
}
