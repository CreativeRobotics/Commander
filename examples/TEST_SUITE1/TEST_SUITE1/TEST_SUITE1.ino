/*Commander example - Test Suite
 */
#include <Commander.h>
Commander cmd;
//Variables we can set or get
int myInt = 0;
int myInt1, myInt2;
float myFloat1, myFloat2;
float myFloat = 0.0;
String myString1 = "";
String myString2 = "";
//User string - this can be anything you want, and is printed when the help and ? commands are used
//Its a good idea to put the # symbol in front of each line so that if the response to these commands is fet to another commander, they will be interpreted as comments.
String deviceInfo = "#\t(Start of user string)\n#\tCommander test suite\n#\tDevice firmware version 1.0.0 revision x\n#\thttps://github.com/CreativeRobotics/Commander\n#\t(End of user string)";
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while(!Serial){;}
  initialiseCommander();
  cmd.printUserString();
  cmd.println();
  Serial.println("Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  cmd.update();
}
