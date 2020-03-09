/*
 * Commander file read example:
 * Open an SD file using SDFat, attach it to a Commander object and let it read the contents
 * Create a second SD file and attach to the outgoing port so replies are logged to a file
 * Attach the Serial port to the Auxiluary port so replies are copied to the Serial port.
 * When the file read is complete, close the file and attach Commander to the Serial port 
 * so that commands can be sent from there, and attach the log file to the Aux port so responses are still logged
 * A 'stop log' command can be used to close this log file.
 * A 'print log' command can be used to print out the file to the serial port
 * The print log command will close the log file before printing.
 */
#include <SdFat.h>
#include <sdios.h>
#include <Commander.h>
#include <SPI.h>
#define ENABLE_EXTENDED_TRANSFER_CLASS true
SdFat SD;
File myFile;
File myLogFile;
Commander cmd;

//Some variables that can be read from and written to
int myInt1 = 0;
int myInt2 = 0;
float myFloat1 = 0.0;
float myFloat2 = 0.0;

bool SDOK = false;
bool fileOpen = false;
bool readOK = false;
const int cardSelect = 4;

void setup() {
  Serial.begin(115200);
  while(!Serial){} //Wait for the serial port to open before running the example
  Serial.println("Commander File Read Example");

  //See if an SD card is inserted
  if(!SD.begin(cardSelect, SD_SCK_MHZ(50))) {
    Serial.println("SDCard Init Error");
  }else{
    SDOK = true;
    Serial.println("SDCard Started");
  }
  Serial.println("Starting Commander ... ");
  initialiseCommander();

  //Check that a file called "commands.txt" exists
  if(SDOK && SD.exists("commands.txt")){
    Serial.println("Opening File");
    myFile = SD.open("commands.txt" , O_RDONLY);
    fileOpen = true;
  }else if(SDOK){
    Serial.println("No File Found");
  }
  //Create the log file
  if(SDOK){
    //Delete any existing log file for convenience
    if(SD.exists("log.txt")){
      Serial.println("deleting old log file");
      SD.remove("log.txt");
    }
    myLogFile = SD.open("log.txt", O_WRONLY | O_CREAT);
  }
}

void loop() {
  //If the file was opened, read it and finsh
  if(fileOpen){
    readFile();
    myFile.close();
    fileOpen = false;
    Serial.println("File read finished");
    //now attach the Commander object to the serial port ...
    cmd.attachInputPort(&Serial);
    cmd.attachOutputPort(&Serial);
    cmd.attachAltPort(&myLogFile); //attach the log file to the alt port
    readOK = true;
    //Enable command prompts and echo characters
    cmd.commandPrompt(ON);
    cmd.echo(true);
    //Now print out te prompt with a message
    cmd.println("----------------------------------------------------------------");
    cmd.println("File logging is still active. You have control - Enter a command");
    cmd.printCommandPrompt();
  }else if(readOK){
    //If the file was handled then the commander is now attached to the serial port and can accept commands from there
    cmd.update();
  }
}

void readFile(){
  //Call update for as long as there are still characters left to read
  //cmd will return 1 when it handles a command but there is more in the buffer and 0 when it reaches the end of the file
  while(cmd.update()){;}
  //If there is no newline at the end of the file then any command on the last line won't be handled
  //Add a newline to the buffer and handle any commands found there
  cmd.endLine();
}
