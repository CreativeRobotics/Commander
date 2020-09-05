/* Prefab command example
 *  Prefabs are command sets and command handlers that have been pre defined for specific scenarios
 *  This demonstrates the FileNavigator prefab that impliments an SDFat file navigation system.
 *  The prefab is implimented as a sub command and can be invoked with the SD command.
 *  
 *  IMPORTANT:
 *  The prefab includes a file write command that lets you stream text files to a file on the SD card or open a file and sent typed text to the file.
 *  When this command is activated the command processor will stop working until the ASCII value 4 is received.
 *  This can be sent from a terminal application such as coolTerm by pressing CONTROL+D
 *  The Arduino Serial terminal does NOT allow you to send this character and so you cannot terminate the file download when using the Arduino Serial terminal.
 */
#include <Commander.h>
#include <prefabs\SDCards\PrefabFileNavigator.h>

#include <SPI.h>
SdFat SD;
Commander cmd;
extern const uint16_t       numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const commandList_t  masterCommands[]; //forward declare the master command list

//These variables are defined in PrefabFileNavigator.h so they need to be initialised here
bool SDOK = false; //indicates that the SD card is present and initialised
String cmdName = "SD:"; //String for the SD Explored prompt


//String for the top level commander prompt
String prompt = "CMD";

//SPI Chip select pin
const int cardSelect = 4;

void setup() {
  Serial.begin(115200);
  while(!Serial){yield();} //Wait for the serial port to open before running the example
  Serial.println("Commander prefab File navigator Example");

  //See if an SD card is inserted
  if(!SD.begin(cardSelect, SD_SCK_MHZ(50))) {
    Serial.println("SDCard Init Error");
  }else{
    SDOK = true;
    Serial.println("SDCard Started");
  }
  Serial.println("Starting Commander ... type help to see a command list");
  //Attach the command list and the list size variable
  //tell the SD prefab what the top layer command list is called and how large it is
  setTopLayer(masterCommands, numOfMasterCmds, prompt);
  //Attach the predefined special handler - this is required for writing files
  cmd.attachSpecialHandler(streamToFileHandler);
  cmd.begin(&Serial, &Serial,  masterCommands, numOfMasterCmds);
  //cmd.begin(&Serial, &Serial,  fileCommands, numOfFileCmds);
  cmd.commandPrompt(ON);
  cmd.commanderName = prompt;
  cmd.echo(true);
  //cmd.println("Files on card:");
  //SD.ls(cmd.getOutputPort(), LS_R);
  
  cmd.printCommandPrompt();
}

void loop() {
  cmd.update();
}
