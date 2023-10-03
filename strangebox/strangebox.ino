#include <FastLED.h>
#include <Wire.h>
#include "SparkFun_AK975X_Arduino_Library.h"
#include <SoftwareSerial.h>

// LED Strip variables
#define PIN_NEO_PIXEL 8
#define NUM_PIXELS    28
void set_strip(byte red, byte green, byte blue);
// my wiring diagram ref: https://docs.google.com/presentation/d/1n-prYIjxbP88oVD0NHh5rnjuX89-BTGiBmRFsXlhnIc/edit?pli=1#slide=id.g208fc40b77c_0_17
CRGB leds[NUM_PIXELS];

// MP3 Player variables
#define ARDUINO_RX 10 // MP3 TX
#define ARDUINO_TX 11 // MP3 RX
#define CMD_NEXT_SONG     0X01  // Play next song.
#define CMD_PREV_SONG     0X02  // Play previous song.
#define CMD_PLAY_W_INDEX  0X03
#define CMD_VOLUME_UP     0X04
#define CMD_VOLUME_DOWN   0X05
#define CMD_SET_VOLUME    0X06
#define CMD_SNG_CYCL_PLAY 0X08  // Single Cycle Play.
#define CMD_SEL_DEV       0X09
#define CMD_SLEEP_MODE    0X0A
#define CMD_WAKE_UP       0X0B
#define CMD_RESET         0X0C
#define CMD_PLAY          0X0D
#define CMD_PAUSE         0X0E
#define CMD_PLAY_FOLDER_FILE 0X0F
#define CMD_STOP_PLAY     0X16  // Stop playing continuously. 
#define CMD_FOLDER_CYCLE  0X17
#define CMD_SHUFFLE_PLAY  0x18 //
#define CMD_SET_SNGL_CYCL 0X19 // Set single cycle.
#define CMD_SET_DAC 0X1A
#define DAC_ON  0X00
#define DAC_OFF 0X01
#define CMD_PLAY_W_VOL    0X22
#define CMD_PLAYING_N     0x4C
#define CMD_QUERY_STATUS      0x42
#define CMD_QUERY_VOLUME      0x43
#define CMD_QUERY_FLDR_TRACKS 0x4e
#define CMD_QUERY_TOT_TRACKS  0x48
#define CMD_QUERY_FLDR_COUNT  0x4f
SoftwareSerial mp3(ARDUINO_RX, ARDUINO_TX);
String sanswer(void);
String sbyte2hex(uint8_t b);
static int8_t Send_buf[8] = {0}; // Buffer for Send commands.  // BETTER LOCALLY
static uint8_t ansbuf[10] = {0}; // Buffer for the answers.    // BETTER LOCALLY
String mp3Answer;           // Answer from the MP3.
String decodeMP3Answer(void);

// IR Sensor variables
AK975X movementSensor;
int get_avg_ir_reading();
int get_background_ir_level(int num_readings);
int ir1, ir2, ir3, ir4, avg, temperature;
int ir_threshold = 800; // above this number the machine will trigger; later set dynamically

// Behaviour methods
int a_while_since_last_trigger();
void handle_trigger_event();

/***************************************************************
 * Control Functions
 ***************************************************************/

void setup() {
  randomSeed(analogRead(5));
  Serial.begin(9600);
  Wire.begin();
  // setup LEDs
  FastLED.addLeds<WS2812,PIN_NEO_PIXEL,GRB>(leds, NUM_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  set_strip(161, 52, 235);
  FastLED.show();
  //Turn on sensor
  if (movementSensor.begin() == false) {
    Serial.println("Device not found. Check wiring.");
    while (1);
  }
  Serial.println("Setup ------------------------------------------------------------");
  Serial.println("Init:");
  // TODO: grab a set of IR readings to set the average background level
  int background = get_background_ir_level(5);
  Serial.print("\tIR background ");
  Serial.println(background);
  ir_threshold = background + 150;
  Serial.print("\tIR treshold ");
  Serial.println(ir_threshold);
  // turn on MP3 Player
  mp3.begin(9600);
  delay(50);
  //sendShortCommand(CMD_QUERY_STATUS);
  //sendLongCommand(CMD_PLAY_W_INDEX, 0, 3);
  Serial.println("Loop ------------------------------------------------------------");
}

void loop() {
  // check for MP3 board reply?
  // Check for the answer.
  if (mp3.available()) {
    Serial.println(decodeMP3Answer());
  }
  // trigger event if above threshold
  avg = get_avg_ir_reading();
  if ((avg > ir_threshold) && (a_while_since_last_trigger())){
    handle_trigger_event();
  }
}

/***************************************************************
 * IR Sensor COMMANDS
 ***************************************************************/

void handle_trigger_event(){
  Serial.print("¡Trigger! ");
  Serial.println(avg);
  //start sound on MP3 player
  sendLongCommand(CMD_PLAY_W_INDEX, 0, 1); // last arg is track number
  for (int r=0; r<2; r++) {
    for (int b=0; b<random(200, 255); b+=5) {
      FastLED.setBrightness(b);
      FastLED.show();
      delay(random(15,30));
    }
    for (int b=random(200, 255); b>0; b-=5) {
      FastLED.setBrightness(b);
      FastLED.show();
      delay(random(15,30));
    }
  }
  FastLED.setBrightness(0);
  FastLED.show();
}

int a_while_since_last_trigger(){
  return 1; // TODO: keep track of time so it doesn't always trigger?
}

/***************************************************************
 * IR Sensor COMMANDS
 ***************************************************************/

int get_background_ir_level(int num_readings) {
  int background_level = 0;
  int actual_readings = 0;
  for( int i=0; i<num_readings; i++) {
    delay(100);
    int curr = get_avg_ir_reading();
    //Serial.print("\t\tIR background reading: ");
    //Serial.println(curr);
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


/***************************************************************
 * LED Strip COMMANDS
 ***************************************************************/

void set_strip(byte red, byte green, byte blue) {
  for (int p; p < NUM_PIXELS; p++) {
    leds[p].r = red;
    leds[p].g = green;
    leds[p].b = blue;
  }
}

/***************************************************************
 * MP3 COMMANDS
 ***************************************************************/

void sendShortCommand(byte command){
  sendLongCommand(command, 0, 0);
}
void sendLongCommand(byte command, byte dat1, byte dat2){
  delay(20);
  Send_buf[0] = 0x7E;    //
  Send_buf[1] = 0xFF;    //
  Send_buf[2] = 0x06;    // Len
  Send_buf[3] = command; //
  Send_buf[4] = 0x01;    // 0x00 NO, 0x01 feedback
  Send_buf[5] = dat1;    // datah
  Send_buf[6] = dat2;    // datal
  Send_buf[7] = 0xEF;    //
  Serial.print("MP3 Send: ");
  for (uint8_t i = 0; i < 8; i++){
    mp3.write(Send_buf[i]) ;
    Serial.print(sbyte2hex(Send_buf[i]));
  }
  Serial.println();
}
String decodeMP3Answer() {
  String decodedMP3Answer = "";
  decodedMP3Answer += sanswer();
  switch (ansbuf[3]) {
  case 0x3A:
        decodedMP3Answer += "MP3 Memory card inserted.";
  break;
  case 0x3D:
        decodedMP3Answer += "MP3 Completed play num " + String(ansbuf[6], DEC);
  break;
  case 0x40:
        decodedMP3Answer += "MP3 Error";
  break;
  case 0x41:
        decodedMP3Answer += "MP3 Data recived correctly. ";
  break;
  case 0x42:
        decodedMP3Answer += "MP3 Status playing: " + String(ansbuf[6], DEC);
  break;
  case 0x48:
        decodedMP3Answer += "MP3 File count: " + String(ansbuf[6], DEC);
  break;
  case 0x4C:
        decodedMP3Answer += "MP3 Playing: " + String(ansbuf[6], DEC);
  break;
  case 0x4E:
        decodedMP3Answer += "MP3 Folder file count: " + String(ansbuf[6], DEC);
  break;
  case 0x4F:
        decodedMP3Answer += "MP3 Folder count: " + String(ansbuf[6], DEC);
  break;
    }
  return decodedMP3Answer;
}
String sanswer(void) {
  uint8_t i = 0;
  String mp3answer = "";
  // Get only 10 Bytes
  while (mp3.available() && (i < 10)){
  uint8_t b = mp3.read();
    ansbuf[i] = b;
    i++;
    mp3answer += sbyte2hex(b);
  }
// if the answer format is correct.
  if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF)) {
    return mp3answer;
  }
  return "???: " + mp3answer;
}
String sbyte2hex(uint8_t b){
  String shex;
  shex = "0X";
  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}
