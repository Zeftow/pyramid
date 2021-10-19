#include "east_face.h"

bool sticky_button_1 = 0;
bool sticky_button_2 = 0;
bool sticky_button_3 = 0;
bool sticky_button_4 = 0;
bool sticky_button_5 = 0;
bool sticky_button_6 = 0;
bool sticky_button_7 = 0;
bool sticky_button_8 = 0;
bool sticky_button_9 = 0;
bool sticky_button_10 = 0;

int star_led_1 = 18;
int star_led_2 = 19;
int star_led_3 = 20;
int star_led_4 = 21;
int star_led_5 = 22;
int star_led_6 = 23;
int star_led_7 = 24;
int star_led_8 = 25;
int star_led_9 = 26;
int star_led_10 = 27;

bool star_state_1 = 18;
bool star_state_2 = 19;
bool star_state_3 = 20;
bool star_state_4 = 21;
bool star_state_5 = 22;
bool star_state_6 = 23;
bool star_state_7 = 24;
bool star_state_8 = 25;
bool star_state_9 = 26;
bool star_state_10 = 27;



  enum pattern_state{
    pattern_idle,
    showing_pattern_1,
    showing_pattern_2,
    showing_pattern_3,
    showing_pattern_4,
    showing_pattern_5,
    showing_pattern_6,
  };

  enum button_state{
    button_idle,
    waiting_button_1,
    waiting_button_2,
    waiting_button_3,
    waiting_button_4,
    waiting_button_5,
    waiting_button_6,
    waiting_button_7,
    waiting_button_8,
    waiting_button_9,
    waiting_button_10,
    waiting_button_11,
    waiting_button_12,
    waiting_button_13,
  };
  

void east_puzzle(){
  // Serial.println("Entering east puzzle");
  return;
  for( int i = 0; i < 7; i++) { // Set this face colours to focus on this face
    leds[i].setRGB(120, 0, 170);
  }

  for( int i = 7; i < 18; i++) { // Set other face colours to red
    leds[i].setRGB(70, 0, 0);
  }
  FastLED.show();
  for (int j =255; j>0; j--){   // Fadeout from Cyan to black on this face
    for( int i = 18; i < NUM_LEDS; i++) {
      leds[i].setRGB(0, j, j);
    }
    delay(20);
    FastLED.show();
  }

  // Single LED is blinking while waiting for user press
  


  pattern_state current_pattern_state = showing_pattern_1;
  clk_led = millis();
  button_state current_button_state = button_idle;


  switch (current_pattern_state) {

  case showing_pattern_1:
    for( int i = 18; i < NUM_LEDS; i++) { // set face to black
      leds[i] = CRGB::Black;
    }
    if (millis() - clk_led >= 1000) { // Blink a single led
      clk_led = millis();
      if (star_state_1 == LOW) {
        star_state_1 = HIGH;
        leds[star_led_1].setHue(135);
      } else {
        star_state_1 = LOW;
        leds[star_led_1] = CRGB::Black;
      }
    }
    break;

    case showing_pattern_2:
      if (millis() - clk_led >= 1000 && millis() - clk_led < 2000) {
          star_state_1 = HIGH;
          leds[star_led_1].setHue(135);
      } else if(millis() - clk_led >= 2000 && millis() - clk_led < 3000){
        star_state_1 = LOW;
        leds[star_led_1] = CRGB::Black;
        star_state_2 = HIGH;
        leds[star_led_2].setHue(135);
      } else if (millis() - clk_led >= 3000 && millis() - clk_led < 4000){
        star_state_2 = LOW;
        leds[star_led_2] = CRGB::Black;
        star_state_3 = HIGH;
        leds[star_led_3].setHue(135);
      }
    break;
  
  }

  FastLED.show();
  
}