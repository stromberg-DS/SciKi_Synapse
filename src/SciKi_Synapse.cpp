/* 
 * Science of Kindness - Synapse
 * Author: Daniel Stromberg
 * Date: 8/20/24
*/

//TO DO:
//  -make a small program to pick colors and print out hex code
//  -make a small program to find location of LEDs

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
const int ORIGINS = PIXEL_COUNT/2;
const int OUTSIDE = 0;
const int INSIDE = 1;
const int SER_RECEPTOR_PIXELS[] = {613, 775};
const int DOP_RECEPTOR_PIXELS[] = {553, 830};
const int ANIMATION_SPEED = 1; //number of pixels to move per frame
const uint32_t DOPAMINE_COLOR = 0xFF5500;
const uint32_t SERATONIN_COLOR = 0x11AAFF;
const uint32_t BASE_LED_COLOR = 0xFF1500;

//Update segment endpoints for final installation
//If possible, make (SEG_3_END-SEG_2_END) and (SEG_4_END-SEG_3_END) divisible by 20. Helps with spacing.
const int SEG_1_END = 210;
const int SEG_2_END = 390;
const int SEG_3_END = 690;
const int SEG_4_END = 890;
const int SEG_5_END = 1050;
const int LED_STRIP_BREAKS[] = {SEG_1_END, SEG_2_END, SEG_3_END, SEG_4_END, SEG_5_END, PIXEL_COUNT-1};
const uint32_t TEST_COLORS[] = {0x00FF00, 0x0000FF, 0xFF0000, 0xFF00FF, 0x00FFFF, 0xFFFF00};

const int TOP_SEGMENT_INSIDE_LENGTH = SEG_4_END-SEG_3_END;
const int TOP_SEGMENT_OUTSIDE_LENGTH = SEG_3_END-SEG_2_END;

const int INSIDE_LED_LEADER_COUNT = TOP_SEGMENT_INSIDE_LENGTH/LEADER_WIDTH;
const int OUTSIDE_LED_LEADER_COUNT = TOP_SEGMENT_OUTSIDE_LENGTH/LEADER_WIDTH;

int dopamineLeaderPos_outside[OUTSIDE_LED_LEADER_COUNT];
int dopamineLeaderPos_inside[INSIDE_LED_LEADER_COUNT];
int seratoninLeaderPos_outside[OUTSIDE_LED_LEADER_COUNT];
int seratoninLeaderPos_inside[INSIDE_LED_LEADER_COUNT];

int lastMillis = 0;
int currentMillis;
int testBrightness = 255;
int currentLED = 0;
float t;
float breatheBrightness;

const int FADE_LENGTH = 10;
//Array of colors in fade from bright to dim
int dopamineFade[FADE_LENGTH];
int seratoninFade[FADE_LENGTH];

byte redFromHex, greenFromHex, blueFromHex;

Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);
Button dopamineButton(DOP_BUTTON_PIN);
Button seratoninButton(SER_BUTTON_PIN);

//Functions
uint32_t blendColor(uint32_t color1, uint32_t color2, float ratio);
void segmentMarquee(uint32_t color, int origin, int min, int max, int leaderPositions[], int leaderCount);
void segmentFill(int startLED, int endLED, uint32_t fillColor);
void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed, int amplitude, int yOffset);
void hexToRGB(int colorIn, byte *redOut, byte *greenOut, byte *blueOut);
int rgbToHex(byte redIn, byte greenIn, byte blueIn);
void resetLEDLeaders(int leaderPositions[], int startLED, int endLED, int leaderCount, int origin, int color);
void showStripSegments();

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected, 10000);
    pixel.begin();
    pixel.setBrightness(MAX_BRIGHTNESS);

    // showStripSegments();

    resetLEDLeaders(dopamineLeaderPos_inside, SEG_3_END, SEG_4_END, INSIDE_LED_LEADER_COUNT, DOP_RECEPTOR_PIXELS[INSIDE], 0xFF0000);
    resetLEDLeaders(dopamineLeaderPos_outside, SEG_2_END, SEG_3_END, OUTSIDE_LED_LEADER_COUNT, DOP_RECEPTOR_PIXELS[OUTSIDE], 0x00FF00);
    resetLEDLeaders(seratoninLeaderPos_inside, SEG_3_END, SEG_4_END, INSIDE_LED_LEADER_COUNT, SER_RECEPTOR_PIXELS[INSIDE], 0xFFFF00);
    resetLEDLeaders(seratoninLeaderPos_outside, SEG_2_END, SEG_3_END, OUTSIDE_LED_LEADER_COUNT, SER_RECEPTOR_PIXELS[OUTSIDE], 0x0000FF);
    pixel.show();

    // while(!dopamineButton.isPressed()){
        // delay(1000);
    // }

    //Calculate fade values once in setup()
    for(int i=0; i<FADE_LENGTH; i++){
        float fadeRatio = pow((float)(FADE_LENGTH-i) / ((float)FADE_LENGTH), 3);
        dopamineFade[i] = blendColor(0x100500, DOPAMINE_COLOR, fadeRatio);
        seratoninFade[i] = blendColor(0x010A10, SERATONIN_COLOR, fadeRatio);
        Serial.printf("dopamineFade[%i]: 0x%06X\n", i, dopamineFade[i]);
        // Serial.printf("seratoninFade[%i]: 0x%06X\n", i, seratoninFade[i]);
    }

    pixel.clear();
    pixel.show();
}

void loop() {
    currentMillis = millis();
    pixel.clear();

    
    if(dopamineButton.isPressed()){
        segmentFill(SEG_2_END, SEG_4_END, dopamineFade[9]);
        segmentMarquee(DOPAMINE_COLOR, DOP_RECEPTOR_PIXELS[INSIDE], SEG_3_END, SEG_4_END, dopamineLeaderPos_inside, INSIDE_LED_LEADER_COUNT);
        segmentMarquee(DOPAMINE_COLOR, DOP_RECEPTOR_PIXELS[OUTSIDE], SEG_2_END, SEG_3_END, dopamineLeaderPos_outside, OUTSIDE_LED_LEADER_COUNT);
        segmentBreathe(SEG_5_END, PIXEL_COUNT, DOPAMINE_COLOR, 0.5, 127, 127);
    } else{
        segmentBreathe(SEG_5_END, PIXEL_COUNT, DOPAMINE_COLOR, 2.9, 127, 127);
    }
    
    if(seratoninButton.isPressed()){
        segmentFill(SEG_2_END, SEG_4_END, seratoninFade[9]);
        segmentMarquee(SERATONIN_COLOR, SER_RECEPTOR_PIXELS[INSIDE], SEG_3_END, SEG_4_END, seratoninLeaderPos_inside, INSIDE_LED_LEADER_COUNT);
        segmentMarquee(SERATONIN_COLOR, SER_RECEPTOR_PIXELS[OUTSIDE], SEG_2_END, SEG_3_END, seratoninLeaderPos_outside, OUTSIDE_LED_LEADER_COUNT);
        segmentBreathe(SEG_4_END, SEG_5_END, SERATONIN_COLOR, 0.6, 127, 127);
    } else{
        segmentBreathe(SEG_4_END, SEG_5_END, SERATONIN_COLOR, 3.0, 127, 127);
    }

    segmentBreathe(0, SEG_2_END, BASE_LED_COLOR, 3.1, 80, 127);

    pixel.show();
}

// Blends two colors based on the given ratio
uint32_t blendColor(uint32_t color1, uint32_t color2, float ratio) {
  uint8_t r1 = (color1 >> 16) & 0xFF;
  uint8_t g1 = (color1 >> 8) & 0xFF;
  uint8_t b1 = color1 & 0xFF;

  uint8_t r2 = (color2 >> 16) & 0xFF;
  uint8_t g2 = (color2 >> 8) & 0xFF;
  uint8_t b2 = color2 & 0xFF;

  uint8_t r = (float)r1 * (1.0 - ratio) + (float)r2 * ratio;
  uint8_t g = g1 * (1 - ratio) + g2 * ratio;
  uint8_t b = b1 * (1 - ratio) + b2 * ratio;

  return pixel.Color(r, g, b);
}

//Create a marquee effect on a segment of an LED strip with origin points
void segmentMarquee(uint32_t color, int origin, int min, int max, int leaderPositions[], int leaderCount){

    for(int i=0; i<leaderCount; i++){
        currentLED = leaderPositions[i];
        pixel.setPixelColor(currentLED, color);

        //Add head and tail fade from array
        for(int f=0; f<FADE_LENGTH; f++){
            if(color == DOPAMINE_COLOR){
                pixel.setPixelColor(currentLED+f, dopamineFade[f]);
                pixel.setPixelColor(currentLED-f, dopamineFade[f]);
            } else{
                pixel.setPixelColor(currentLED+f, seratoninFade[f]);
                pixel.setPixelColor(currentLED-f, seratoninFade[f]);
            }
        }

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
                leaderPositions[i] = origin;
            }
        }
    }
}

//Basic fill of a segment of the LED strip
void segmentFill(int startLED, int endLED, uint32_t fillColor){
    for(int i=startLED; i <= endLED; i++){
        pixel.setPixelColor(i, fillColor);
    }
}

//Breathe a full segment of an led strip
void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed, int amplitude, int yOffset){
    t = millis()/1000.0 + 0.5;
    breatheBrightness = amplitude*sin(M_PI*t/speed)+yOffset;

    hexToRGB(breatheColor, &redFromHex, &greenFromHex, &blueFromHex);

    for(int i = startLED; i <= endLED; i++){
        pixel.setColorScaled(i, redFromHex, greenFromHex, blueFromHex, breatheBrightness);
    }
}

//Split one hex color value into its component red, green, blue pieces
void hexToRGB(int colorIn, byte *redOut, byte *greenOut, byte *blueOut){
    *blueOut = colorIn & 0xFF;
    *redOut = colorIn>>16;
    *greenOut = (colorIn>>8) & 0xFF;
}

//Convert a set of red, green, blue values into one color value
int rgbToHex(byte redIn, byte greenIn, byte blueIn){
    return (redIn<<16) | (greenIn<<8) | blueIn;
}

//Initialize/reset the location of LED leaders
void resetLEDLeaders(int leaderPositions[], int startLED, int endLED, int leaderCount, int origin, int color){
    int originOffset = (origin-startLED)%20-10;     //find the offset that centers leaders around the origin
    int segmentLength = endLED-startLED;

    for(int h=0; h<leaderCount; h++){
        leaderPositions[h] = h*(LEADER_WIDTH) + startLED+originOffset;
        if(leaderPositions[h]<startLED) leaderPositions[h]+=(segmentLength);        //Wrap leaders around to the end if they're below the startLED
        //Uncomment to see leader starting positions
        // pixel.setPixelColor(leaderPositions[h], color);
    }
}

//Light up the different segments of the strip
void showStripSegments(){
    for (int i=0; i<6; i++){
        if(i-1>=0){
            segmentFill(LED_STRIP_BREAKS[i-1], LED_STRIP_BREAKS[i], TEST_COLORS[i]);
        } else{
            segmentFill(0, LED_STRIP_BREAKS[i], TEST_COLORS[i]);
        }
    }
    pixel.setPixelColor(SER_RECEPTOR_PIXELS[0], 0XFFFFFF);
    pixel.setPixelColor(DOP_RECEPTOR_PIXELS[0], 0xFFFFFF);
    pixel.setPixelColor(SER_RECEPTOR_PIXELS[1], 0XFFFFFF);
    pixel.setPixelColor(DOP_RECEPTOR_PIXELS[1], 0xFFFFFF);
}