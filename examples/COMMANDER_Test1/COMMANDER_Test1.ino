
#include <Commander.h>

Commander cmd;

/*
 * The commander needs to be given a list of commands, and told how long the list is
 * If we do this in setup, but have the list of commands later then the compiler can't work out how big the list is
 * because we can only compute its size AFTER it is declared
 * To get around this we have a variable called numOfCmds, but we forward declare it here, so the compiler knows it exists, and will be given a value further on in the sketch
 */
extern const uint16_t numOfCmds; //This is a forward declaration so the compiler knows we are going to declare this variable properly later

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  cmd.begin(&Serial);
  cmd.attachCommands(myCommands, numOfCmds);
  //cmd.echoTerminal = true;
  while(!Serial){;}
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  cmd.update();
  delay(1);
  
  //Serial.println("Updated");
}
