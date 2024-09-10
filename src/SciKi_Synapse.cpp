/* 
 * Science of Kindness - Synapse
 * Author: Daniel Stromberg
 * Date: 8/20/24
*/

#include "Particle.h"
#include <neopixel.h>
#include <math.h>
#include "Button.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

const int PIXEL_COUNT = 1222; //was 190
const int DOP_BUTTON_PIN = D10;
const int SER_BUTTON_PIN = D16;
const int FRAME_DELAY = 5;
const int ledColor = 0xFF2299;
const int MAX_BRIGHTNESS = 100;
const int LEADER_WIDTH = 20;
const int TAIL_LENGTH = LEADER_WIDTH /2;
const int NUM_LEADERS = PIXEL_COUNT / LEADER_WIDTH;
const int ORIGINS = PIXEL_COUNT/2;
const int SER_RECEPTOR_PIXELS[] = {613, 775};
const int DOP_RECEPTOR_PIXELS[] = {553, 830};
const int ANIMATION_SPEED = 1; //number of pixels to move per frame
const uint32_t DOPAMINE_COLOR = 0xFFAA00;
const uint32_t SERATONIN_COLOR = 0x00FFFF;
const int LED_STRIP_BREAKS[] = {210, 386, 690, 890, 1050, PIXEL_COUNT-1};
const uint32_t TEST_COLORS[] = {0x00FF00, 0x0000FF, 0xFF0000, 0xFF00FF, 0x00FFFF, 0xFFFF00};

int dopamineLeaderPos[PIXEL_COUNT / LEADER_WIDTH];
int seratoninLeaderPos[PIXEL_COUNT / LEADER_WIDTH];
int lastMillis = 0;
int currentMillis;
int pixBrightness[PIXEL_COUNT];
int testBrightness = 255;
int currentLED = 0;
float t;
float breatheBrightness;
byte redFromHex, greenFromHex, blueFromHex;

Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);
Button dopamineButton(DOP_BUTTON_PIN);
Button seratoninButton(SER_BUTTON_PIN);

uint32_t blendColor(uint32_t color1, uint32_t color2, float ratio);
void segmentMarquee(uint32_t color, int origin, int min, int max, int leaderPositions[]);
void segmentFill(int startLED, int endLED, uint32_t fillColor);
void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed);
void hexToRGB(int colorIn, byte *redOut, byte *greenOut, byte *blueOut);
int rgbToHex(byte redIn, byte greenIn, byte blueIn);

void setup() {
    Serial.begin(9600);
    pixel.begin();
    pixel.setBrightness(MAX_BRIGHTNESS);

    for(int h=0; h<NUM_LEADERS; h++){
        //Later: only place leaders within one segment of the strip
        dopamineLeaderPos[h] = h*(LEADER_WIDTH);
        seratoninLeaderPos[h] = h*(LEADER_WIDTH);
    }

    for (int i=0; i<6; i++){
        if(i-1>=0){
            segmentFill(LED_STRIP_BREAKS[i-1], LED_STRIP_BREAKS[i], TEST_COLORS[i]);
        } else{
            segmentFill(0, LED_STRIP_BREAKS[i], TEST_COLORS[i]);
        }
    }
    pixel.setPixelColor(SER_RECEPTOR_PIXELS[0], 0x0000FF);
    pixel.setPixelColor(DOP_RECEPTOR_PIXELS[0], 0x00FF00);

    pixel.show();
    while(!dopamineButton.isPressed()){
        delay(500);
    }

    pixel.clear();
    pixel.show();
}

void loop() {
    currentMillis = millis();
    pixel.clear();

    if(dopamineButton.isPressed()){
        segmentMarquee(DOPAMINE_COLOR, DOP_RECEPTOR_PIXELS[0], LED_STRIP_BREAKS[1], LED_STRIP_BREAKS[2], dopamineLeaderPos);
        segmentMarquee(DOPAMINE_COLOR, DOP_RECEPTOR_PIXELS[1], LED_STRIP_BREAKS[2], LED_STRIP_BREAKS[3], dopamineLeaderPos);

    } else if(seratoninButton.isPressed()){
        segmentMarquee(SERATONIN_COLOR, SER_RECEPTOR_PIXELS[0], LED_STRIP_BREAKS[1], LED_STRIP_BREAKS[2], seratoninLeaderPos);
        segmentMarquee(SERATONIN_COLOR, SER_RECEPTOR_PIXELS[1], LED_STRIP_BREAKS[2], LED_STRIP_BREAKS[3], seratoninLeaderPos);
    }

    if(!seratoninButton.isPressed() && !dopamineButton.isPressed()){
        pixel.clear();
    }

    segmentBreathe(LED_STRIP_BREAKS[4], LED_STRIP_BREAKS[5], DOPAMINE_COLOR, 2.9);
    segmentBreathe(LED_STRIP_BREAKS[3], LED_STRIP_BREAKS[4], SERATONIN_COLOR, 3.0);

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

void segmentMarquee(uint32_t color, int origin, int min, int max, int leaderPositions[]){

    for(int i=0; i<NUM_LEADERS; i++){
        currentLED = leaderPositions[i];

        pixel.setPixelColor(currentLED, color);

        //move pixels away from receptor,
        if(currentLED < origin){ 
            if(currentLED >min){
                leaderPositions[i]-= ANIMATION_SPEED;
            }else{
                leaderPositions[i] = origin-1;
            }
        }else{
            if(currentLED < max){
                leaderPositions[i]+=ANIMATION_SPEED;
            }else{
                leaderPositions[i] = origin+1;
            }
        }
    }
}

void segmentFill(int startLED, int endLED, uint32_t fillColor){
    for(int i=startLED; i <= endLED; i++){
        pixel.setPixelColor(i, fillColor);
    }
}

void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed){
    t = millis()/1000.0 + 0.5;
    breatheBrightness = 127*sin(M_PI*t/speed)+127;

    hexToRGB(breatheColor, &redFromHex, &greenFromHex, &blueFromHex);

    for(int i = startLED; i <= endLED; i++){
        pixel.setColorScaled(i, redFromHex, greenFromHex, blueFromHex, breatheBrightness);
    }
}

void hexToRGB(int colorIn, byte *redOut, byte *greenOut, byte *blueOut){
    *blueOut = colorIn & 0xFF;
    *redOut = colorIn>>16;
    *greenOut = (colorIn>>8) & 0xFF;
}

int rgbToHex(byte redIn, byte greenIn, byte blueIn){
    return (redIn<<16) | (greenIn<<8) | blueIn;
}