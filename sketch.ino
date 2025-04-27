#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Create Servo and LCD objects
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define Pins
const int trigPin = 9;
const int echoPin = 10;
const int pirPin = 7;
const int gasSensorPin = A0;
const int greenLed = 2;
const int redLed = 3;
const int buzzer = 4;
const int servoPin = 5;

// Variables
long duration;
int distance;
bool lidOpen = false;
int gasValue = 0;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(0); // Close lid initially

  lcd.init();
  lcd.backlight();
  
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(buzzer, LOW);
}

void loop() {
  int pirState = digitalRead(pirPin);
  
  distance = getDistance();
  gasValue = analogRead(gasSensorPin);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Gas: ");
  Serial.println(gasValue);

  lcd.setCursor(0, 0);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm ");

  lcd.setCursor(0, 1);
  
  // Gas detection behavior
  if (gasValue < 400) {
    lcd.print("Gas: Safe    ");
    digitalWrite(buzzer, LOW);
    digitalWrite(redLed, LOW);
  } else if (gasValue >= 400 && gasValue <= 600) {
    lcd.print("Gas: Warning ");
    digitalWrite(buzzer, LOW);
    digitalWrite(redLed, HIGH); // only LED ON
  } else { // gasValue > 600
    lcd.print("Gas: Danger! ");
    digitalWrite(buzzer, HIGH); // buzzer + red LED
    digitalWrite(redLed, HIGH);
  }

  // Smart Lid Control
  if (pirState == HIGH) {
    if (!lidOpen) {
      myServo.write(90); // Open lid
      lidOpen = true;
      delay(3000); // Keep open for 3 sec
    }
  } else {
    if (lidOpen) {
      myServo.write(0); // Close lid
      lidOpen = false;
    }
  }

  // Dustbin Full Check
  if (distance < 10) {
    digitalWrite(redLed, HIGH);
    digitalWrite(buzzer, HIGH);
    digitalWrite(greenLed, LOW);
  } else if (gasValue < 400) { // Only show Green LED if no gas alert
    digitalWrite(greenLed, HIGH);
  }

  delay(500);
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  
  long dist = duration * 0.034 / 2;
  return dist;
}
