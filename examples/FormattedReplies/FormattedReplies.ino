/*Commander example - single object, multi layer command structure
 * Demonstrates how to use the reply pre and post fix for formatting
 * This extendes the 'quickset' example.
 */
#include <Commander.h>
Commander cmd;
//Variables we can set or get
int myInt1 = 0;
int myInt2 = 0;
float myFloat1 = 0.0;
float myFloat2 = 0.0;
//SETUP ---------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initialiseCommander();
  
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  //Call the update functions using the activeCommander pointer
  cmd.update();
}
