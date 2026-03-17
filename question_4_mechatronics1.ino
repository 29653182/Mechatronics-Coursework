#include <Servo.h>
#include <LiquidCrystal.h>
 
// --- Pin Definitions ---
#define TRIG_PIN     9
#define ECHO_PIN     10
#define SERVO_PIN    6
#define POT_PIN      A0
 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo barrierServo;
 
bool barrierOpen = false;
 
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}
 
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  barrierServo.attach(SERVO_PIN);
  barrierServo.write(0);  // closed at 3 o'clock
 
  lcd.begin(16, 2);
  lcd.print("Car Park System");
  delay(1500);
  lcd.clear();
}
 
void loop() {
  int potValue = analogRead(POT_PIN);
  int setRange = map(potValue, 0, 1023, 10, 150);
 
  long distance = measureDistance();
 
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  if (distance >= 999) {
    lcd.print("---   ");
  } else {
    lcd.print(distance);
    lcd.print("cm    ");
  }
 
  lcd.setCursor(0, 1);
  lcd.print("Range:");
  lcd.print(setRange);
  lcd.print("cm    ");
 
  if (distance <= setRange && distance < 999) {
    if (!barrierOpen) {
      barrierServo.write(90);   // open — lifts to 12 o'clock
      barrierOpen = true;
    }
  } else {
    if (barrierOpen) {
      barrierServo.write(0);  // close — drops to 3 o'clock
      barrierOpen = false;
    }
  }
 
  delay(200);
}