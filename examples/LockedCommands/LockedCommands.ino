/*Commander example - Using locks and passwords
 * Commander can be locked using a soft of a hard lock.
 * When soft locked commander will respond to internal commands only
 * When hard locked Commander will only respond to the unlock command
 * If a password is not set then 'U' will unlock
 * If a password is set then 'U mypassword' will unlock it
 * The password is not stored inside Commander and must be in the users sketch
 * A 'set password' command can be used to change the password when Commander is unlocked - the password is stored in RAM so it will not be retained after power off.
 */
#include <Commander.h>

extern const uint16_t numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const commandList_t masterCommands[];
Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;
String myPassphrase = "default";
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  cmd.begin(&Serial, masterCommands, numOfMasterCmds);
  cmd.commandPrompt(ON); //enable the command prompt
  cmd.echo(true);     //Echo incoming characters to theoutput port
  cmd.errorMessages(ON); //error messages are enabled - it will tell us if we issue any unrecognised commands
  cmd.setPassPhrase(myPassphrase);
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  
  Serial.println("Type 'X' to soft lock commander");
  Serial.println("Type 'U default' to unlock commander");

  cmd.print("Password is ");
  cmd.printPassPhrase();
  cmd.println();
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  cmd.update();
}
