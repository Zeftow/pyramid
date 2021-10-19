#pragma once
using namespace std;

#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
#include <string.h>
#include "time.h"


#define NUM_LEDS 50  


extern CRGB leds[NUM_LEDS];
extern unsigned long clk_led;
extern uint32_t east_clock;

void east_puzzle();

