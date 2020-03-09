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
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while(!Serial){;}
  initialiseCommander();
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  cmd.update();
}
