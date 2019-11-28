/*
 * Commander file read example:
 * Open an SD file using the SD library, attach it to a Commander object and let it read the contents
 * Replies are redirected to the Serial port.
 * By default this is written for the Adafruit Feather M0 Adalogger
 */
//#include <SdFat.h>
//#include <sdios.h>
#include <SPI.h>
#include <SD.h>

#include <Commander.h>

//SdFat SD;
File myFile;

extern const uint16_t       numOfMasterCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later
extern const commandList_t  masterCommands[]; //forward declare the master command list as well
Commander cmd;

//Some variables that can be read from and written to
int myInt1 = 0;
int myInt2 = 0;
float myFloat1 = 0.0;
float myFloat2 = 0.0;

bool SDOK = false;
bool fileOpen = false;
const int cardSelect = 4;

void setup() {
  Serial.begin(115200);
  while(!Serial){} //Wait for the serial port to open before running the example
  Serial.println("Commander File Read Example");

  //See if an SD card is inserted
  if(!SD.begin(cardSelect)) {
    Serial.println("SDCard Init Error");
  }else{
    SDOK = true;
    Serial.println("SDCard Started");
  }
  Serial.println("Starting Commander ... ");
  //Start Commander and attach the file stream object, and the serial port for replies
  //begin(Stream object for input, stream object for output, command array, size of command array)
  //Attach the outgoing port to Serial
  //Attach the command list and the list size variable
  cmd.begin(&myFile, &Serial,  masterCommands, numOfMasterCmds);
  //enable printing of comment lines
  cmd.printComments(true);
  //Check that a file called "commands.txt" exists
  if(SDOK && SD.exists("commands.txt")){
    Serial.println("Opening File");
    myFile = SD.open("commands.txt" , FILE_READ);
    fileOpen = true;
  }else if(SDOK){
    Serial.println("No File Found");
  }
}

void loop() {
  //If the file was opened, read it and finsh
  if(fileOpen){
    readFile();
    myFile.close();
    fileOpen = false;
    Serial.println("File read finished");
    //Now redirect commander to the serial port so we can type commands in there:
    cmd.attachInputPort(&Serial);
    //Enable command prompts and echo characters
    cmd.enablePrompt();
    cmd.echo(true);
    //Now print out te prompt with a message
    cmd.println("You have control - Enter a command");
    cmd.printCommandPrompt();
  }
  cmd.update();
  delay(1);
}

void readFile(){
  //Call update for as long as there are still characters left to read
  //cmd will return 1 when it handles a command but there is more in the buffer and 0 when it reaches the end of the file
  while(cmd.update()){;}
  //If there is no newline at the end of the file then any command on the last line won't be handled
  //Add a newline to the buffer and handle any commands found there
  cmd.endLine();
}
