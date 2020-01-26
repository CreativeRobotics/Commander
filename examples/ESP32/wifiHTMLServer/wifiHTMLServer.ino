
//#define PRINT_LINE_DELAY delay(10);
#include <Commander.h>
#include <utilities/htmlUtilities.h>

#include <WiFi.h>

#include "pages.h"

WiFiServer server(80);

Commander cmd;
Commander cmdWiFi;
extern const commandList_t  Commands[]; //forward declare the master command list
extern const uint16_t       numOfCmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later

extern const commandList_t  WiFiCommands[]; //forward declare the master command list
extern const uint16_t       numOfWiFICmds; //This is a forward declarationso the compiler knows we are going to declare this variable properly later

String myssid   = "BTHub6-2G8M";
String password = "GRL4MN6Cw3iV";
unsigned long clientConnectionTimeout = 5000; //millis
unsigned long wifiConnectTimeout = 10000; //ten seconds


WiFiClient serverClient;
String webFormSubmit = ""; //holds data sent from a client
#define DEBUG Serial
bool DEBUGGING = true;
bool PRINT_CLIENT_DATA = true; //streams all the client info over the serial port
//----------------------------------------------------------------------------------
void setup()
{
  //SerialBlueTooth.register_callback(btCallback);
  
  Serial.begin(115200);
  //cmd.setBuffer(256);
  cmd.begin(&Serial, Commands, numOfCmds);
  cmd.commanderName = "ESP32>";
  cmd.enablePrompt();
  cmd.printCommandPrompt();
  //cmd.disableErrors();

  cmdWiFi.setBuffer(256);
  cmdWiFi.begin(&serverClient, WiFiCommands, numOfWiFICmds);
  cmdWiFi.commanderName = "ESP32>";
  cmdWiFi.setPrefix("<p>");
  cmdWiFi.setPostfix("</p>");
  cmdWiFi.setAutoFormat(false);
  cmdWiFi.attachDefaultHandler(defaultHandler);
  delay(10);
  Serial.println("ESP ONLINE");

  if(connectToWiFi(myssid, password)){
    server.begin();
  }else{
    while(1) {;}
  }
}
//----------------------------------------------------------------------------------
int value = 0;

void loop(){
  cmd.update();
  checkWiFi();
}

void sayHello(){
  serverClient.print(helloPage);
}
void tryCommander(String theCommand){
  Serial.print("Trying commander with: ");
  Serial.println(theCommand);
  
  cmdWiFi.feedString(theCommand);
}


void checkWiFi(){
  serverClient = server.available();   // listen for incoming clients
  if (serverClient) {                             // if you get a client,
    if(PRINT_CLIENT_DATA) DEBUG.println("New Client.");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client
    char lastChar = 0;
    char c;


    //bool readingClient = true;
    while (serverClient.connected()) {            // loop while the client's connected
      if (serverClient.available()) {            // if there's bytes to read from the client,
        if(c != '\r') lastChar = c;
        c = serverClient.read();             // read a byte, then

        if(PRINT_CLIENT_DATA){
          DEBUG.write(c);//Serial.write(c);                    // print it out the serial monitor
        }

        if(c == '\r' && currentLine.startsWith("GET")) {
          //Serial.println("Found Form Data");
          webFormSubmit = currentLine;
          currentLine = "";
        }
        if(c != '\n' && c != '\r') currentLine += c;
        if(c == '\n' && lastChar == '\n') break; //End of page
      }
    }
    if(webFormSubmit.indexOf("favicon.ico") > -1){
      //server.send(404, "text/plain", "Not found");
      serverClient.stop();
      Serial.println("--------------------------------------------------------");
      return;
    }
    //generate the web page
    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Content-type:text/html");
    serverClient.println();
    serverClient.println(htmlStart); //start of html page
    tryCommander(GET_CommandString(webFormSubmit, "help"));
    serverClient.println(pageFooter); //help link
    serverClient.println(htmlEnd); //end of html page

    //end the page
    serverClient.println();
    serverClient.println();
    serverClient.stop();
    Serial.println("--------------------------------------------------------");
    //clientDisconnectTime = millis() + clientConnectionTimeout;
  }
}

bool connectToWiFi(String mySsid, String myPassword){
  unsigned long counter = 0;
  if(DEBUGGING) DEBUG.println("Connecting Wifi...");

  char ss[mySsid.length()+1];
  char ps[myPassword.length()+1];
  mySsid.toCharArray(ss, mySsid.length()+1);
  myPassword.toCharArray(ps, myPassword.length()+1);
  if(DEBUGGING){ DEBUG.print('['); DEBUG.print(ss); DEBUG.println(']');}
  if(DEBUGGING){ DEBUG.print('['); DEBUG.print(ps); DEBUG.println(']');}
  WiFi.begin(ss, ps);
  bool trying = true;
  while (trying) {
      delay(1);
      //if(DEBUGGING) Serial.print(".");
      counter++;
      if(WiFi.status() == WL_CONNECTED){
        Serial.print("WiFi Connected. WiFiIP = ");
        Serial.println(WiFi.localIP());
        return true;
      }
      if(counter > wifiConnectTimeout) {
        trying = false;
        if(DEBUGGING) DEBUG.println();
        if(DEBUGGING) DEBUG.println("Unable to connect to anything");
      }
  }
  return false;
}
