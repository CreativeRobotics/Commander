//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
//All the strings are const, so the are stored in flash
//The two different commands we are using
const char* normalCommand = "hello";
const char* normalHelp = "say hello";

//Extended help strings, one in French and one in English
const char* extendedHelp = "Prints out a hello message in English.\nThe 'swop' command will toggle the function pointer between two handlers, one for French and one for English, every time it is used.";
const char* otherExtendedHelp = "Imprime un message hello dans Français.\nLa commande 'swop' bascule le pointeur fonction entre deux gestionnaires, l'un pour Français et l'autre pour l'anglais, chaque fois qu'il est utilisé.";

//Normal help strings, one French and one English
const char* otherCommand = "bonjour";
const char* otherHelp = "saluer";

commandList_t masterCommands[] = {
  {normalCommand, helloHandler,     normalHelp},
  {"swap",        swapHandler,      "swap the command strings and function handler"},
};

//Extended help text. This is an array of pointers to strings so we can assign one of the char arrays from above, and use the swop function to change the text.
//(There needs to me the same number of elements in this array as in the command list.)
const char * extraHelp[] = {
  extendedHelp,
  "'swap'\tSwap between two different function handlers for the first command.\nWhen this is called the function handler, command text and help text for the first command in the list will be swapped.",
};
/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/


//Initialisation function that avoids having to forward declare the command array and a size variable.
void initialiseCommander(){
  cmd.begin(&Serial, masterCommands, sizeof(masterCommands))
                                                            .commandPrompt(ON)
                                                            .echo(true)
                                                            .errorMessages(ON)
                                                            .autoChain(OFF)
                                                            .setUserString(deviceInfo)
                                                            .setExtraHelp(extraHelp);
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
bool bonjourHandler(Commander &Cmdr){
  Cmdr.print("Bonjour! c'est");
  Cmdr.println(Cmdr.commanderName);
  Cmdr.print("C'est mon tampon de données: ");
  Cmdr.print(Cmdr.bufferString);
  //Cmdr.printDiagnostics();
  return 0;
}

bool swapHandler(Commander &Cmdr){
  Cmdr.println("Swapping command handler");
  if(swapstate){
    masterCommands[0].handler = helloHandler;
    masterCommands[0].commandString = normalCommand;
    masterCommands[0].manualString = normalHelp;
    extraHelp[0] = extendedHelp;
  }else{
    masterCommands[0].handler = bonjourHandler;
    masterCommands[0].commandString = otherCommand;
    masterCommands[0].manualString = otherHelp;
    extraHelp[0] = otherExtendedHelp;
  }
  //ESSENTIAL: You must call this if the command string text is changed otherwide Commander might not be able to recognise the new command
  Cmdr.reloadCommands();
  swapstate = !swapstate;
  return 0;
}
