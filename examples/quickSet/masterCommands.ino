//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler, "Say hello"},
  {"set",        setHandler,   "Quick set commands. Help: \"set help\""},
  {"get",        getHandler,   "Quick get commands. Help: \"get help\""},
  {"set int",        getIntHandler,   "Get many ints"},
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
bool getIntHandler(Commander &Cmdr){
  int mInt = 0;
  while(Cmdr.getInt(mInt)){
    Cmdr.println(mInt);
  }
  Cmdr.println(mInt);
  return 0;
}
