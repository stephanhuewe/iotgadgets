// Credits:
// https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/examples/SSD1306ClockDemo/SSD1306ClockDemo.ino

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "OLEDDisplayUi.h"
#include "images.h"
#include <ArduinoJson.h>

// Wifi-Settings, please change
const char* ssid = "MyWLAN";            
const char* password = "secret";        

// IOTA Settings
char server[] = "my.iotaserver.de"; // Please use some other IOTA server

int status = WL_IDLE_STATUS;     
WiFiClient client;

String balance = "";
float addressbalance = 0;

//Adresse zum Testen:
//OHKQZAWRCGOHPHGEJFWAXL9JAWVUUMBCNMALLSTWLTKHOLFALWXYCCJRRHFRLGEZIWDTJXRUMNNQABEVX

String ip = "";
String address = "";
String addresscache = "";

bool thereWasAClientCall = false;

ESP8266WebServer webserver(80);    // Set Webserver Port (e.g. 80)
 
// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D1, D2);
OLEDDisplayUi ui ( &display );

int screenW = 128;
int screenH = 64;
int screenCenterX = screenW/2;
int screenCenterY = ((screenH-16)/2)+16; 
int clockRadius = 23;

// utility function for digital clock display: prints leading 0
String twoDigits(int digits){
  if(digits < 10) {
    String i = '0'+String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}

// Frame for showing the balance introduction text
void balanceFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(screenCenterX + x , screenCenterY + y, "Your balance:" );
}

// Frame for showing the balance
void iotaFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(screenCenterX + x , screenCenterY + y, String(addressbalance) + " MIOTA" );
}

// Frame for showing the IP
void seedFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
 
  display->drawString(screenCenterX + x , screenCenterY + y, ip );
}

// Frame for showing the IP introduction text
void iPFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(screenCenterX + x , screenCenterY + y, "IOTABOX IP:" );
}

// This is called, whenever "http://<ip address>/" was opened by a client
void handleRoot()           
{
  int val = 0;
  // The HTML-Code was compressed into one single line
  String html1 = "<!DOCTYPE html>\r\n<html>\r\n\<head></head>\r\n<body><img src=\"https://preview.ibb.co/hqwkLn/iota.jpg\" border=\"0\"><br>\r\n<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">\r\n\<title>IOTA Money Box</title>\r\n\<form action='http://" + ip + "/submit' method='POST'><input type=\"text\" name=\"adr\" size=\"150\"><input type=\"submit\" value=\"Add Address\"></form>\r\n<br><br>Current addresses: " + addresscache + "<br><br>Your total is: " + addressbalance + " MIOTA</body>\r\n</html>";
  webserver.send(200, "text/html", html1);
}

// This is called, when the form was submitted
void handleSubmit()
{
  Serial.println("Submit begin");

  if( ! webserver.hasArg("adr") || webserver.arg("adr") == NULL) { // if the address ("adr") data is missing
    webserver.send(400, "text/plain", "400: Invalid Request");         
    return;
  }
  else
  {
    Serial.println(webserver.arg("adr"));
    address = webserver.arg("adr");
    // Address is 90 bytes (81 Address + 9 Checksum) - API takes only address without checksum. So we take the substring instead...
    address = address.substring(0, 81);
    addresscache = addresscache + "<br>" + address;
    webserver.send(200, "text/html", "Address " + address + " added, check your display.<br>Click <a href='http://" + ip + "'> to go back.");
  
    Serial.println();
    Serial.println("Starting connection to server...");
    Serial.println("Sending address: " + address);
  
    // if you get a connection, report back via serial
    if (client.connect(server, 14265)) {
      Serial.println("Connected to server");
      // Make a HTTP request to external
      String content = "{\"command\": \"getBalances\", \"addresses\":[\"" + address + "\"], \"threshold\":100}";
      client.println("POST / HTTP/1.1");
      client.println("User-Agent: Huewe");
      client.println("Accept: *//*");
      client.println("Content-Type: application/json");    
      client.println("X-IOTA-API-Version: 1");
      client.println("Host: my.iotaserver.de:14265");
      client.println("Content-Length: 142"); // + content.length()); Calc does not work
      client.println();
      client.println(content);
    } 
  
    Serial.println("Content sent!");
    thereWasAClientCall = true;
  }
  Serial.println("Submit end");
}

// this array keeps all frames
FrameCallback frames[] = { iPFrame, seedFrame, balanceFrame, iotaFrame };

// how many frames are there?
int frameCount = 4;

void setup() {
  Serial.begin(9600);
  Serial.println();
  randomSeed(analogRead(0));

  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();

  unsigned long secsSinceStart = millis();
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSinceStart - seventyYears * SECS_PER_HOUR;
  setTime(epoch);


 WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
Serial.println("WiFi connected");

// Get Current IP
ip = String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
Serial.println("Current IP: " + ip);

// Define webserver behaviour
webserver.on("/", handleRoot);
webserver.on("/submit", handleSubmit);
webserver.begin();
Serial.println("Webserver running");

  
}

// helper function for finding text
int find_text(String needle, String haystack) {
  int foundpos = -1;
  for (int i = 0; i <= haystack.length() - needle.length(); i++) {
    if (haystack.substring(i,needle.length()+i) == needle) {
      foundpos = i;
    }
  }
  return foundpos;
}

// The main loop
void loop() {
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    delay(remainingTimeBudget);

  }

  // Manage webserver
  webserver.handleClient();
  
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
      
      char c = client.read();  
      Serial.write(c);
      balance += c; 
  }
  
  if (balance != "")
  {
    Serial.println("Content was: " + balance);

    // Here we try to parse the content
    const size_t bufferSize = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 160;
    DynamicJsonBuffer jsonBuffer(bufferSize);
  
    int firstbracket = balance.indexOf('{');
    balance = balance.substring(firstbracket);
    Serial.println("");
    Serial.println("Substring is: " + balance);
    
    JsonObject& root = jsonBuffer.parseObject(balance);
  
    if (!root.success()) {
      Serial.println("parseObject() failed");
    }
     
     const char* balances0 = root["balances"][0];
     Serial.println("Json is:");
     Serial.println(balances0);

     // Formatting
     balance = String(balances0);
     float value = balance.toFloat();
     value = value / 1000000;
     balance = String(value);
  
     addressbalance = addressbalance + value;
     balance = "";
     addresscache = addresscache + " - " + value + " MIOTA";
  }
  
    // if the server's disconnected, stop the client
    if (thereWasAClientCall)
    {
      if (!client.connected()) {
        Serial.println();
        Serial.println("Disconnecting from server...");
        client.stop();
      }
    }
}
