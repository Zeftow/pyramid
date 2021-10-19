using namespace std;

#include <Arduino.h>
#include <Wire.h>
#include <SSD1306.h>            // 0,96" I2C OLED Screen from amazon
#include <sstream>
#include <string.h>
#include <WiFi.h> 
#include "time.h"
#include <HTTPClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>


#include <HTTPClient.h>
#include <ArduinoJson.h>


String experiment_datetime;

byte mac[6];
String board_id;

int ion_zero = 54;
int ion_span = 1000;
float ion_ppm = 0;
String exp_name = "default";




// Time function
#define NTP_OFFSET  2*3600 // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "2.pool.ntp.org"

// LEDS
#define LED_H6_R 18
#define LED_H6_G 19
#define LED_H6_B 23
#define LED_H5_R 16
#define LED_H5_G 17
#define LED_H5_B 5

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// SCREEN
SSD1306Wire display(0x3c, 21, 22);


// For the led blink:
unsigned long clk_led = 0;
unsigned long clk_db = 0;
unsigned long clk_baseline = 0;

int ledState = LOW;


int sentinelID = 1;

// Time NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// sleep for 30 minutes = 1800 seconds
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
uint64_t TIME_TO_SLEEP = 20;

// Wifi
const char* ssid = "Zeftow's";
const char* password = "hey@zeftow";

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);  

  int timeout = 5*4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi failed to connect");
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 00, "NO WIFI");
    display.display();
    delay(1000);
    ESP.restart();
  }

  Serial.print("WiFi connected after: "); 
  Serial.print(millis());
  Serial.print(" ms, IP address: "); 
  Serial.println(WiFi.localIP());
  WiFi.macAddress(mac);
  board_id =  String(mac[0],HEX) + String(mac[1],HEX) + String(mac[2],HEX) + String(mac[3],HEX) + String(mac[4],HEX) + String(mac[5],HEX);
  String short_id = String(mac[3],HEX) + String(mac[4],HEX) + String(mac[5],HEX);
  Serial.println("BoardID = " + board_id);
  long int number = strtol(short_id.c_str(), NULL, 16);
  Serial.print("Board number = ");
  Serial.println(number);
}

// GOOGLE API KEY
// AIzaSyApj77Yh_tkz90vh85tfz8fm26A5GIF2PU
// GET https://sheets.googleapis.com/v4/spreadsheets/1jcKjlWcNU8u_2qu-b--cxR6yGWE7y82IpwgybKaTzbg/values/uplink!A1:D5?key=AIzaSyApj77Yh_tkz90vh85tfz8fm26A5GIF2PU
/*
String getData() {
  HTTPClient http;

  String serverPath = "https://sheets.googleapis.com/v4/spreadsheets/19yixTZwU5aLfFv8h--gMTwoAbIrUOyqisNa1Kk2H1ek/values/uplink!";
  String dataRange = "B";
  int lineID = sentinelID + 1;
  dataRange = dataRange + lineID + ":D" + lineID;
  String apiKey = "?key=AIzaSyApj77Yh_tkz90vh85tfz8fm26A5GIF2PU";

  // Your Domain name with URL path or IP address with path
  String requestURL = serverPath + dataRange + apiKey;
  http.begin(requestURL.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();
  String payload;
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  ion_zero = doc["values"][0][0];
  ion_span = doc["values"][0][1];
  const char* _name = doc["values"][0][2];
  exp_name = _name;

  return payload;
}

*/

/*
{
  "range": "uplink!A1:C1",
  "majorDimension": "ROWS",
  "values": [
    [
      "72",
      "1234",
      "Benzene test 2"
    ]
  ]
}
*/

/*
void update_screen(){
  // Time management
  if (WiFi.status() == WL_CONNECTED){
    timeClient.update();
  }
  String formattedTime = timeClient.getFormattedTime();
  // Screen display

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, experiment_datetime);

  display.drawString(0, 14, "IonScience : " + (String)(ads_mv) + " mV");
  display.drawString(0, 28, formattedTime);
  display.drawString(0, 42, (String)ion_zero);
  display.drawString(20, 42, (String)ion_span);
  display.drawString(50, 42, exp_name);
  display.display();
}
*/

void update_screen(){
  // Time management
  if (WiFi.status() == WL_CONNECTED){
    timeClient.update();
  }
  String formattedTime = timeClient.getFormattedTime();
  // Screen display

  display.clear();
  display.setFont(ArialMT_Plain_24);

  display.drawString(0, 00, "VOC : ");
  
  if (ion_ppm < 0) {
    display.drawString(13, 30, "0.00 ppm");
  } else if (ion_ppm < 10) {
    display.drawString(13, 30, (String)ion_ppm + " ppm");
  } else {
    display.drawString(0, 30, (String)ion_ppm + " ppm");
  }
  display.display();
}


void led_blink(){
  // led blink every one second to show code is running
  if (millis() - clk_led >= 1000) {
      // save the last time you entered the if
      clk_led = millis();
      // if the LED is OFF turn it ON and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
  }
  if (WiFi.status() == WL_CONNECTED) {
    ledcWrite(1, !ledState*5);
  } else {
    ledcWrite(0, !ledState*5);
    ledcWrite(1, 0);
  }
}



/************************  SETUP ***************************/

void setup(void) {
  Serial.begin(115200);
  Serial.println("Serial Init ok");
  
  display.init();
  display.clear();
  // int counter = 0;
  // display.drawString(0,0, "Counter: " + String(counter));
  // display.drawString(0,0, "Hey Dona");
  display.display();
  // delay(1000);
  display.clear();
  //display.drawString(0,0, "Congrats for the science");
  display.drawString(0,0, "Pyramid Start");
  display.display();

  ledcAttachPin(LED_H5_R, 0);
  ledcAttachPin(LED_H5_G, 1);
  ledcAttachPin(LED_H5_B, 2);
  ledcAttachPin(LED_H6_R, 3);
  ledcAttachPin(LED_H6_G, 4);
  ledcAttachPin(LED_H6_B, 5);

  ledcSetup(0, 5000, 7);
  ledcSetup(1, 5000, 7);
  ledcSetup(2, 5000, 7);
  ledcSetup(3, 5000, 7);
  ledcSetup(4, 5000, 7);
  ledcSetup(5, 5000, 7);

  for (int k=0; k<3; k++){
    for (int i = 0; i <= 5; i++) {
    ledcWrite(k, i);
    ledcWrite(k+3, i);
    delay(20);
    }
    for (int i = 5; i >= 0; i--) {
      ledcWrite(k, i);
      ledcWrite(k+3, i);
      delay(20);
    }
  }



  

  initWifi();
  delay(100);
  // getData();
  timeClient.begin();
  delay(100);
  timeClient.update();

  //Syncing Time
  


   //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);


  //Generating Sensor

  experiment_datetime = timeClient.getFormattedDate();
  experiment_datetime.replace("-", "/");
  experiment_datetime.replace("T", ".");
  experiment_datetime.replace("Z", "");
  experiment_datetime.replace(":", "_");

  Serial.println("Formatted time = "  + experiment_datetime);


  if (timeClient.getEpochTime() < 20000){
    display.clear();
    display.setFont(ArialMT_Plain_24);

    display.drawString(0, 00, "NO NTP");
    display.display();
    delay(1000);
    ESP.restart();
  }
}


/************************  LOOP  ***************************/


void loop(void) {
  // Sensors management and terminal display

  // OLED display management
  update_screen();
  printLocalTime();

  delay(500);
  // Background tasks
  // led_blink();
}