//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler,        "Say hello"},
  {"set",        setHandler,          "Quick set commands. Help: \"set help\""},
  {"get",        getHandler,          "Quick get commands. Help: \"get help\""},
  {"set prefix", setPrefixHandler,    "Sets the prefix string. EG: 'set prefix PREFIX'"},
  {"set postfix",setPostfixHandler,   "Sets the postfix string. EG: 'set prefix POSTFIX'"},
  {"prefix",     prefixHandler,       "Changes the prefix text to 'PREFIXED REPLY:' - use 'prefix hello' to send the hello command"},
  {"HTML",       formatHTMLHandler,   "Set pre and postfix lines to '<br>' and reloads any payload for handling EG:'HTML help' gets a formatted help page"},
  {"reload",     formatAgainHandler,  "Passes any payload to commander and enables formatting - uses any existing pre and postfix text"},
  {"toggle",     toggleFmtHandler,    "toggle formatting between always on or always off"},
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

bool setPrefixHandler(Commander &Cmdr){
  //Extract any payload and use it as the prefix message
  //Uses getPayloadString to extract the payload without any newline
  Cmdr.setPrefix(Cmdr.getPayloadString());
  Cmdr.print("Set prefix to: ");
  Cmdr.println(Cmdr.getPayloadString());
}

bool setPostfixHandler(Commander &Cmdr){
  //Extract any payload and use it as the postfix message
  //Uses getPayloadString to extract the payload without any newline
  Cmdr.setPostfix(Cmdr.getPayloadString());
  Cmdr.print("Set postfix to: ");
  Cmdr.println(Cmdr.getPayloadString());
}

bool prefixHandler(Commander &Cmdr){
  //The setPrefix or setPostfix commands always enable the pre or postfix behaviour for the command being handled
  //If autoformat is NOT enabled then the pre/postfix behaviour will end when commander returns control to the user code
  //Using just the setPrefix command will cause ONLY the prefix message to be printed whilst the command is being handled.
  //Here we are checking to see if this command came with a payload, and if so we send the payload back to Commander for processing
  //If the payload was a command then it will get handled, and responses will be formatted.
  Cmdr.setPrefix("PREFIXED REPLY:");
  if(Cmdr.hasPayload()){
    Cmdr.feedString(Cmdr.getPayload());
  }else{
    Cmdr.print("here is part of my reply");
    Cmdr.println(" here is eol");
    Cmdr.println("Next line ...");
    Cmdr.print("Here is an int: ");
    Cmdr.println(myInt1);
    Cmdr.print("Here is a float: ");
    Cmdr.print(myFloat1);
    Cmdr.println(" and another line ... ");
  }
  return 0;
}

bool formatHTMLHandler(Commander &Cmdr){
  //The setPrefix or setPostfix commands always enable the pre or postfix behaviour for the command being handled
  //If autoformat is NOT enabled then the pre/postfix behaviour will end when commander returns control to the user code
  //Here we are checking to see if this command came with a payload, and if so we send the payload back to Commander for processing
  //If the payload was a command then it will get handled, and responses will be formatted.
  Cmdr.setPrefix("<p>");
  Cmdr.setPostfix("</p>");
  if(Cmdr.hasPayload()){
    Cmdr.feedString(Cmdr.getPayload());
  }else{
    Cmdr.print("here is part of my reply");
    Cmdr.println(" here is eol");
    Cmdr.println("Next line ...");
    Cmdr.print("Here is an int: ");
    Cmdr.println(myInt1);
    Cmdr.print("Here is a float: ");
    Cmdr.print(myFloat1);
    Cmdr.println(" and another line ... ");
  }
  return 0;
}

bool formatAgainHandler(Commander &Cmdr){
  //The startFormatting function will enable pre and postfix formatting for the commadn heing handled.
  //If autoformat is NOT enabled then the pre/postfix behaviour will end when commander returns control to the user code
  //Here we are checking to see if this command came with a payload, and if so we send the payload back to Commander for processing
  //If the payload was a command then it will get handled, and responses will be formatted.
  Cmdr.startFormatting();
  if(Cmdr.hasPayload()){
    Cmdr.feedString(Cmdr.getPayload());
  }else{
    Cmdr.print("here is part of my reply");
    Cmdr.println(" here is eol");
    Cmdr.println("Next line ...");
    Cmdr.print("Here is an int: ");
    Cmdr.println(myInt1);
    Cmdr.print("Here is a float: ");
    Cmdr.print(myFloat1);
    Cmdr.println(" and another line ... ");
  }
  return 0;
}


bool toggleFmtHandler(Commander &Cmdr){
  //Setting autoFormat to true will make Commander add the pre and postfix messages to every response.
  //Pre and Postfix messages are Strings, if they are empty then nothing will be printed
  //EG: If you need autoformatting and only want a postfix message then you can set the prefix message to ""
  Cmdr.autoFormat( !Cmdr.autoFormat() ); //Get current autoformat state, invert it and set it as the new autoformat state.
  Cmdr.autoFormat() ? Cmdr.println("Autoformat ON") : Cmdr.println("Autoformat OFF");
  return 0;
}
