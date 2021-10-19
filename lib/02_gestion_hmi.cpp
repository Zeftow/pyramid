#pragma once

#include "01_cube_v2.cpp"
/*
 * Ici tout ce qui concerne le Human Machine Interface (HMI), c'est a dire la gestion des boutons, et tout et tout
 */
 
// POUR INFO appeler read Hmi prend environ 232 micro secondes

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
    Serial.print(" | pot2 = "); Serial.print(pot2);v
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