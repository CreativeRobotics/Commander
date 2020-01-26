//connect to VM4789902 vrtqvfDxr8hq
//connect to AndroidAP hvpm2495

//bool query(serialBuffer_t &sBuff);
//bool help(serialBuffer_t &sBuff);

const commandList_t Commands[] = {
  {"set help",     setHelpState,    "help sys on or off"},
  {"set internal",     setIntCmdState,    "internal cmds on or off"},
  {"show internal",     hideIntCmd,    "internal cmds print on or off"},
  {"enable server",     startEnableServer,    "enable web server"},
  {"disable server",    disableServer,        "disable web server"},
  {"set SSID",          setSSID,              "set WiFI SSID"}, //setSSID0=
  {"set Pass",          setPassword,          "set WiFi password"}, //setWiFiPass0=
  {"connect to",        wifiConnectTo,        "connect to WiFI (SSID) (Password)"},
  {"set WiFi Timeout",  setWiFiTimeout,       "set WiFi connect timeout"}, //setWiFiPass0=
  {"hello",             helloHandler,         "say hello"}, //setWiFiPass0=
  {"webhelp",           callHelp,             "call help www style"}, //setWiFiPass0=
  {"test",              callTest,             "Show the test page"}, //setWiFiPass0=
  {"homework",          callHomework,         "Show homework page"}, //setWiFiPass0=
  {"robots.txt",        callRobots,           "Robots page"}, //setWiFiPass0=
  {"?",                 query,                "query"}, //setWiFiPass0=
};
const uint16_t numOfCmds = sizeof(Commands);// /  sizeof(ESPCommands[0]); //calculate the number of commands so we know the array bounds



const commandList_t WiFiCommands[] = {
  {"hello",       helloHandler,       "say hello"}, //setWiFiPass0=
  {"help",        callHelp,           "call help"}, //setWiFiPass0=
  {"test",        callTest,           "Show the test page"}, //setWiFiPass0=
  {"homework",    callHomework,       "Show homework page"}, //setWiFiPass0=
  {"robots.txt",  callRobots,         "Robots page"}, //setWiFiPass0=
  {"?",           query,              "query"}, //setWiFiPass0=
};
const uint16_t numOfWiFICmds = sizeof(WiFiCommands);// /  sizeof(ESPCommands[0]); //calculate the number of commands so we know the array bounds


//Serial port command handlers ======================================================================================================
//Default handler - for error messages



bool setHelpState(Commander &Cmdr){
  if(Cmdr.containsOn()){
    Cmdr.enableHelp();
    Cmdr.println("Help Visible");
  }else{
    Cmdr.disableHelp();
    
    Cmdr.println("Help hidden");
  }
  return 0;
}
bool setIntCmdState(Commander &Cmdr){
  if(Cmdr.containsOn()){
    Cmdr.enableInternalCommands();
    Cmdr.println("Int cmds ON");
  }else{
    Cmdr.disableInternalCommands();
    
    Cmdr.println("Int cmds OFF");
  }
  return 0;
}

bool hideIntCmd(Commander &Cmdr){
  if(Cmdr.containsOn()){
    Cmdr.enableInternalCommandPrint();
    Cmdr.println("Int cmds visible");
  }else{
    Cmdr.disableInternalCommandPrint();
    
    Cmdr.println("Int cmds hidden");
  }
  return 0;
}
bool callRobots(Commander &Cmdr){
  
  Cmdr.print(robotsPage);
  return 0;
}


bool defaultHandler(Commander &Cmdr){
  Serial.println("Default Handler called");
  Cmdr.print(cmdHeadErrorPage);
  for(uint16_t n = 0; n < Cmdr.getCommandListLength(); n++){
    Cmdr.println(Cmdr.getCommandItem(n));
  }
  Cmdr.print(cmdForm);
  Cmdr.print(cmdFootPage);
  return 0;
}
bool helloHandler(Commander &Cmdr){
  
  Cmdr.print(helloPage);
  return 0;
}
bool callHomework(Commander &Cmdr){
  
  Cmdr.print(myHomework);
  return 0;
}
bool callTest(Commander &Cmdr){
  
  Cmdr.print(testPage);
  return 0;
}
bool callHelp(Commander &Cmdr){
  
  //cmdWiFi.setAutoFormat(false);
  Cmdr.print(cmdHeadPage);
  for(uint16_t n = 0; n < Cmdr.getCommandListLength(); n++){
    Cmdr.println(Cmdr.getCommandItem(n));
  }
  //Cmdr.printCommandList();
  Cmdr.print(cmdForm);
  Cmdr.print(cmdFootPage);
  //cmdWiFi.setAutoFormat(true);
  return 0;
}
bool query(Commander &Cmdr){
  Cmdr.startFormatting();
  Cmdr.printCommanderVersion();
  return 0;
}
bool setWiFiTimeout(Commander &Cmdr){
  if( Cmdr.getInt(wifiConnectTimeout) ){
    if(wifiConnectTimeout < 10) wifiConnectTimeout = 10;
  }
  return 0;
}



bool wifiConnectTo(Commander &Cmdr){
  if(DEBUGGING) DEBUG.println("Trying to connect to ...");
  Cmdr.getString(myssid);
  Cmdr.getString(password);
  if(DEBUGGING) {DEBUG.print("SSID=["); DEBUG.print(myssid); DEBUG.println(']');}
  if(DEBUGGING) {DEBUG.print("PSWD=["); DEBUG.print(password); DEBUG.println(']');}
  return 0;
}

bool wifiDisconnect(Commander &Cmdr){
  WiFi.mode(WIFI_OFF);
  Cmdr.println("WiFi Disconnected");
}


bool setSSID(Commander &Cmdr){
  if(DEBUGGING) DEBUG.println("Setting SSID");
  if(Cmdr.hasPayload()){
    myssid = Cmdr.getPayloadString();
  }
  if(DEBUGGING){
    DEBUG.print("Set SSID to: ");
    DEBUG.println(myssid);
  }
  return 0;
}

bool setPassword(Commander &Cmdr){
  if(DEBUGGING) DEBUG.println("Setting Password");
  if(Cmdr.hasPayload()){
    password = Cmdr.getPayloadString();
  }
  if(DEBUGGING){
    Cmdr.print("Set Password to: ");
    Cmdr.println(password);
  }
  return 0;
}
bool startEnableServer(Commander &Cmdr){
  server.begin();
  return 0;
}

bool disableServer(Commander &Cmdr){
  server.end();
  return 0;
}
