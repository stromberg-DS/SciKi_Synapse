/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <neopixel.h>


SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);


// LED strip configuration
#define LED_PIN    6       // Pin where the LED strip is connected
const int PIXEL_COUNT = 200;      // Total number of LEDs on the strip

Adafruit_NeoPixel pixel(PIXEL_COUNT, SPI1, WS2812);

// Configuration for the wave effect
int waveWidth = 10;        // Width of the wave (number of LEDs)
int waveSpeed = 2;         // Speed of the wave
int peakColor = pixel.Color(255, 0, 0);  // Color of the wave peak (red)
int troughColor = pixel.Color(0, 0, 255); // Color of the trough (blue)

int positions[] = {50, 100, 150};  // Positions to which the waves move

uint32_t blendColor(uint32_t color1, uint32_t color2, float ratio);

void setup() {
  pixel.begin();
  pixel.show();  // Initialize all pixels to 'off'
}

void loop() {
  // Clear the strip
  pixel.clear();

  for (int i = 0; i < sizeof(positions) / sizeof(positions[0]); i++) {
    int position = positions[i];
    int nextPosition = positions[(i + 1) % (sizeof(positions) / sizeof(positions[0]))];
    
    // Calculate the midpoint between this position and the next
    int midpoint = (position + nextPosition) / 2;

    // Generate waves moving towards the midpoint from both directions
    for (int j = -waveWidth; j <= waveWidth; j++) {
      int forwardIndex = position + j;
      int backwardIndex = position - j;

      // Ensure forwardIndex and backwardIndex are within bounds
      forwardIndex = (forwardIndex + PIXEL_COUNT) % PIXEL_COUNT;
      backwardIndex = (backwardIndex + PIXEL_COUNT) % PIXEL_COUNT;

      // Only draw within the bounds from position to midpoint
      if ((forwardIndex <= midpoint && position <= midpoint) || (forwardIndex >= midpoint && position >= midpoint)) {
        float brightness = 1.0 - abs(j) / (float)waveWidth;
        uint32_t color = blendColor(troughColor, peakColor, brightness);
        pixel.setPixelColor(forwardIndex, color);
      }
      
      if ((backwardIndex >= midpoint && position >= midpoint) || (backwardIndex <= midpoint && position <= midpoint)) {
        float brightness = 1.0 - abs(j) / (float)waveWidth;
        uint32_t color = blendColor(troughColor, peakColor, brightness);
        pixel.setPixelColor(backwardIndex, color);
      }
    }
  }

  pixel.show();
  delay(50);  // Adjust to control the speed of the wave animation

  // Move the wave positions
  for (int i = 0; i < sizeof(positions) / sizeof(positions[0]); i++) {
    positions[i] = (positions[i] + waveSpeed) % PIXEL_COUNT;
  }
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
