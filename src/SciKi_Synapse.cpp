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
const int ledColor = 0xFF2299;
const int MAX_BRIGHTNESS = 100;
const int TAIL_LENGTH = 10;

int lastMillis = 0;
int currentMillis;
int pixBrightness[PIXEL_COUNT];
int testBrightness = 255;
int currentLED = 0;

Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);
Button dopamineButton(BUTTON_PIN);

void setup() {
    Serial.begin(9600);
    pixel.begin();
    pixel.setBrightness(MAX_BRIGHTNESS);

    // Test pixels on startup
    // 
    for(int i=0; i<PIXEL_COUNT; i++){
        pixBrightness[i] = 0;
        pixel.setPixelColor(i, 0x00FF00);
        pixel.show();
    }
    // 
    // delay(2000);
    pixel.clear();
    pixel.show();
    

}

void loop() {
    currentMillis = millis();
    int timeSinceLastFrame = currentMillis-lastMillis;



    if(timeSinceLastFrame > FRAME_DELAY){
        pixel.clear();
        pixBrightness[currentLED] = MAX_BRIGHTNESS;

////////////////////////// CURRENTLY WORKS //////////////
        // for(int j = 0; j<PIXEL_COUNT; j++){
        //     pixel.setColorScaled(j, 255, 30, 100, pixBrightness[j]);
        // }

        // for(int i=0; i<(PIXEL_COUNT-1); i++){
        //     if(pixBrightness[i]>0){
        //         pixBrightness[i] = pixBrightness[i] -1;
        //     }
        // }
///////////////////////

        for(int k = 1; k <=TAIL_LENGTH; k++){
            int tailLED = currentLED - k;
            if (tailLED < 0){       //wrap around if pixel is negative
                tailLED+=PIXEL_COUNT;
            }
            int tailBrightness = MAX_BRIGHTNESS * (TAIL_LENGTH-k)/TAIL_LENGTH;
            pixBrightness[tailLED] = tailBrightness;
        }

        for(int j = 0; j<PIXEL_COUNT; j++){
            pixel.setColorScaled(j, 255, 30, 100, pixBrightness[j]);
        }

        //Move to next LED
        currentLED++;
        if (currentLED >= PIXEL_COUNT){
            currentLED=0;
        }

        lastMillis = currentMillis;
        // Serial.printf("currentLED: %i\n", currentLED);
        // Serial.printf("time since frame: %i\n\n", timeSinceLastFrame);
        pixel.show();
    }

    
}
