#include <ESP8266WiFi.h> // Include WiFi library
#include <ESP8266WebServer.h> // Include WebServer library
//#include <DS3231M.h> // Include the DS3231M RTC library
#include <FastLED.h>

#include <Wire.h>;
#include "RTClib.h"

#include "wifi.h"

//Creates an instance of the DS3231M class and server object on port 80l standard port for webbrowsers talking to web servers
//DS3231M_Class DS3231M;       

RTC_DS3231 rtc;

#define DATA_PIN    12
#define CHIPSET     WS2812
#define NUM_LEDS    369
#define COLOR_ORDER GRB
#define initialLEDs 3     //Number of LEDs in the very first strip
#define BRIGHTNESS  255 

const uint8_t kMatrixHeight = 17;
const uint8_t kMatrixStart = 0;

uint8_t gHue = 0;

int steps[]= {0, 4, 5, 6, 4, 6, 5, 4, 1, 0, -4, -5, -6, -4, -5, -5, -4};

int stepsSky[]= {0, 0, 4, 8, 10, 13, 19, 22, 25, 30, 34, 29, 23, 19, 14, 9, 5};

//Define boolean variables for LED button status
bool RegenbogenStatus = LOW;
bool SternenshauerStatus = LOW;
bool SonnenaufgangStatus = LOW;

//Define variables to store current time and alarm time; Alarm time is set to 65 initially, to hide "Alarm time" in HTML when alarm is not set
uint8_t Actualhour;
uint8_t Actualminute;
uint8_t Alarmhour = 65;
uint8_t Alarmminute = 65;

#include "gradient.h"

CRGB leds[NUM_LEDS];
CRGBPalette16 myPal = sunrise_gp;

#include "matrix.h"
#include "animations.h"

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS );
  Serial.begin(115200);
  
//  //Initialize RTC communications; Red LED blinks, if DS3231M cannot be initialized
//  while (!DS3231M.begin()) 
//    {
//      Serial.println(F("Unable to find DS3231MM. Checking again in 3s."));
//      delay(1000);
//      digitalWrite(REDpin, HIGH);
//      delay(1000);
//      digitalWrite(REDpin, LOW);
//      delay(1000);
//    }
//  //DS3231M.adjust(DateTime(2020, 4, 4, 13, 04, 00)); // Sets time of DS3231M
//  Serial.println("DS3231M initialized.");
//  digitalWrite(REDpin, LOW); // Turns red LED off, in case it stayed on in initial sequence
//  
  //Initiate WiFi in Station Mode
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  //Functions that get called when specific url is being called
  server.on("/", handle_OnConnect); 
  server.on("/regenbogen_an", handle_regenbogenan);
  server.on("/regenbogen_aus", handle_regenbogenaus);
  server.on("/sternenschauer_an", handle_sternenschaueran);
  server.on("/sternenschauer_aus", handle_sternenschaueraus);
  server.on("/sonnenaufgang_an", handle_sonnenaufgangan);
  server.on("/sonnenaufgang_aus", handle_sonnenaufgangaus);
  server.on("/alarmtime", handle_alarmtime);
  server.on("/alarmreset", handle_alarmreset);
  server.onNotFound(handle_NotFound);  //404 message when url not specified with server.on requested

  server.begin(); // Start webserver
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

  rtc.begin();                                        //Starts RTC
  //rtc.adjust(DateTime(2020, 7, 21, 17, 41, 0));     //Set time and date manually (i.e. rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)) January 21, 2014 at 3am)
}

void loop() {
  server.handleClient();
  
//  DateTime now = DS3231M.now(); // get the current time from device

  DateTime now = rtc.now();                         //Fetches current Date and Time

  //Save current hour and minute and save it to variable
  if (Actualhour != now.hour())
    {Actualhour = now.hour();}
  if (Actualminute != now.minute())
    {Actualminute = now.minute();}

  //Check if actual time is the same as alarmtime. If so, call "alarmlight"
  if(now.hour() == Alarmhour && now.minute() == Alarmminute)
  {HintermBerg();}

  //Check if boolean variable for the HTML button has changed through calling the corresponding link (in SendHTML) and turn on LEDs
  if(RegenbogenStatus)
    {LilaWolken();}
  else if(SternenshauerStatus)
    {sternenschauer();}
  else if(SonnenaufgangStatus)
    {HintermBerg();}
  else
    {FastLED.clear();}
  FastLED.show();
}

//void HintermBerg(){
//  for(int r = 0; r < 255; r++){
//    for (int y = kMatrixStart; y < kMatrixHeight; y++){
//      for (int x = 0; x < stepsSky[y]; x++){
//        float grad = float(x) / float(stepsSky[y]);
//        float k2 = (float(y) / float(kMatrixHeight)) * float(r);
//
//        //Basic version; Problem: lower rows still show red
//        //float k = float(y) / float(kMatrixHeight) * float(r);
//        
//        //Speeds up color gradient
//        //float k = sqrt(float(y) / float(kMatrixHeight)) * float(r);
//
//        //Improves color gradient, so that even bottom rows show blue
//        float v = float(y) / float(kMatrixHeight);
//        float k = pow(v, v) * float(r);  
//        
//        //float sinus = sin8(k - float(64.75));
//        float sinus = sin8(k2 - float(64.75));
//        float j = float(255) - k;
//        int grad2 = grad * j * (sinus / 255) + k;
//        leds[xySky(stepsSky[y] - x, y)] =  ColorFromPalette(myPal, grad2);
//        leds[xyBerg(x, y)] = CHSV(92, 255, 255);
//      }
//    }
//    FastLED.show();
//    delay(10);
////  for (int y = kMatrixStart; y < kMatrixHeight; y++){
////    for (int x = 0; x < currentLEDs(y); x++){
////        leds[xyBerg(x, y)] = CHSV(0, 0, gHue);
////    }
////  }
//  }
//}

//These function handle request from client (through server.on) and send HTML back to client. Variables in SendHTML string will get updated as well
//Function that called when calling root (/) of webserver
void handle_OnConnect() 
{
  server.send(200, "text/html", SendHTML()); 
}

//Functions that get called when LED buttons get pushed
void handle_regenbogenan() {
  RegenbogenStatus = HIGH;
  Serial.println("Regenbogen Status: AN");
  server.send(200, "text/html", SendHTML()); 
}

void handle_regenbogenaus() {
  RegenbogenStatus = LOW;
  Serial.println("RegenbogenStatus: AUS");
  server.send(200, "text/html", SendHTML()); 
}

void handle_sternenschaueran() {
  SternenshauerStatus = HIGH;
  Serial.println("Sternenschauer Status: AN");
  server.send(200, "text/html", SendHTML()); 
}

void handle_sternenschaueraus() {
  SternenshauerStatus = LOW;
  Serial.println("Sternenschauer Status: AUS");
  server.send(200, "text/html", SendHTML()); 
}

void handle_sonnenaufgangan() {
  SonnenaufgangStatus = HIGH;
  Serial.println("Sonnenaufgang Status: AN");
  server.send(200, "text/html", SendHTML()); 
}

void handle_sonnenaufgangaus() {
  SonnenaufgangStatus = LOW;
  Serial.println("SonnenaufgangStatus: AUS");
  server.send(200, "text/html", SendHTML()); 
}

//Function that sets alarmtime by saving set time from URL
void handle_alarmtime() {
  if (server.arg("hour")!= ""){
    Alarmhour = server.arg("hour").toInt();
  }

  if (server.arg("minute")!= ""){
    Alarmminute = server.arg("minute").toInt();
    Serial.print("Alarm time set to: ");
    Serial.print(Alarmhour);
    Serial.print(":");
    Serial.println(Alarmminute);
  }
  server.send(200, "text/html", SendHTML());
}

//Alarm variables get set to 65 to virtually set them to an out of range for actual time and reset the alarm
void handle_alarmreset() {
  Alarmhour = 65;
  Alarmminute = 65;
  server.send(200, "text/html", SendHTML());
}

//Function to handle 404 request
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

//Strings hold HTML and CSS styling
String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>\n";
    ptr +="<title>Lichtwecker</title>\n";
    ptr +="<style>html {background-color: black;font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr +="body{margin-top: 50px;color: white;font-size: 14px} h1 {color: #fff;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 20px;}\n";
    ptr +=".button-on {background-color: #6FB98F;}\n";
    ptr +=".button-on:active {background-color: #45BD79;}\n";
    ptr +=".button-off {background-color: #F18D9E;}\n";
    ptr +=".button-off:active {background-color: #EE5972;}\n";
    ptr +=".time {margin-right: 5px;margin-left: 5px;padding: 5px 10px;border: none;border-radius: 4px;background-color: #4d4d4d;color: #cecece;}\n";
    ptr +=".submit {margin-left: 5px;padding: 6px 10px;border: none;border-radius: 4px;background-color: #4d4d4d;color: #cecece;text-decoration: none}\n";
    ptr +=".form {margin: 0px auto 20px;}\n";
    ptr +="p {font-size: 14px;color: #fff;margin-bottom: 10px;}\n";
    ptr +="</style>\n";
    ptr +="</head>\n";
    ptr +="<body>\n";
    ptr +="<h1>ESP8266 Lichtwecker </h1>\n";
    ptr +="<p>Current Time: ";
    ptr +=(int)Actualhour;
    ptr +=":";
  if (Actualminute < 10) // Display leading zeros
    {ptr +="0";}
    ptr +=(int)Actualminute;
    ptr +="</p>\n";
  if (Alarmhour < 61) // Show alarm time when it is set to value within range of actual time
    {ptr +="<p>Alarm Time: ";
    ptr +=(int)Alarmhour;
    ptr +=":";}
  if(Alarmminute < 10) // Display leading zeros
    {ptr +="0";}
  if(Alarmminute < 61) // Show alarm time when it is set to value within range of actual time
    {ptr +=(int)Alarmminute;
    ptr +="</p>\n";}

  // HTML dropdown form to set the alarm time
    ptr +="<form class='form' method ='GET' action='/alarmtime'>";
    ptr +="<select class='time' name = hour>";
    ptr +="<option value='0'>0</option>";
    ptr +="<option value='1'>1</option>";
    ptr +="<option value='2'>2</option>";
    ptr +="<option value='3'>3</option>";
    ptr +="<option value='4'>4</option>";
    ptr +="<option value='5'>5</option>";
    ptr +="<option value='6'>6</option>";
    ptr +="<option value='7'>7</option>";
    ptr +="<option value='8'>8</option>";
    ptr +="<option value='9'>9</option>";
    ptr +="<option value='10'>10</option>";
    ptr +="<option value='11'>11</option>";
    ptr +="<option value='12'>12</option>";
    ptr +="<option value='13'>13</option>";
    ptr +="<option value='14'>14</option>";
    ptr +="<option value='15'>15</option>";
    ptr +="<option value='16'>16</option>";
    ptr +="<option value='17'>17</option>";
    ptr +="<option value='18'>18</option>";
    ptr +="<option value='19'>19</option>";
    ptr +="<option value='20'>20</option>";
    ptr +="<option value='21'>21</option>";
    ptr +="<option value='22'>22</option>";
    ptr +="<option value='23'>23</option>";
    ptr +="</select>";
    ptr +=":";
    ptr +="<select class='time' name= minute>";
    ptr +="<option value='00'>00</option>";
    ptr +="<option value='10'>10</option>";
    ptr +="<option value='20'>20</option>";
    ptr +="<option value='30'>30</option>";
    ptr +="<option value='40'>40</option>";
    ptr +="<option value='50'>50</option>";
    ptr +="</select>";
    ptr +="<input class='submit' type='submit' value='Set Alarm'>";
    ptr +="</form>\n"; 

  // Display alarm reset button when alarm is set to value within range of actual time
  if(Alarmhour < 61 && Alarmminute < 61)
    {ptr +="<a class='submit' href='/alarmreset'>Alarmreset</a>\n";}

  // If function that will either show the "ON" or "OFF" button for LEDs
  if(RegenbogenStatus)
    {ptr +="<p>Regenbogen Status: AN</p><a class='button button-off' href='/regenbogen_aus'>AUS</a>\n";}
  else
    {ptr +="<p>Regenbogen Status: AUS</p><a class='button button-on' href='/regenbogen_an'>AN</a>\n";}

  if(SternenshauerStatus)
    {ptr +="<p>Sternenschauer Status: AN</p><a class='button button-off' href='/sternenschauer_aus'>AUS</a>\n";}
  else
    {ptr +="<p>Sternenschauer Status: AUS</p><a class='button button-on' href='/sternenschauer_an'>AN</a>\n";}

  if(SonnenaufgangStatus)
    {ptr +="<p>Sonnenaufgang Status: AN</p><a class='button button-off' href='/sonnenaufgang_aus'>AUS</a>\n";}
  else
    {ptr +="<p>Sonnenaufgang Status: AUS</p><a class='button button-on' href='/sonnenaufgang_an'>AN</a>\n";}

    ptr +="</body>\n";
    ptr +="</html>\n";
  return ptr;
}
