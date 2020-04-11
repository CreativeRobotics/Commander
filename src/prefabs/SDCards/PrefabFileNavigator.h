//Commander prefab: SDFat file navigator
#ifndef PrefabFileNavigator_h
#define PrefabFileNavigator_h

#include <Arduino.h>
#include <string.h>
#include <SdFat.h>
#include <sdios.h>

extern bool SDOK;
extern SdFat SD;

File file1, file2;
bool fileOpen = false; //indicates if a file is open
extern String cmdName;

const commandList_t* topLayerCommands;
uint16_t topLayerCommandSize = 0;
String topPrompt = "";
 /*
  * This needs to be passed to the commander object so it knows how big the array of commands is, but this happens earlier in setup().
  * This has already been forward declared before setup() so the compiler knows it exists
  */
/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/

//some utility functions
//close any open files
void closeFiles(){
	if(file1.isOpen()) file1.close();
	if(file2.isOpen()) file2.close();
}
//-----------------------------------------------------------------------------------------------

void setTopLayer(const commandList_t* topCommands, uint16_t sizes, String prompt){
	topLayerCommands = topCommands;
	topLayerCommandSize = sizes;
	topPrompt = prompt;
}
//-----------------------------------------------------------------------------------------------

bool noCardError(Commander &Cmdr){
  if(!SDOK) Cmdr.println("ERR: No SD Card");
  else  Cmdr.println("Card detected");
  return 0;
}
//-----------------------------------------------------------------------------------------------

//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool makeDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.mkdir( Cmdr.getPayloadString().c_str() )){
    Cmdr.print("Created: ");
    Cmdr.println(Cmdr.getPayloadString());
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool changeDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.chdir( Cmdr.getPayloadString().c_str() )){
    Cmdr.print("In: ");
    Cmdr.println(Cmdr.getPayloadString());
    if(Cmdr.getPayloadString() == "/" ) Cmdr.commanderName = cmdName + Cmdr.getPayloadString();
    else Cmdr.commanderName = cmdName + " /" + Cmdr.getPayloadString();
  }else Cmdr.println("Error - no such directory");
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool printDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);

  SD.ls(Cmdr.getOutputPort(), LS_R);
  return 0;
}
//-----------------------------------------------------------------------------------------------

bool removeFile(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
	closeFiles();
  if(SD.remove( Cmdr.getPayloadString().c_str())){
    Cmdr.print("Removed: ");
    Cmdr.println(Cmdr.getPayloadString());
  }else{
    Cmdr.println("operation failed");
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool removeDirectory(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
  if(SD.rmdir( Cmdr.getPayloadString().c_str())){
    Cmdr.print("Removed: ");
    Cmdr.println(Cmdr.getPayloadString());
  }else{
    Cmdr.println("operation failed");
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool renameFile(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
	closeFiles();
  String pld = Cmdr.getPayloadString(); //get the payload without any newline
  int idx = pld.indexOf(" "); //find the first space
  if(idx == -1){
    Cmdr.println("Error, unable to handle command");
    return 0;
  }
  String sub1 = pld.substring(0, idx); //get the first word.
  String sub2 = pld.substring(idx+1, pld.length());
  Cmdr.print("Changing ");
  Cmdr.print(sub1);
  Cmdr.print(" to ");
  Cmdr.println(sub2);
  if(SD.rename(sub1.c_str(), sub2.c_str())){
    Cmdr.println("File or directory has been renamed");
  }else   Cmdr.println("Error: Operation failed");
  return 0;
}
//-----------------------------------------------------------------------------------------------

bool readFileContents(Commander &Cmdr){
  if(!SDOK)  return noCardError(Cmdr);
	closeFiles();
  File tmpFile = SD.open(Cmdr.getPayloadString().c_str(), O_RDONLY);
	if(!tmpFile.isOpen()){
		Cmdr.println("Unable to open file");
		return 0;
	}
	Cmdr.print("Reading from file ");
	Cmdr.println(Cmdr.getPayloadString());
  while(tmpFile.available()){
    Cmdr.write(tmpFile.read());
  }
  Cmdr.write('\n');
  tmpFile.close();
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool writeToFileHandler(Commander &Cmdr){
	/* Open a file and write data to it.
		 Use the payload as the filename
		 Set Commander to Stream mode and wait for contents to be downloaded
	*/
	closeFiles();
	if(Cmdr.hasPayload()){
		//create file
		file1 = SD.open(Cmdr.getPayloadString().c_str(), O_WRITE | O_CREAT);
		Cmdr.print(F("Created file: "));
		Cmdr.println(Cmdr.getPayloadString());
		Cmdr.startStreaming();
	}else Cmdr.print(F("Error - Filename required"));
  return 0;
}
//-----------------------------------------------------------------------------------------------

bool exitHandler(Commander &Cmdr){
	if(topLayerCommands == NULL || topLayerCommandSize == 0){
		Cmdr.println("Exit not functional");
		return 0;
	}
  //close any open files ...
	Cmdr.println("Closing SD Navigator");
	Cmdr.transferBack(topLayerCommands, topLayerCommandSize, topPrompt);
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool streamToFileHandler(Commander &Cmdr){
	//special handler for file streams
	if(!file1.isOpen()){
		//file is not open so tell the streamer to stop
		Cmdr.println("Error: Streaming to closed file");
		Cmdr.stopStreaming();
		return 0;
	}
	if(Cmdr.bufferString.length() > 0){
		//write to the file and flush the buffer.
		Cmdr.print("WRITING:");
		Cmdr.println(Cmdr.bufferString);
		file1.print(Cmdr.bufferString);
		file1.flush();
	}
	if(Cmdr.isStreaming() == false && file1.isOpen()){
		//File is open but streaming has stopped - indicated end of file and time to tidy up.
		Cmdr.println("Flushing and closing file");
		file1.flush();
		file1.close();

	}
	return 0;
}
//-----------------------------------------------------------------------------------------------


//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t fileCommands[] = {
  {"mkdir", makeDirectory, "make a sub directory"},
  {"chdir", changeDirectory, "Change directory - use / for root"},
  {"ls", printDirectory, "Print directory"},
  {"remove", removeFile, "Delete a file"},
  {"removedir", removeDirectory, "Delete an empty directory"},
  {"rename", renameFile, "rename a file or sub directory"},
  {"read", readFileContents, "Read file and send to terminal"},
  {"write", writeToFileHandler, "Create file and open for writing - Send ASCII code 0x04 to terminate"},
  {"status", noCardError, "check card status"},
	{"exit", exitHandler, "Exit sub command"}
   
};

//initialise the numOfMasterCmds variable after creating the masterCommands[] array - numOfMasterCmds now indicates the length of the array
const uint16_t numOfFileCommands = sizeof(fileCommands);
#endif //PrefabFileNavigator_h