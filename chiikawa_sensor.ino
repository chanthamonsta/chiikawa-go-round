#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

// DFPlayer setup
SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// MPU6050 setup
Adafruit_MPU6050 mpu;

// Rotation detection
float threshold = .5;  // degrees per second threshold for movement
bool isPlaying = false;
unsigned long lastCheck = 0;

void setup() {
  Serial.begin(115200);
  mySoftwareSerial.begin(9600);

  Serial.println(F("Initializing DFPlayer Mini..."));
  if (!myDFPlayer.begin(mySoftwareSerial, false, true)) {
    Serial.println(F("Unable to begin DFPlayer Mini:"));
    Serial.println(F("1. Check connections."));
    Serial.println(F("2. Insert SD card."));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(30);
  myDFPlayer.play(1);
  myDFPlayer.pause();  // Start paused

  // Initialize MPU6050
  Serial.println(F("Initializing MPU6050..."));
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("MPU6050 Found!"));

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

  delay(100);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);


  // Check every 100 ms
  if (millis() - lastCheck > 100) {
      Serial.println(F("Checking!"));
    lastCheck = millis();

    // We're mainly interested in rotation around Z axis (the spin)
    float spinSpeed = abs(g.gyro.z);
    Serial.println("What is spin speed?");
    Serial.println(spinSpeed);


    if (spinSpeed > threshold && !isPlaying) {
      Serial.print("Spinning detected! GyroZ: ");
      Serial.println(spinSpeed);
      myDFPlayer.start();  // Resume playback
      isPlaying = true;
    }
    else if (spinSpeed <= threshold && isPlaying) {
      Serial.print("Stopped spinning. GyroZ: ");
      Serial.println(spinSpeed);
      myDFPlayer.pause();
      isPlaying = false;
    }
  }
}