#include <LiquidCrystal.h>

// LCD pin mapping: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int ldrPin = A5;      // Photoresistor on A0
int ledPin = 13;      // LED on pin 13

void setup() {
  lcd.begin(16, 2);     // Initialise LCD
  pinMode(ledPin, OUTPUT);
  lcd.clear();
}

void loop() {
  int lightVal = analogRead(ldrPin);  // Read the LDR (0–1023)

  
 lightVal = analogRead(ldrPin); 
lightVal = map(lightVal, 10, 1000, 0, 100); // Changes Range to 0-100
  
 
  // Display light level
  lcd.setCursor(0, 0);
  lcd.print("Light: ");
  lcd.print(lightVal);
  lcd.print("    "); // Clear leftover characters

  // LED control logic (bright = LED off, dark = LED on)
  if (lightVal < 80) {
    digitalWrite(ledPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("LED: ON ");
  } else {
    digitalWrite(ledPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("LED: OFF");
  }

  delay(200);
}