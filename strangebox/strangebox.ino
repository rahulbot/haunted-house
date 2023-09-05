#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "SparkFun_AK975X_Arduino_Library.h"

#define PIN_NEO_PIXEL 8
#define NUM_PIXELS    28

#define DELAY_INTERVAL 100


AK975X movementSensor;
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);
int ir1, ir2, ir3, ir4, avg, temperature;

int ir_threshold = 800; // above this number the machine will trigger

void setup() {
  Serial.begin(9600);
  NeoPixel.begin();
  Wire.begin();
  //Turn on sensor
  if (movementSensor.begin() == false) {
    Serial.println("Device not found. Check wiring.");
    while (1);
  }
  Serial.println("Starting ------------------------------------------------------------");
  Serial.println("\tInit:");
  // TODO: grab a set of IR readings to set the average background level
  int background = get_background_ir_level(5);
  Serial.print("\tIR background ");
  Serial.println(background);
  ir_threshold = background + 100;
  Serial.print("\tIR treshold ");
  Serial.println(ir_threshold);
}

int get_background_ir_level(int num_readings) {
  int background_level = 0;
  int actual_readings = 0;
  for( int i=0; i<num_readings; i++) {
    delay(100);
    int curr = get_avg_ir_reading();
    Serial.print("\t\tIR background reading: ");
    Serial.println(curr);
    if (curr != 0) {
      background_level += curr;
      actual_readings += 1;
    }
  }
  return background_level / actual_readings;
} 

int get_avg_ir_reading() {
  int avg_reading = 0;
  if (movementSensor.available()) {
    avg_reading += movementSensor.getIR1() / 10;
    avg_reading += movementSensor.getIR2() / 10;
    avg_reading += movementSensor.getIR3() / 10;
    avg_reading += movementSensor.getIR4() / 10;
    avg_reading = avg_reading / 4;
    movementSensor.refresh();
  }
  return avg_reading;
}

void loop() {
  NeoPixel.clear();
  NeoPixel.show();
  avg = get_avg_ir_reading();
  if (avg > ir_threshold) {
    Serial.print("boo! ");
    Serial.println(avg);
    for( int pixel=0; pixel < NUM_PIXELS; pixel++) {
      NeoPixel.setPixelColor(pixel, NeoPixel.Color(50, 0, 0));
      NeoPixel.show();
      delay(100);
    }
  }
}
