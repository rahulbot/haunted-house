#define GONG_1 8
#define GONG_2 9
#define GONG_3 10
#define GONG_4 11
#define GONG_5 12

#define F1 GONG_1
#define F2 GONG_4
#define D1 GONG_2
#define D2 GONG_3

#define HIT_STROKE  80
#define EIGHT_NOTE  300

void setup() {
  pinMode(GONG_1, OUTPUT);
  pinMode(GONG_2, OUTPUT);
  pinMode(GONG_3, OUTPUT);
  pinMode(GONG_4, OUTPUT);
  pinMode(GONG_5, OUTPUT);
}

void loop() {
  hit_two(F1, D1);
  delay(EIGHT_NOTE);
  hit(F2);
  hit(D2);
  hit_two(F1, D1);
  delay(EIGHT_NOTE);
  hit(F2);
  hit(D2);
  delay(EIGHT_NOTE*8);
  
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