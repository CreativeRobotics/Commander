//This example demonstrated a commander object attached to a Bluetooth Serial Port.
//Use a bluetooth terminal on a smartphone or laptop to connect and enter commands.
//Based on the ESP32 BluetoothSerial example By Evandro Copercini - 2018
//

#include "BluetoothSerial.h"
#include <Commander.h>

extern const uint16_t numOfMasterCmds; //This is a forward declaration so the compiler knows we are going to declare this variable properly later
extern const commandList_t masterCommands[];
Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  
  cmd.begin(&SerialBT, masterCommands, numOfMasterCmds);
  cmd.enablePrompt(); //enable the command prompt
  //cmd.echo(true);     //Echo incoming characters to the output port
  cmd.enableErrors(); //error messages are enabled - it will tell us if we issue any unrecognised commands
  //while(!Serial){;}
  //Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

void loop() {
  cmd.update();
  /*if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }*/
  delay(20);
}
