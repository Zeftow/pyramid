using namespace std;

#include <Arduino.h>
#include <Wire.h>
#include <SSD1306.h>            // 0,96" I2C OLED Screen from amazon
#include "Adafruit_SGP30.h"     // I2C TVOC and eC02 sensor
#include <Adafruit_ADS1015.h>   // 16 bit adc, better library
#include <sstream>
#include <string.h>
#include <WiFi.h> 
#include <HTTPClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>


#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Point.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>


// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com/"
// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "hGvHIZBp-ErtNvc7ixXvwIyGkmS_HRmzrDjdxS6Co-M9rz0ow1ATIG0O1QTSCmFoOcb3x56R7HsqUFk3ZJybBw=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "cb8c4e47d9f58969"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "sentinels"
// Define the Timezone
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
//Single InfluxDB instance
InfluxDBClient clientInflux(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("truc");
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

// for ADS1115
Adafruit_ADS1115 ads;
float ads_mv_per_bit = 0.125;
float ads_mv = 0;

void update_ads(){
  ads_mv = ads.readADC_Differential_0_1() * ads_mv_per_bit;
  Serial.println("ads_mv = " + (String)ads_mv + " mV");
  ion_ppm = ((ads_mv-ion_zero)/(ion_span-ion_zero)*30);
}


// For the led blink:
unsigned long clk_led = 0;
unsigned long clk_db = 0;
unsigned long clk_baseline = 0;

int ledState = LOW;


int sentinelID = 1;


// sleep for 30 minutes = 1800 seconds
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
uint64_t TIME_TO_SLEEP = 20;

// Wifi
const char* ssid = "TP-Link 4G";
const char* password = "Neocorona420";

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

  switch (number) {
  
  case 13687848:  // 7c9ebdd0dc28
    sentinelID = 1;
    Serial.println("Board ID set to 1");
    break;


  case 13687420:  // 7c9ebdd0da7c
    sentinelID = 2;
    Serial.println("Board ID set to 2");
    break;
  
  case 13687512:  // 7c9ebdd0dad8
    sentinelID = 3;
    Serial.println("Board ID set to 3");
    break;

  case 855472:  // 7c9ebdd0db00
    sentinelID = 4;
    Serial.println("Board ID set to 4");
    break;

  case 13687832:  // 7c9ebdd0dc18
    sentinelID = 5;
    Serial.println("Board ID set to 5");
    break;

  case 13687476:  // 7c9ebdd0dab4
    sentinelID = 6;
    Serial.println("Board ID set to 6");
    break;

  case 13687852:  // 7c9ebdd0dc2c
    sentinelID = 7;
    Serial.println("Board ID set to 7");
    break;

  case 13687536:  // 7c9ebdd0daf0
    sentinelID = 8;
    Serial.println("Board ID set to 8");
    break;

  case 13687548:  // 7c9ebdd0dafc
    sentinelID = 9;
    Serial.println("Board ID set to 9");
    break;

  case 13687480:  // 7c9ebdd0dab8
    sentinelID = 10;
    Serial.println("Board ID set to 10");
    break;

  case 13687744:  // 7c9ebdd0dbc0
    sentinelID = 11;
    Serial.println("Board ID set to 11");
    break;

  case 855480:  // 7c9ebdd0db8
    sentinelID = 12;
    Serial.println("Board ID set to 12");
    break;

  default:
  Serial.println("Board not recognized, booting with board 1 setup");
    break;
  }
}


// GOOGLE API KEY
// AIzaSyApj77Yh_tkz90vh85tfz8fm26A5GIF2PU
// GET https://sheets.googleapis.com/v4/spreadsheets/1jcKjlWcNU8u_2qu-b--cxR6yGWE7y82IpwgybKaTzbg/values/uplink!A1:D5?key=AIzaSyApj77Yh_tkz90vh85tfz8fm26A5GIF2PU

String getData(){
  HTTPClient http;

  String serverPath = "https://sheets.googleapis.com/v4/spreadsheets/19yixTZwU5aLfFv8h--gMTwoAbIrUOyqisNa1Kk2H1ek/values/uplink!";
  String dataRange = "B";
  int lineID = sentinelID + 1;
  dataRange = dataRange + lineID + ":D" + lineID;
  String apiKey = "";

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


void write_db(){
  if (millis() - clk_db >= 1000) {
    clk_db = millis();
      // InfluxDB data push
    sensor.clearFields();
    sensor.addField("ads_mv", ads_mv);
    sensor.addField("ion_ppm", ion_ppm);
    if (!clientInflux.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(clientInflux.getLastErrorMessage());
    }
  }
  int requesTime = millis() - clk_db;
  Serial.println("Request time : " + (String)requesTime + " ms");
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
  display.drawString(0,0, "<3 sunshine :-*");
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



  ads = Adafruit_ADS1115(0x48);
  ads.begin();
  ads.setGain(GAIN_ONE);
  
  update_ads();

  initWifi();
  delay(100);
  getData();
  timeClient.begin();
  delay(100);
  timeClient.update();


  // Check server connection
  if (clientInflux.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(clientInflux.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(clientInflux.getLastErrorMessage());
    ESP.restart();
  }

  //Syncing Time
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  //Generating Sensor

  experiment_datetime = timeClient.getFormattedDate();
  experiment_datetime.replace("-", "/");
  experiment_datetime.replace("T", ".");
  experiment_datetime.replace("Z", "");
  experiment_datetime.replace(":", "_");

  Serial.println("Formatted time = "  + experiment_datetime);

  sensor = Point("Sentinel_" + (String)sentinelID);
  sensor.addTag("device", "Sentinel_"+(String)sentinelID);
  sensor.addTag("experiment", exp_name);
  sensor.addTag("experiment_datetime", experiment_datetime);

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
  update_ads();

  // OLED display management
  update_screen();

  write_db();

  // Background tasks
  // led_blink();
}