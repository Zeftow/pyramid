#pragma once

/*
 * Ici tout ce qui concerne la centrale Intertielle
 */
#include "02_gestion_hmi.cpp"


// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
// IMU SDA on pin A4, SCL on pin A5
/*
// POUR INFO: appeler readImu() prend environ 62 micro secondes.
  unsigned long StartTime = micros();
  for(int k=0; k<10000; k++){
    readImu();
  }
  unsigned long CurrentTime = micros();
  unsigned long tempsImu = (CurrentTime - StartTime)/10000;
  Serial.print("Temps Imu = "); Serial.println(tempsImu);
*/
#include<Wire.h>

const int MPU_addr_cube=0x68;  // I2C address of the MPU-6050 on the cube
const int MPU_addr_remote=0x69;  // I2C address of the MPU-6050 on the remote
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void setupImu(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr_remote);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

/*
 * Cette fonction appelle l'IMU et enregistre les valeurs dans des variables globales.
 */
void readImu(){
  Wire.beginTransmission(MPU_addr_remote);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr_remote,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)

  // On rajoute 4000 pour avoir les mêmes variations chez tout le monde, jsp d'où ça vient...
  AcZ=4000 + (Wire.read()<<8|Wire.read());  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  /*
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  //delay(300);
  //*/
}

/*
 * Typical Static Values:
 * Ac values between 16300 and -16300 typically, can reach +-32000 on impact
 * Gy values easily reach 32000 when changing direction of roation
 */
 
uint8_t mAcX, mAcY, mAcZ, mGyX, mGyY, mGyZ;
uint8_t mAcXNew, mAcYNew, mAcZNew, mGyXNew, mGyYNew, mGyZNew;
// uint8_t mAcXOld, mAcYOld, mAcZOld, mGyXOld, mGyYOld, mGyZOld;
/*
 * Cette fonction permet de convertir les valeurs de l'accelero en valeurs 0-255 utilisables par FastLED
 */
void mapAc(){
  AcX = constrain(AcX, -17000, 17000);
  AcY = constrain(AcY, -17000, 17000);
  AcZ = constrain(AcZ, -17000, 17000);
  mAcXNew = map(AcX, -17000, 17000, 0, 255);
  mAcYNew = map(AcY, -17000, 17000, 0, 255);
  mAcZNew = map(AcZ, -17000, 17000, 0, 255);
  mAcX = (5 * mAcX + mAcXNew)/6;
  mAcY = (5 * mAcY + mAcYNew)/6;
  mAcZ = (5 * mAcZ + mAcZNew)/6;
  
  /*
  Serial.print("mAcX = "); Serial.print(mAcX);
  Serial.print(" | mAcY = "); Serial.print(mAcY);
  Serial.print(" | mAcZ = "); Serial.println(mAcZ);
  delay(500);
  */
}

void mapAcFast(){
  AcX = constrain(AcX, -17000, 17000);
  AcY = constrain(AcY, -17000, 17000);
  AcZ = constrain(AcZ, -17000, 17000);
  mAcX = map(AcX, -17000, 17000, 0, 255);
  mAcY = map(AcY, -17000, 17000, 0, 255);
  mAcZ = map(AcZ, -17000, 17000, 0, 255);

}
/*
 * Cette fonction permet de convertir les valeures du gyro en valeurs 0-255 utilisables par FastLED
 */
void mapGy(){
    
    mGyX = map(GyX, -32768, 32767, 0, 255);
    mGyY = map(GyY, -32768, 32767, 0, 255);
    mGyZ = map(GyZ, -32768, 32767, 0, 255);
}
