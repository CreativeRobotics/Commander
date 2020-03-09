/*Commander example - basic
 * Demonstrating commands to get and set an int and a float
 */
#include <Commander.h>
Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  initialiseCommander();
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  cmd.update();
}
