using namespace std;

#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
#include <string.h>
#include "time.h"
#include "Adafruit_MPR121.h"

#include "east_face.h"


#define NUM_LEDS 50       // How many leds in your strip?
#define BRIGHTNESS 30      // Max 30 to keep it resonable
#define FRAMES_PER_SECOND  72  // To keep frameRate resonable
#define STEPS 500          // Typical animation lengh 
#define STEPS_DELAY 10     // Typical delay to keep framerate resonable

#define DATA_PIN 12 // là ou est connecté le ruban
#define BUTTON_1 4  // Bouton relié a une digital IO, l'autre patte doit être à la masse
#define BUTTON_2 5  // Bouton relié a une digital IO, l'autre patte doit être à la masse
#define BUTTON_3 6  // Bouton relié a une digital IO, l'autre patte doit être à la masse
#define BUTTON_4 7  // Bouton relié a une digital IO, l'autre patte doit être à la masse
#define POT_1 0     // Potentiomètre (Un coté au VCC, l'autre au GND, le troisième dans un analog in
#define POT_2 1
#define POT_3 2
#define POWER_LED 2 // Led utilisée pour indiquer si la luminosité est bridée par la gestion de la puissance
#define FEEDBACK_LED 3 // Led multi-usages 

// MPR121
#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
// Default address is 0x5A, if tied to 3.3V its 0x5B
// If tied to SDA its 0x5C and if SCL then 0x5D
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;


// Define the arrays of leds
CRGB leds[NUM_LEDS];

// Triangle variable
uint8_t triangle = 0;
int8_t rise = 1;

// Game State
int8_t game_state = 0;


// Valeurs du RGB pour une led donnée, déclarées ici comme ça elles prennent pas de la place inutilement à chaque fois qu'elles sont déclarées
int R = 0;
int G = 0;
int B = 0;

// Variables Globales pour le code
uint8_t gStep = 0; //Global Step, permet aux animation de savoir où elles en sont.
uint8_t gHue = 0;  // Global Hue (=Color) pour avoir des transitions cohérentes entre les animations.
int gSpeed = 144;  // To keep frameRate resonable //Redondant avec un define FRAMERATE
bool nextFlag = false;
uint8_t currentAnimation = 0;


// For the led blink:
unsigned long clk_led = 0;
unsigned long clk_db = 0;
unsigned long clk_baseline = 0;

int ledState = LOW;

// For the HMI
int pot1, pot2, pot3, sw1, sw2, sw3, sw4, temp;
int compteurHmi = 6;
bool yoloFlag = false;

void readHmi(){
    pot1 = analogRead(POT_1);
    pot2 = analogRead(POT_2);
    pot3 = analogRead(POT_3);
    sw1 = !digitalRead(BUTTON_1);
    sw2 = !digitalRead(BUTTON_2);
    sw3 = !digitalRead(BUTTON_3);
    sw4 = !digitalRead(BUTTON_4);
    pot1 = map(pot1, 0, 1023, 255, 0);
    pot2 = map(pot2, 0, 1023, 255, 0);
    pot3 = map(pot3, 0, 1023, 255, 0);

    if(sw4){
        nextFlag = true;
    }  

    if(sw3){
        compteurHmi++;
        if(compteurHmi == 13){
            compteurHmi = 0;
        }
        delay(200);
    }
    if(sw2){
        compteurHmi--;
        if(compteurHmi == -1){
            compteurHmi = 12;
        }
        delay(200);
    }

    yoloFlag = sw1;
    // Debug brightness mapping
    // temp = map(pot1, 0, 1023, 0, 100);
    // FastLED.setBrightness(temp); //S'occupe directement de diminuer la luminosité
    
    
    /*
    // Debug serial print
    Serial.print("pot1 = "); Serial.print(pot1);
    Serial.print(" | pot2 = "); Serial.print(pot2);
    Serial.print(" | pot3 = "); Serial.print(pot3);
    Serial.print(" | sw1 = "); Serial.print(sw1);
    Serial.print(" | sw2 = "); Serial.print(sw2);
    Serial.print(" | sw3 = "); Serial.println(sw3);
    delay(200);
    //*/

  
}


void mapPots(int mini, int maxi){
    pot1 = map(pot1, 0, 255, mini, maxi);
    pot2 = map(pot2, 0, 255, mini, maxi);
    pot3 = map(pot3, 0, 255, mini, maxi);
}


// PAtterns
float w = 0;
uint8_t phi = 0;

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
  
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))



void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}






void standby(){
  for( int i = 0; i < 18; i++) {
    leds[i].setRGB(triangle, 0, triangle);
  }
  for( int i = 18; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  if(millis() > 3000){
    game_state++;
    Serial.println("GameState incremented, exiting standby");
  }
}



void update_triangle(){
  if (triangle == 255){
    rise = -1;
  }
  if (triangle == 0){
    rise = 1;
  }
  triangle += rise;
}

SimplePatternList gPatterns = {standby, east_puzzle };

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}


/************************  SETUP ***************************/

void setup(void) {
  Serial.begin(115200);
  Serial.println("Serial Init ok");
  FastLED.addLeds<WS2811,DATA_PIN,RGB>(leds, NUM_LEDS).setCorrection(TypicalPixelString);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}


/************************  LOOP  ***************************/

void loop(void) {
  
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[game_state]();


  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { update_triangle(); } // slowly cycle the "base color" through the rainbow

  // Get the currently touched pads
  currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }

  
}