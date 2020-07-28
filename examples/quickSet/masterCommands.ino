//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler, "Say hello"},
  {"set",        setHandler,   "Quick set commands. Help: \"set help\""},
  {"get",        getHandler,   "Quick get commands. Help: \"get help\""},
  {"print",        printHandler,   "Print items. Help: \"print help\""},
  {"set int",        getIntHandler,   "Get many ints"},
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
  //Error messaged do NOT work for quick set and get commands
  
}
//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}


bool setHandler(Commander &Cmdr){
  //quickget function
  //Call quickSetHelp() first to handle any help command
  Cmdr.quickSetHelp();
  if( Cmdr.quickSet("int1",   myInt1) )   Cmdr.println("int1 set to " + String(myInt1));
  if( Cmdr.quickSet("int2",   myInt2) )   Cmdr.println("int2 set to " + String(myInt2));
  if( Cmdr.quickSet("float1", myFloat1) ) Cmdr.println("float1 set to " + String(myFloat1));
  if( Cmdr.quickSet("float2", myFloat2) ) Cmdr.println("float2 set to " + String(myFloat2));
  return 0;
}
bool getHandler(Commander &Cmdr){
  //quickset function
  //Call quickSetHelp() first to handle any help command
  Cmdr.quickSetHelp();
  Cmdr.quickGet("int1",   myInt1);
  Cmdr.quickGet("int2",   myInt2) ;
  Cmdr.quickGet("float1", myFloat1);
  Cmdr.quickGet("float2", myFloat2) ;
  return 0;
}


bool printHandler(Commander &Cmdr){
  //quickset function
  //Call quickSetHelp() first to handle any help command
  Cmdr.quickSetHelp();
  if(Cmdr.quick("int1")) Cmdr.println(myInt1);
  if(Cmdr.quick("int2")) Cmdr.println(myInt2);
  if(Cmdr.quick("float1")) Cmdr.println(myFloat1);
  if(Cmdr.quick("float2")) Cmdr.println(myFloat2);
  if(Cmdr.quick("hello")) Cmdr.println("Hello world");
  return 0;
}
bool getIntHandler(Commander &Cmdr){
  int mInt = 0;
  while(Cmdr.getInt(mInt)){
    Cmdr.println(mInt);
  }
  Cmdr.println(mInt);
  return 0;
}
