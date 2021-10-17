#pragma once

/*
 * Ici les définitions de variables globales au code
 */

#include <Arduino.h> //: Nécessaire en C++ mais pas en Arduino

#include <FastLED.h>

#define NUM_LEDS 144       // How many leds in your strip?
#define BRIGHTNESS 30      // Max 30 to keep it resonable
#define FRAMES_PER_SECOND  72  // To keep frameRate resonable
#define STEPS 500          // Typical animation lengh 
#define STEPS_DELAY 10     // Typical delay to keep framerate resonable

// Define the arrays of leds
CRGB leds[NUM_LEDS];


//Attenuation factor of the decrease() function, close to 1 means longer tail for a moving dot
float attenuation = 0.85;

//Listes de "comment qu'il est construit ce cube" (pour passer d'une arrête à une autre)


/*// Cube V1
  #define HARDWARE 1 // La version du hardware utilisée dans le setup
  #define DATA_PIN 12
  #define BUTTON_1_PIN 3

  //int L0[24]={0 ,11,12,23,24,35,36,47,48,59 ,60 ,71,72,83 ,84 ,95,96 ,107,108,119,120,131,132,143};
  int L1[24]={47,71,71,72,23,36,35,48,0 ,131,120,11,23,107,143,35,84 ,108,107,120,60 ,132,59 ,84 };
  int L2[24]={48,12,11,24,72,95,95,0 ,47,132,119,12,24,108,96 ,36,143,83 ,83 ,60 ,119,59 ,131,96 };
  int Symetrie[12]={8,11,10,9,6,7,4,5,0,3,2,1};
//*/


// Cube V2
#define HARDWARE 2  // La version du hardware utilisée dans le setup
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
// Listes décrivant comment sont agencés les sommets
// int L0[24]={ 0, 11, 12, 23, 24, 35, 36, 47, 48,  59,  60, 71, 72,  83,  84, 95,  96, 107, 108, 119, 120, 131, 132, 143};
int L1[24] = {47, 12, 71, 24, 72, 36, 95,  0, 47, 108, 120, 11, 23, 132,  96, 35,  84, 108,  59,  60, 119,  83, 131,  84};
int L2[24] = {48, 71, 11, 72, 23, 95, 35, 48,  0, 107, 119, 12, 24, 131, 143, 36, 143,  59, 107, 120,  60, 132,  83,  96};
int Symetrie[12] = {11, 8, 9, 10, 6, 7, 4, 5, 1, 2, 3, 0};
//*/

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