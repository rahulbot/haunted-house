#define MOVE_PIN 2
#define GONG_1 8
#define GONG_2 9
#define GONG_3 10
#define GONG_4 11
#define GONG_5 12

#define G1 GONG_1
#define G2 GONG_2
#define G3 GONG_3
#define G4 GONG_4

#define HIT_STROKE  80
#define EIGHT_NOTE  300

int lastTrigger = -60;

void setup() {
  pinMode(GONG_1, OUTPUT);
  pinMode(GONG_2, OUTPUT);
  pinMode(GONG_3, OUTPUT);
  pinMode(GONG_4, OUTPUT);
  pinMode(GONG_5, OUTPUT);
  pinMode(MOVE_PIN, INPUT);
  //pinMode(MOVE_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int currentSecs = millis() / 1000;
  int movement = digitalRead(MOVE_PIN);
  Serial.print(currentSecs);
  Serial.print('\t');
  Serial.print(lastTrigger);
  Serial.print('\t');
  Serial.println(movement);
  if(movement && ((currentSecs - lastTrigger) > 15)) {
    lastTrigger = currentSecs;
    Serial.println("******************************************");
    Serial.println("******************************************");
    Serial.println("******************************************");
    Serial.println("******************************************");
    hit(G1);
    delay(EIGHT_NOTE*2);
    hit(G2);
    delay(EIGHT_NOTE*2);
    hit(G3);
    delay(EIGHT_NOTE*2);
    hit(G4);
    delay(EIGHT_NOTE*2);
  }
  delay(100);
}

void hit_two(int pin1, int pin2) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  delay(HIT_STROKE);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  delay(EIGHT_NOTE - HIT_STROKE);
}

void hit(int pin1) {
  digitalWrite(pin1, HIGH);
  delay(HIT_STROKE);
  digitalWrite(pin1, LOW);
  delay(EIGHT_NOTE - HIT_STROKE);
}