/* 
 * LED Picker
 *      -Find the number of an LED on a strip.
 * Author: Daniel Stromberg
 * Date: 9/11/2024
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <neopixel.h>
#include <math.h>

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

const int PIXEL_COUNT = 1222;
const int ROUGH_POT_PIN = A5;
const int FINE_POT_PIN = A1;
const int POT_THRESHOLD = 25;

int lastMillis=-999;
int roughLEDNum;
int lastRoughPotVal=0;
int fineLEDNum;
int ledToLight;
int lastLEDToLight;
int roughPotReading;
int finePotReading;


Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected, 10000);
    pinMode(ROUGH_POT_PIN, INPUT);
    pinMode(FINE_POT_PIN, INPUT);

    pixel.begin();
    pixel.setBrightness(150);
    pixel.clear();
    pixel.show();
}

void loop() {
    // pixel.clear();

    roughPotReading = analogRead(ROUGH_POT_PIN);
    delay(25);
    finePotReading = analogRead(FINE_POT_PIN);

    if(abs(roughPotReading-lastRoughPotVal)>POT_THRESHOLD){
        lastRoughPotVal = roughPotReading;
    }
    
    roughLEDNum = map(lastRoughPotVal, 0, 4095, 0, PIXEL_COUNT);

    fineLEDNum = map(finePotReading, 0, 4095, -10, 10);
    ledToLight = constrain((roughLEDNum+fineLEDNum), 0, PIXEL_COUNT);

    pixel.setPixelColor(ledToLight, 0x00FFFF);

    if(lastLEDToLight != ledToLight){
        pixel.setPixelColor(lastLEDToLight, 0);
        pixel.show();
        lastLEDToLight = ledToLight;
    }
    if(millis()-lastMillis >1000){
        Serial.printf("Fine LED Num: %i\n", fineLEDNum);
        Serial.printf("Rough LED num: %i\n", roughLEDNum);
        Serial.printf("CURRENT LED: %i\n\n", ledToLight);
        lastMillis=millis();
    }    
}
