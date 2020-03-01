/*Commander example - basic
 * Demonstrating commands to get and set an int and a float
 */
#include <Commander.h>
extern const uint16_t numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const commandList_t masterCommands[];
Commander cmd;
//Variables we can set or get
int myInt = 0;
int myInt1, myInt2;
float myFloat1, myFloat2;
float myFloat = 0.0;
String myString1 = "";
String myString2 = "";
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  cmd.begin(&Serial, masterCommands, numOfMasterCmds);
  cmd.commandPrompt(ON); //enable the command prompt
  cmd.echo(true);     //Echo incoming characters to theoutput port
  cmd.errorMessages(ON); //error messages are enabled - it will tell us if we issue any unrecognised commands
  cmd.autoChain(ON);
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  cmd.update();
}
