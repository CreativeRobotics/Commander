/*
 * Commander template example
 * Use this file as a starter for your own projects
 */
#include <Commander.h>
Commander cmd;

//Command handlers
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool goodbyeHandler(Commander &Cmdr){
  Cmdr.print("Goodbye from ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

//Now the command list must be defined
const commandList_t commands[] = {
  {"hello",       helloHandler,     "Say hello"},
  {"set",         goodbyeHandler,   "Say goodbye"},
};

//Now the sketch setup, loop and any other functions
void setup() {
  Serial.begin(115200);
  while(!Serial){;}                               //Wait for the serial port to open (if using USB)
  cmd.begin(&Serial, commands, sizeof(commands)); //start Commander on Serial
  cmd.commandPrompt(ON);                          //enable the command prompt
  cmd.echo(true);                                 //Echo incoming characters to theoutput port
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

void loop() {
  // put your main code here, to run repeatedly:
  cmd.update();
}
