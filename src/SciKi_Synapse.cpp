/* 
 * Science of Kindness - Synapse
 * Author: Daniel Stromberg
 * Date: 8/20/24
*/

#include "Particle.h"
#include <neopixel.h>
#include <math.h>
#include "Button.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

const int PIXEL_COUNT = 190;
const int BUTTON_PIN = D0;
const int FRAME_DELAY = 20;

int brightness = 100;
int lastMillis = 0;
int currentMillis;

Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);
Button dopamineButton(BUTTON_PIN);

void setup() {
    Serial.begin(9600);
    pixel.begin();
    pixel.setBrightness(brightness);

    for(int i=0; i<PIXEL_COUNT; i++){
        pixel.setPixelColor(i, 0x00FF00);
        pixel.show();
        delay(10);
    }

    delay(2000);
    pixel.clear();
    pixel.show();
    

}

void loop() {
    static int currentLED = 0;
    currentMillis = millis();

    if((currentMillis - lastMillis) > FRAME_DELAY){
        if (currentLED < PIXEL_COUNT){
            pixel.clear();
            pixel.setPixelColor(currentLED, 0xFF0000);
            currentLED++;

        } else{
            currentLED = 0;
        }
        lastMillis = currentMillis;
        pixel.show();
    }

}
