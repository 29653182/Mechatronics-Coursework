#include <Servo.h>
#include <LiquidCrystal.h>

//  Pin Definitions 
const int SERVO_PIN  = 9;
const int BUTTON_PIN = 2;
const int POT_PIN    = A0;
const int LED_PIN    = 13;   

//  LCD: RS, EN, D4, D5, D6, D7 
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

//  Objects & State 
Servo myServo;

bool isOn = false;

// Debounce tracking
bool lastRawBtn = HIGH;     // raw pin reading last loop
bool lastStableBtn = HIGH;  // last *stable* (debounced) state
unsigned long lastDebounceTime = 0;
const int DEBOUNCE_DELAY = 50;

int servoPos = 0;
int servoDir = 1;

int lastSpeedPct = -1;
bool lastIsOn = false;

//  Custom LCD Characters 
byte arrowUp[8]   = {0b00100,0b01110,0b11111,0b00100,0b00100,0b00100,0b00100,0b00000};
byte arrowDown[8] = {0b00100,0b00100,0b00100,0b00100,0b11111,0b01110,0b00100,0b00000};

// setup
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);            
  digitalWrite(LED_PIN, LOW);          // start OFF

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  lcd.begin(16, 2);
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);

  lcd.setCursor(2, 0);
  lcd.print("SERVO CTRL v1");
  lcd.setCursor(3, 1);
  lcd.print("Press START");
  delay(2000);
  lcd.clear();

  updateLCD(0, false, true);
}

// Main Loop
void loop() {
  // BUTTON HANDLING (DEBOUNCED)

  bool raw = digitalRead(BUTTON_PIN);

  // If raw state changed, restart debounce timer
  if (raw != lastRawBtn) {
    lastDebounceTime = millis();
  }

  // If raw state stable long enough, accept it
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {

    // If stable state actually changed:
    if (raw != lastStableBtn) {

      // (button pressed)
      if (raw == LOW) {
        isOn = !isOn;

        // immediately reflect LED state on toggle
        digitalWrite(LED_PIN, isOn ? HIGH : LOW);

        if (!isOn) {
          myServo.write(0);
          servoPos = 0;
          servoDir = 1;
        }
      }

      lastStableBtn = raw; // update stable state
    }
  }

  lastRawBtn = raw;
  // READ POTENTIOMETER SPEED  

  int potVal   = analogRead(POT_PIN);
  int speedPct = map(potVal, 0, 1023, 0, 100);
  int moveDelay = map(speedPct, 0, 100, 80, 5);

  // UPDATE LCD WHEN SOMETHING CHANGES


  bool changed = (speedPct != lastSpeedPct) || (isOn != lastIsOn);
  if (changed) {
    updateLCD(speedPct, isOn, false);
    lastSpeedPct = speedPct;
    lastIsOn = isOn;
  }

  // Keep LED synced 
  digitalWrite(LED_PIN, isOn ? HIGH : LOW);

  //  MOVE SERVO WHEN SYSTEM IS ON    

  if (isOn) {
    servoPos += servoDir;

    if (servoPos >= 180) {
      servoPos = 180;
      servoDir = -1;
    } else if (servoPos <= 0) {
      servoPos = 0;
      servoDir = 1;
    }

    myServo.write(servoPos);
    delay(moveDelay);

  } else {
    delay(50);
  }
}


//     LCD DRAWING FUNCTION

void updateLCD(int pct, bool on) {

  lcd.setCursor(0, 0);
  if (on) {
    lcd.write(byte(0));
    lcd.print(" STATUS: ON  ");
  } else {
    lcd.print("  STATUS: OFF ");
  }

  lcd.setCursor(0, 1);
  lcd.print("SPD:");

  if (pct < 10) lcd.print("  ");
  else if (pct < 100) lcd.print(" ");
  lcd.print(pct);
  lcd.print("% ");

  int filled = map(pct, 0, 100, 0, 7);
  lcd.print("[");
  for (int i = 0; i < 7; i++) {
    lcd.print(i < filled ? (char)255 : ' ');
  }
  lcd.print("]");
}
