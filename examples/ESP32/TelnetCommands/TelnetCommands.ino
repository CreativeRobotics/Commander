//This example demonstrated a commander object attached to a Wifi Telnet server
//Use a bluetooth terminal on a smartphone or laptop to connect and enter commands.


#include <Commander.h>

Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

//how many clients should be able to telnet to this ESP32
#define MAX_SRV_CLIENTS 1
const char* ssid = "**********";
const char* password = "**********";

WiFiServer server(23);
WiFiClient serverClient;

void setup() {
  
  Serial.begin(115200);
  Serial.println("\nConnecting");
  
  wifiMulti.addAP(ssid, password);
  
  Serial.println("Connecting Wifi ");
  for (int loops = 10; loops > 0; loops--) {
    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.print("WiFi connected ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    else {
      Serial.println(loops);
      delay(1000);
    }
  }
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    delay(1000);
    ESP.restart();
  }
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
  
  initialiseCommander();
  cmd.printCommandPrompt();
}

void loop() {

  if (wifiMulti.run() == WL_CONNECTED) {
    //check if there are any new clients
    if (server.hasClient()){
      //find free/disconnected spot
      if (!serverClient || !serverClient.connected()){
        if(serverClient) serverClient.stop();
        serverClient = server.available();
        if (!serverClient) Serial.println("available broken");
        Serial.print("New client: ");
        Serial.println(serverClient.remoteIP());
      }
    }
    //check clients for data

    if (serverClient && serverClient.connected()){
      if(serverClient.available()){
        //get data from the telnet client and push it to the UART
        
        cmd.update();
        //while(serverClient.available()) Serial2.write(serverClient.read());
      }
    } else {
      if (serverClient) {
        serverClient.stop();
      }
    }

  } else {
    Serial.println("WiFi not connected!");
    if (serverClient) serverClient.stop();
    delay(1000);
  }
}
