/*
 #ESP32 Web Server satellite maps
 #Gareth aka chiprobot
*/
#include <WiFi.h>
const char* ssid     = "place your wifi id here";
const char* password = "place your password here";
WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
 // connecting to your WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);

WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");    // this is the address to use for viewing the map
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop(){
 WiFiClient client = server.available();    // listen for incoming clients

  if (client) {                             
    Serial.println("new client");          
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            
      if (client.available()) {             // if there's client data
        char c = client.read();          // read a byte
          if (c == '\n') {                      // check for newline character,
          if (currentLine.length() == 0) {  // if line is blank it means its the end of the client HTTP request
            client.println("<!DOCTYPE html>"); // open wrap the web page
            client.print("<html><head><meta name='viewport' content='initial-scale=1.0'><meta charset='utf-8'><style>#map {height: 100%;}html, body {height: 100%;margin: 0;padding: 0;}</style></head>");
    
 client.print("<body><h1>ESP32 Server Maps 'Pyramids at Giza'</h1><div id='map'></div><script>function initMap(){var map = new google.maps.Map(document.getElementById('map'),{center: {lat: 29.975995, lng: 31.130806},zoom: 16,mapTypeId: 'satellite'});}</script><script async defer src='https://maps.googleapis.com/maps/api/js?key=place_your_authentication_key_here&callback=initMap'></script>");

            client.print("</body></html>"); // close wrap the web page

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {   currentLine = ""; }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
         // here you can check for any keypresses if your web server page has any
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    }
}