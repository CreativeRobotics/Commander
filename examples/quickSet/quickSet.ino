/*Commander example - single object, multi layer command structure
 * Demonstrates how to use quick set and quick get commands
 * Quick set and get allow multiple commands in a single command handler specifically for setting or getting variables
 * For example if you have six integer values in your sketch and you want to use commands to read and write to them
 * you can use the quick set commands. This reduces the ammount of code you have to type.
 * Quick set and get has a special help file method as well.
 * 
 * Quick set and get commands can be chained so you can set and get all the variables in a group with one command line
 * In this example there are four variables, myInt1, myInt2, myFloat1 and myFloat2
 * You can set myInt1 to the value 23 by sending 'set int1 23'
 * You can read back the value by sending 'get int1'
 * You can get a list of all the quick set or get commands by typing 'set help' or 'get help'
 * 
 * You can set all four variables at once by sending 'set int1 23 int2 34 float1 56.7 float2 89.0'
 * You can read back the ones you want with a single line: 'get int2 float2 int1'
 * When reading back multiple variables Remember: The order with which you request the variables will NOT be the order in which they are sent. This is determined by the get command handler
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
