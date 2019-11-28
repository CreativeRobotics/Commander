/*
 * Commander file navigation example
 * Demonstrates listing files, navigating and creating directories, renaming and deleting files and directories and printing out files.
 */
#include <SdFat.h>
#include <sdios.h>
#include <Commander.h>
#include <SPI.h>
SdFat SD;
extern const uint16_t       numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const commandList_t  masterCommands[]; //forward declare the master command list
Commander cmd;

bool SDOK = false;
bool fileOpen = false;
const int cardSelect = 4;
String cmdName = "CMD: ";
void setup() {
  Serial.begin(115200);
  while(!Serial){} //Wait for the serial port to open before running the example
  Serial.println("Commander File navigator Example");

  //See if an SD card is inserted
  if(!SD.begin(cardSelect, SD_SCK_MHZ(50))) {
    Serial.println("SDCard Init Error");
  }else{
    SDOK = true;
    Serial.println("SDCard Started");
  }
  Serial.println("Starting Commander ... type help to see a command list");
  //Start Commander and attach the incoming port to the File stream
  //Attach the outgoing port to Serial
  //Attach the command list and the list size variable
  cmd.begin(&Serial, &Serial,  masterCommands, numOfMasterCmds);
  cmd.enablePrompt();
  cmd.commanderName = cmdName + "/";
  cmd.echo(true);
  //cmd.println("Files on card:");
  //SD.ls(cmd.getOutputPort(), LS_R);
  
  cmd.printCommandPrompt();
}

void loop() {
  cmd.update();
}
