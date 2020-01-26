/*Commander example - single object, multi layer command structure
 * Demonstrates how to use the reply pre and post fix for formatting
 * This extendes the 'quickset' example.
 */
 
#include <Commander.h>
extern const uint16_t numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const commandList_t masterCommands[]; //forward declare the master command list
Commander cmd;
//Variables we can set or get
int myInt1 = 0;
int myInt2 = 0;
float myFloat1 = 0.0;
float myFloat2 = 0.0;
bool formattingOn = false;
//SETUP ---------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.begin(115200);
  cmd.begin(&Serial, masterCommands, numOfMasterCmds);
  cmd.commandPrompt(ON); //enable the command prompt
  cmd.echo(true);     //Echo incoming characters to theoutput port
  cmd.errorMessages(ON); //error messages are enabled - it will tell us if we issue any unrecognised commands
  //Error messaged do NOT work for quick set and get commands
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  //Call the update functions using the activeCommander pointer
  cmd.update();
}
