//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
//You can have up to maxItems number of commands in the command list at once
//There are a total of numberOfCommands command handlers available to use.
const int maxItems = 16;
const int numberOfCommands = 24;


const char* commandList[numberOfCommands] = {
  "cmd0",
  "cmd1",
  "cmd2",
  "cmd3",
  "cmd4",
  "cmd5",
  "cmd6",
  "cmd7",
  "cmd8",
  "cmd9",
  "cmd10",
  "cmd11",
  "cmd12",
  "cmd13",
  "cmd14",
  "cmd15",
  "cmd16",
  "cmd17",
  "cmd18",
  "cmd19",
  "cmd20",
  "cmd21",
  "cmd22",
  "cmd23",
};


const char* helpList[numberOfCommands] = {
  "help text for command cmd0",
  "help text for command cmd1",
  "help text for command cmd2",
  "help text for command cmd3",
  "help text for command cmd4",
  "help text for command cmd5",
  "help text for command cmd6",
  "help text for command cmd7",
  "help text for command cmd8",
  "help text for command cmd9",
  "help text for command cmd10",
  "help text for command cmd11",
  "help text for command cmd12",
  "help text for command cmd13",
  "help text for command cmd14",
  "help text for command cmd15",
  "help text for command cmd16",
  "help text for command cmd17",
  "help text for command cmd18",
  "help text for command cmd19",
  "help text for command cmd20",
  "help text for command cmd21",
  "help text for command cmd22",
  "help text for command cmd23",
};




cmdHandler handlers[numberOfCommands] = {
  Handler0,
  Handler1,
  Handler2,
  Handler3,
  Handler4,
  Handler5,
  Handler6,
  Handler7,
  Handler8,
  Handler9,
  Handler10,
  Handler11,
  Handler12,
  Handler13,
  Handler14,
  Handler15,
  Handler16,
  Handler17,
  Handler18,
  Handler19,
  Handler20,
  Handler21,
  Handler22,
  Handler23,
};

const commandList_t startupCommands[] = {
  {"add", addHandler, "Add a command (add [handlerNo])"},
  {"finish", finishHandler, "Finish adding commands and load the list"},
};

//uninitialised array to be filled
commandList_t myCommands[maxItems];


/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/

//Initialisation function that avoids having to forward declare the command array and a size variable.
void initialiseCommander(){
  //Initialise with inport and outport pointing to serial
  //inport will be reassigned to the file if it is found, the directed back to serial at the end of the file read
  //Any messages will be printed to Serial
  cmd.begin(&Serial, &Serial, startupCommands, sizeof(startupCommands))
                                                            .commandPrompt(ON)
                                                            .echo(true)
                                                            .errorMessages(ON)
                                                            .autoChain(OFF)
                                                            .setUserString(deviceInfo);
}

//commandString;
//  cmdHandler handler;
//  char* manualString;




bool finishHandler(Commander &Cmdr){
  Cmdr.println("Switching to custom command list");
  Cmdr.attachCommandArray(myCommands, commandCount);
  return 0;
}


bool Handler0(Commander &Cmdr){
  Cmdr.println("Called command handler number 0");
  return 0;
}
bool Handler1(Commander &Cmdr){
  Cmdr.println("Called command handler number 1");
  return 0;
}
bool Handler2(Commander &Cmdr){
  Cmdr.println("Called command handler number 2");
  return 0;
}
bool Handler3(Commander &Cmdr){
  Cmdr.println("Called command handler number 3");
  return 0;
}
bool Handler4(Commander &Cmdr){
  Cmdr.println("Called command handler number 4");
  return 0;
}
bool Handler5(Commander &Cmdr){
  Cmdr.println("Called command handler number 05");
  return 0;
}
bool Handler6(Commander &Cmdr){
  Cmdr.println("Called command handler number 6");
  return 0;
}
bool Handler7(Commander &Cmdr){
  Cmdr.println("Called command handler number 7");
  return 0;
}
bool Handler8(Commander &Cmdr){
  Cmdr.println("Called command handler number 8");
  return 0;
}
bool Handler9(Commander &Cmdr){
  Cmdr.println("Called command handler number 9");
  return 0;
}
bool Handler10(Commander &Cmdr){
  Cmdr.println("Called command handler number 10");
  return 0;
}
bool Handler11(Commander &Cmdr){
  Cmdr.println("Called command handler number 11");
  return 0;
}
bool Handler12(Commander &Cmdr){
  Cmdr.println("Called command handler number 12");
  return 0;
}
bool Handler13(Commander &Cmdr){
  Cmdr.println("Called command handler number 13");
  return 0;
}
bool Handler14(Commander &Cmdr){
  Cmdr.println("Called command handler number 14");
  return 0;
}
bool Handler15(Commander &Cmdr){
  Cmdr.println("Called command handler number 15");
  return 0;
}
bool Handler16(Commander &Cmdr){
  Cmdr.println("Called command handler number 16");
  return 0;
}
bool Handler17(Commander &Cmdr){
  Cmdr.println("Called command handler number 17");
  return 0;
}
bool Handler18(Commander &Cmdr){
  Cmdr.println("Called command handler number 18");
  return 0;
}
bool Handler19(Commander &Cmdr){
  Cmdr.println("Called command handler number 19");
  return 0;
}
bool Handler20(Commander &Cmdr){
  Cmdr.println("Called command handler number 20");
  return 0;
}
bool Handler21(Commander &Cmdr){
  Cmdr.println("Called command handler number 21");
  return 0;
}
bool Handler22(Commander &Cmdr){
  Cmdr.println("Called command handler number 22");
  return 0;
}
bool Handler23(Commander &Cmdr){
  Cmdr.println("Called command handler number 23");
  return 0;
}


bool addHandler(Commander &Cmdr){
  //There should be just a number in the payload
  //Cmdr.printDiagnostics();
  int itemID = -1;
  if(commandCount >= maxItems){
    Cmdr.println("Error, menu is full");
    return 0;
  }
  if(Cmdr.getInt(itemID)){
    if(itemID > -1 && itemID < numberOfCommands){
      myCommands[commandCount].commandString  = commandList[itemID];
      myCommands[commandCount].handler        = handlers[itemID];
      myCommands[commandCount].manualString   = helpList[itemID];
      Cmdr.print("Added the command ");
      Cmdr.print(commandList[itemID]);
      Cmdr.print(" as menu item ");
      Cmdr.println(commandCount);
      commandCount++;
    }else Cmdr.println("Error, that item is not available");
  }
  return 0;
}
