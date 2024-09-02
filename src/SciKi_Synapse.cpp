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

const int PIXEL_COUNT = 1266; //was 190
const int DOP_BUTTON_PIN = D10;
const int SER_BUTTON_PIN = D16;
const int FRAME_DELAY = 5;
const int ledColor = 0xFF2299;
const int MAX_BRIGHTNESS = 100;
const int LEADER_WIDTH = 20;
const int TAIL_LENGTH = LEADER_WIDTH /2;
const int NUM_LEADERS = PIXEL_COUNT / LEADER_WIDTH;
const int ORIGINS = PIXEL_COUNT/2;
const int SER_RECEPTOR_PIXEL = 900;
const int DOP_RECEPTOR_PIXEL = 400;
const int ANIMATION_SPEED = 1; //number of pixels to move per frame
const uint32_t DOPAMINE_COLOR = 0xFF0000;
const uint32_t SERATONIN_COLOR = 0x00FFFF;

int leaderPositions[PIXEL_COUNT / LEADER_WIDTH];
int lastMillis = 0;
int currentMillis;
int pixBrightness[PIXEL_COUNT];
int testBrightness = 255;
int currentLED = 0;

Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);
Button dopamineButton(DOP_BUTTON_PIN);
Button seratoninButton(SER_BUTTON_PIN);

uint32_t blendColor(uint32_t color1, uint32_t color2, float ratio);
void lightStrip(uint32_t color, int origin);

void setup() {
    Serial.begin(9600);
    pixel.begin();
    pixel.setBrightness(MAX_BRIGHTNESS);

    for(int h=0; h<NUM_LEADERS; h++){
        leaderPositions[h] = h*(LEADER_WIDTH);
    }

    pixel.clear();
    pixel.show();
}

void loop() {
    currentMillis = millis();
    pixel.clear();

    if(dopamineButton.isPressed()){
        lightStrip(DOPAMINE_COLOR, DOP_RECEPTOR_PIXEL);
    }

    if(seratoninButton.isPressed()){
        lightStrip(SERATONIN_COLOR, SER_RECEPTOR_PIXEL);
    }

    if(!seratoninButton.isPressed() && !dopamineButton.isPressed()){
        pixel.clear();
    }

    pixel.show();
}

uint32_t blendColor(uint32_t color1, uint32_t color2, float ratio) {
  // Blends two colors based on the given ratio
  uint8_t r1 = (color1 >> 16) & 0xFF;
  uint8_t g1 = (color1 >> 8) & 0xFF;
  uint8_t b1 = color1 & 0xFF;

  uint8_t r2 = (color2 >> 16) & 0xFF;
  uint8_t g2 = (color2 >> 8) & 0xFF;
  uint8_t b2 = color2 & 0xFF;

  uint8_t r = r1 * (1 - ratio) + r2 * ratio;
  uint8_t g = g1 * (1 - ratio) + g2 * ratio;
  uint8_t b = b1 * (1 - ratio) + b2 * ratio;

  return pixel.Color(r, g, b);
}

void lightStrip(uint32_t color, int origin){
    for(int i=0; i<NUM_LEADERS; i++){
        currentLED = leaderPositions[i];

        for(int j=-LEADER_WIDTH; j<LEADER_WIDTH; j++){
        int fadeIndex = currentLED + j;

        if (fadeIndex<0) fadeIndex +=origin;
        if (fadeIndex >= PIXEL_COUNT) fadeIndex -= PIXEL_COUNT;

        float fadeBrightness = 1.0 -abs(j) / (float)LEADER_WIDTH;

        uint32_t fadeColor = blendColor(0, color, fadeBrightness);
        pixel.setPixelColor(fadeIndex, fadeColor);

        }

        //move pixels away from receptor,
        if(currentLED < origin){ 
        if(currentLED >0){
            leaderPositions[i]-= ANIMATION_SPEED;
        }else{
            leaderPositions[i] = origin-1;
        }
        }else{
        if(currentLED <PIXEL_COUNT){
            leaderPositions[i]+=ANIMATION_SPEED;
        }else{
            leaderPositions[i] = origin;
        }
        }
    }
}