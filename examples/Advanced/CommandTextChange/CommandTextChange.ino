/*Commander example - Changing command text and help text with other commands
 */
#include <Commander.h>
Commander cmd;

//User string - this can be anything you want, and is printed when the help and ? commands are used
//Its a good idea to put the # symbol in front of each line so that if the response to these commands is fet to another commander, they will be interpreted as comments.
String deviceInfo = "#\t(Start of user string)\n#\tChanging command text and help text\n#\tDevice firmware version x.x.x revision x\n#\thttps://github.com/CreativeRobotics/Commander\n#\t(End of user string)";
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);
  while(!Serial){;}
  delay(100);
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
