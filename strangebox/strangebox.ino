#include <Adafruit_NeoPixel.h>

#define PIN_NEO_PIXEL 8
#define NUM_PIXELS    28

#define DELAY_INTERVAL 100

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

void setup() {
  NeoPixel.begin();
}

void loop() {
  NeoPixel.clear();
  for( int pixel=0; pixel < NUM_PIXELS; pixel++) {
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(255, 50, 50));
    NeoPixel.show();
    delay(DELAY_INTERVAL);
  }
}