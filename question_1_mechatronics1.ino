//  Pin Definitions 
const int TRAFFIC_RED    = 2;
const int TRAFFIC_YELLOW = 3;
const int TRAFFIC_GREEN  = 4;

const int PED_RED        = 5;
const int PED_GREEN      = 6;

const int BUTTON_PIN     = 7;
const int BUZZER_PIN     = 8;

// 7-Segment pins: A, B, C, D, E, F, G
const int SEG[7] = {9, 10, 11, 12, 13, A0, A1};

//  7-Segment Digit Encoding 
// Segments: A B C D E F G
// Digit patterns for 0–9
const byte DIGITS[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}, // 9
};

//  Function: Display digit on 7-segment 
void showDigit(int num) {
  if (num < 0 || num > 9) {
    for (int i = 0; i < 7; i++) digitalWrite(SEG[i], LOW);
    return;
  }
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEG[i], DIGITS[num][i]);
  }
}

//  Function: Clear 7-segment 
void clearDisplay() {
  for (int i = 0; i < 7; i++) digitalWrite(SEG[i], LOW);
}

//  Function: Buzzer beep 
void beep(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

//  Function: Set traffic light 
void setTrafficLight(bool red, bool yellow, bool green) {
  digitalWrite(TRAFFIC_RED,    red    ? HIGH : LOW);
  digitalWrite(TRAFFIC_YELLOW, yellow ? HIGH : LOW);
  digitalWrite(TRAFFIC_GREEN,  green  ? HIGH : LOW);
}

//  Function: Set pedestrian light 
void setPedLight(bool red, bool green) {
  digitalWrite(PED_RED,   red   ? HIGH : LOW);
  digitalWrite(PED_GREEN, green ? HIGH : LOW);
}

//  Setup 
void setup() {
  pinMode(TRAFFIC_RED,    OUTPUT);
  pinMode(TRAFFIC_YELLOW, OUTPUT);
  pinMode(TRAFFIC_GREEN,  OUTPUT);

  pinMode(PED_RED,   OUTPUT);
  pinMode(PED_GREEN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  for (int i = 0; i < 7; i++) {
    pinMode(SEG[i], OUTPUT);
  }

  // Normal starting state: Traffic GREEN, Pedestrian RED
  setTrafficLight(false, false, true);
  setPedLight(true, false);
  clearDisplay();

  Serial.begin(9600);
  Serial.println("System ready. Press pedestrian button.");
}

//  Main Loop
void loop() {
  // Poll button (active LOW due to INPUT_PULLUP)
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); 
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button pressed! Starting pedestrian crossing sequence.");
      pedestrianCrossingSequence();
      while (digitalRead(BUTTON_PIN) == LOW) delay(10);
    }
  }
}

//  Pedestrian Crossing Sequence 
void pedestrianCrossingSequence() {

  // PHASE 1: Traffic Yellow (3 second warning)
  setTrafficLight(false, true, false);
  setPedLight(true, false);
  clearDisplay();
  beep(800, 200);
  delay(3000);

  // PHASE 2: Traffic Red + Pedestrian Green
  setTrafficLight(true, false, false);
  setPedLight(false, true);
  beep(1000, 500);
  delay(500);

  // PHASE 3: Countdown from 9 to 0
  for (int count = 9; count >= 0; count--) {
    showDigit(count);

    if (count > 3) {
      // Normal crossing: one beep per second
      beep(1000, 150);
      delay(1000);
    } else {
      // Urgent warning: fast double-beep + flashing green
      for (int half = 0; half < 2; half++) {
        beep(1200, 100);
        delay(200);
        beep(1200, 100);
        delay(200);
      }
      delay(200);

      if (count > 0) {
        setPedLight(false, false); // Flash OFF
        delay(150);
        setPedLight(false, true);  // Flash ON
      }
    }
  }

  // PHASE 4: Pedestrian Red + stop buzzer
  setPedLight(true, false);
  clearDisplay();
  beep(600, 800);
  delay(1000);

  // PHASE 5: Traffic Yellow (transition back)
  setTrafficLight(false, true, false);
  delay(2000);

  // PHASE 6: Traffic Green resumes
  setTrafficLight(false, false, true);
  clearDisplay();
  Serial.println("Crossing complete. Traffic resumed.");
}
