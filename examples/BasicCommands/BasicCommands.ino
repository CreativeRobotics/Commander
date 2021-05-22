/*Commander example - basic
 * Demonstrating commands to get and set an int and a float
 */
#include <Commander.h>
Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;

String deviceInfo = "#\tCommander basic commands example\n#\thttps://github.com/CreativeRobotics/Commander";
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  initialiseCommander();
  while(!Serial){;}
  cmd.printUserString();
  cmd.println();
  Serial.println("Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  cmd.update();
}
