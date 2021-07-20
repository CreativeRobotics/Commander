/*Commander example - Create a custom menu from an SD card
 * This example shows how to populate a command list and then load the list into Commander to use it.
 * The command list length is defined at compile time, so there is no dynamic memory allocation, but you don't need to use the whole list.
 * The sketch starts with a command list of two items, add and finish. Add will add a command (specified by a number) from the list of 24 possible commands)
 * Finish will complete the process and load the new command list.
 * The sketch will first try to open an SD card called menu.txt. If it opens the file then it reads it and any commands in the file are executed.
 * Once it is read, or if no file is found, control switches over the the serial port.
 * 
 * This demonstrates how to use a configuration file on the SD card to create a bespoke list of menu items that are available over the serial port when the device boots up.
 * To change the menu items, just edit the SD card file.
 * 
 * This could be used to run a 'boot script' using a command list when booting the device, which then creates a menu of commands that depend on features that are detected during the boot process.
 */
#include <SPI.h>
#include <SD.h> 
#include <Commander.h>
File myFile;
Commander cmd;

bool SDOK = false;
bool fileOpen = false;
const int cardSelect = 4;

int commandCount = 0;

//User string - this can be anything you want, and is printed when the help and ? commands are used
//Its a good idea to put the # symbol in front of each line so that if the response to these commands is fet to another commander, they will be interpreted as comments.
String deviceInfo = "#\t(Start of user string)\n#\tCustom menu at boot from SD card\n#\tDevice firmware version x.x.x revision x\n#\thttps://github.com/CreativeRobotics/Commander\n#\t(End of user string)";
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while(!Serial){;}
  delay(100);
  //See if an SD card is inserted
  if(!SD.begin(cardSelect)) {
    Serial.println("SDCard Init Error");
  }else{
    SDOK = true;
    Serial.println("SDCard Started");
  }
  Serial.println("Starting Commander ... ");


  initialiseCommander();
  cmd.printUserString();

  if(SDOK && SD.exists("menu.txt")){
    Serial.println("Opening File");
    myFile = SD.open("commands.txt" , FILE_READ);
    fileOpen = true;
  }else if(SDOK){
    Serial.println("No File Found");
  }

  
  cmd.println();
  Serial.println("Type 'help' to get help");


  cmd.printCommandPrompt();
}

void loop() {
  //If the file was opened, read it
  if(fileOpen){
    cmd.attachInputPort(&myFile);
    Serial.println("---------------------------------------");
    readFile();
    myFile.close();
    fileOpen = false;
    Serial.println("---------------------------------------");
    Serial.println("File read finished");
    //Now redirect commander to the serial port so we can type commands in there:
    cmd.attachInputPort(&Serial);
    //Enable command prompts and echo characters
    cmd.commandPrompt(ON);
    cmd.echo(true);
    //Now print out the prompt with a message
    cmd.println("You have control - Enter a command");
    cmd.printCommandPrompt();
  }
  cmd.update();
  delay(1);
}

void readFile(){
  //Call update for as long as there are still characters left to read
  //cmd will return 1 when it handles a command but there is more in the buffer and 0 when it reaches the end of the file
  while(cmd.update()){delay(10);}
  //If there is no newline at the end of the file then any command on the last line won't be handled
  //Add a newline to the buffer and handle any commands found there
  cmd.endLine();
}
