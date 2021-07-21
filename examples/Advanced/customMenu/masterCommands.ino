//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
//All the strings are const, so the are stored in flash
//We need a string for each command and its help text
const char* command0 = "hello";
const char* help0 = "say hello";
const char* command1 = "hello1";
const char* help1 = "say hello1";
const char* command2 = "hello2";
const char* help2 = "say hello2";
const char* command3 = "hello3";
const char* help3 = "say hello3";
const char* command4 = "hello4";
const char* help4 = "say hello4";


//These are some other commands we need
const char* listCommand = "list";
const char* listhelp = "List available commands";


const char* createCommand = "create";
const char* createHelp = "Create a command list (eg 'create 2 4 3')";

const char* revertCommand = "revert";
const char* revertHelp = "revert to the old list";

const commandList_t masterCommands[] = {
  {listCommand,          listHandler,        listhelp},
  {createCommand,        createHandler,      createHelp},
};

commandList_t *newCmds;
/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/
/*
  {(char*)command0,       helloHandler,     (char*)help0},
  {(char*)command1,       helloHandler,     (char*)help1},
  {(char*)command2,       helloHandler,     (char*)help2},
  {(char*)command3,       helloHandler,     (char*)help3},
  {(char*)command4,       helloHandler,     (char*)help4},
*/
//Initialisation function that avoids having to forward declare the command array and a size variable.
void initialiseCommander(){
  cmd.begin(&Serial, masterCommands, sizeof(masterCommands))
                                                            .commandPrompt(ON)
                                                            .echo(true)
                                                            .errorMessages(ON)
                                                            .autoChain(OFF)
                                                            .setUserString(deviceInfo);
}

//Three command handlers.
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}
bool hello1Handler(Commander &Cmdr){
  Cmdr.print("Hello 1! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}
bool hello2Handler(Commander &Cmdr){
  Cmdr.print("Hello 2! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}
bool hello3Handler(Commander &Cmdr){
  Cmdr.print("Hello 3! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}
bool hello4Handler(Commander &Cmdr){
  Cmdr.print("Hello 4! this is ");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("This is my buffer: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}
bool listHandler(Commander &Cmdr){
  Cmdr.println("List of available functions:");
  Cmdr.println("0 = hello");
  Cmdr.println("1 = hello1");
  Cmdr.println("2 = hello2");
  Cmdr.println("3 = hello3");
  Cmdr.println("4 = hello4");
  return 0;
}

bool revertHandler(Commander &Cmdr){
  Cmdr.println("Reverting to old list");
  //delete the old list.
  delete [] newCmds;
  Cmdr.attachCommands(masterCommands, sizeof(masterCommands));
  return 0;
}

bool createHandler(Commander &Cmdr){
  int items = Cmdr.countItems();
  int *values = new int[items];
  for(int n = 0; n < items; n++){
    //try and unpack an int, if it fails there are no more left so exit the loop
    if(Cmdr.getInt(values[n])){
      Cmdr.print("unpacked ");
      Cmdr.println(values[n]);
    }else{
      Cmdr.println("Syntax Error");
      return 0;
    }
  }
  //create a new array for the custom list, with one extra item for the revert option
  newCmds = new commandList_t[items+1];


  //now assign the correct strings and handlers
  for(uint8_t n = 0; n < items; n++){
    newCmds[n].commandString  = getCommand(values[n]);
    newCmds[n].handler        = getHander(values[n]);
    newCmds[n].manualString   = getHelp(values[n]);
  }
  //tack the revert option to the end of the array
  newCmds[items].commandString  = revertCommand;
  newCmds[items].handler        = revertHandler;
  newCmds[items].manualString   = revertHelp;
  //Attach it to commander using the attachCommandArray method
  Cmdr.attachCommandArray(newCmds, items+1);
  Cmdr.println("New command list created and activated");
  return 0;
}

//Functions to get the appropriate text and function pointers when creating the command array.
char* getCommand(int item){
  switch(item){
    case 0: return (char*)command0;
    case 1: return (char*)command1;
    case 2: return (char*)command2;
    case 3: return (char*)command3;
    case 4: return (char*)command4;
    default: return (char*)command0; 
  }
}

char* getHelp(int item){
  switch(item){
    case 0: return (char*)help0;
    case 1: return (char*)help1;
    case 2: return (char*)help2;
    case 3: return (char*)help3;
    case 4: return (char*)help4;
    default: return (char*)help0; 
  }
}

cmdHandler getHander(int item){
  switch(item){
    case 0: return helloHandler;
    case 1: return hello1Handler;
    case 2: return hello2Handler;
    case 3: return hello3Handler;
    case 4: return hello4Handler;
    default: return helloHandler; 
  }
}
