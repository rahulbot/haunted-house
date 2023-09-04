#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "SparkFun_AK975X_Arduino_Library.h"

#define PIN_NEO_PIXEL 8
#define NUM_PIXELS    28

#define DELAY_INTERVAL 100


AK975X movementSensor;
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);
int ir1, ir2, ir3, ir4, avg, temperature;

void setup() {
  Serial.begin(9600);
  NeoPixel.begin();
  Wire.begin();
  //Turn on sensor
  if (movementSensor.begin() == false)
  {
    Serial.println("Device not found. Check wiring.");
    while (1);
  }
  Serial.println("Starting ------------------------------------------------------------");
}

void loop() {
  NeoPixel.clear();
  NeoPixel.show();
  if (movementSensor.available())
  {
    ir1 = movementSensor.getIR1() / 10;
    ir2 = movementSensor.getIR2() / 10;
    ir3 = movementSensor.getIR3() / 10;
    ir4 = movementSensor.getIR4() / 10;
    avg = (ir1 + ir2 + ir3 + ir4) / 4;
    float tempF = movementSensor.getTemperatureF();
    //Serial.print("1:");
    //Serial.print(avg);
    //Serial.println();
    if (avg > 800) {
      Serial.print("boo! ");
      Serial.println(avg);
      for( int pixel=0; pixel < NUM_PIXELS; pixel++) {
        NeoPixel.setPixelColor(pixel, NeoPixel.Color(50, 0, 0));
        NeoPixel.show();
        delay(100);
      }
    }
    movementSensor.refresh(); //Read dummy register after new data is read

    //Note: The observable area is shown in the silkscreen.
    //If sensor 2 increases first, the human is on the left
/*    Serial.print("1:DWN[");
    Serial.print(ir1);
    Serial.print("]\t2:LFT[");
    Serial.print(ir2);
    Serial.print("]\t3:UP[");
    Serial.print(ir3);
    Serial.print("]\t4:RGH[");
    Serial.print(ir4);
    Serial.print("]\ttempF[");
    Serial.print(tempF);
    Serial.print("]\tmillis[");
    Serial.print(millis());
    Serial.print("]");
    Serial.println();*/
  }

}