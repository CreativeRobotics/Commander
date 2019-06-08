/*
 * This is an array containing all the commands you want to use
 * Each element in the array needs a command text string,
 * the name of the function that will be called,
 * and a helper string that can be printed out when you ask for help
 * To add your own command, copy and paste a line and edit it to include the command text and function name,
 * then create the function you want to be called
 */
const commandList_t myCommands[] = {
  {"hello", query, "Query"},
  {"help2", help2, "Get Help2"},
  {"say hello", sayhi, "Get Help2"},
};
 /*
  * This needs to be passed to the commander object so it knows how big the array of commands is, but this happens earlier in setup().
  * This has already been forward declared before setup() so the compiler knows it exists
  */
const uint16_t numOfCmds = sizeof(myCommands);

/*These are all the command handler functions, one for each command in the array above
 * Each function name must match a function name in the array.
 * Each function must return bool
 * Each function must accept the argument Commander &Cmdr
 * Cmdr is a reference to the Commander object that called the function
 * Using Cmdr.println() will print data back to the serial port attached to the Commander object that called the function
 * The command list can be used by multiple Commander objects . . .
 * if Cmdr is attached to Serial2 then Cmdr.println("Blah") will send "Blah" to Serial2
 * 
 * Function template:
bool myFunc(Commander &Cmdr){
  return 0;
}
 */

bool sayhi(Commander &Cmdr){
  Cmdr.println("Hello there!");
  //printCommandList(sBuff);
  //printDeviceState(sBuff);
  return 0;
}
bool query(Commander &Cmdr){
  Cmdr.println("Blah");
  //printCommandList(sBuff);
  //printDeviceState(sBuff);
  return 0;
}

bool help2(Commander &Cmdr){
  Cmdr.println("Blah2");
  return 0;
}
