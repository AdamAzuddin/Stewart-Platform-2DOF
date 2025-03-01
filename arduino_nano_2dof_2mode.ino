#include <Servo.h>
#include <math.h>  // Needed for cos()
#define PI 3.14159265358979323846

Servo servo1;  // X-axis servo
Servo servo2;  // Y-axis servo

const int MODE_PIN = 11;  // Pin for mode selection
const int speed = 20; // in m/s
int bankingAngles[250];  // Banking angles array
int yComponentAngles[250];  // Y-component angles array

void setup() {
    pinMode(MODE_PIN, INPUT_PULLUP); // Enable internal pull-up (default HIGH if no jumper)
    Serial.begin(9600);

    servo1.attach(3);
    servo2.attach(5);
    servo1.write(90);
    servo2.write(90);

    const float R = 28.65; // Radius of the curved sections

    // Initialize bankingAngles and yComponentAngles
    for (int i = 0; i < 250; i++) {
        int x = i + 1; // Position on the track

        if (x < 35) {
            bankingAngles[i] = 12;
            yComponentAngles[i] = 90;
        } 
        else if (x < 55) {
            bankingAngles[i] = round(12 + 16.5 * (1 - cos(PI * (x - 35) / 20)));
            yComponentAngles[i] = 90 + round(R * (1 - cos(PI * (x - 35) / 20)));
        } 
        else if (x < 105) {
            bankingAngles[i] = 45;
            yComponentAngles[i] = 90 + round(R * (1 - cos(PI * (x - 35) / 90)));
        } 
        else if (x < 125) {
            bankingAngles[i] = round(45 - 16.5 * (1 - cos(PI * (x - 105) / 20)));
            yComponentAngles[i] = 90 + round(R * (1 - cos(PI * (x - 105) / 20)));
        } 
        else if (x < 160) {
            bankingAngles[i] = 12;
            yComponentAngles[i] = 90;
        } 
        else if (x < 180) {
            bankingAngles[i] = round(12 + 16.5 * (1 - cos(PI * (x - 160) / 20)));
            yComponentAngles[i] = 90 + round(R * (1 - cos(PI * (x - 160) / 20)));
        } 
        else if (x < 230) {
            bankingAngles[i] = 45;
            yComponentAngles[i] = 90 + round(R * (1 - cos(PI * (x - 125) / 90)));
        } 
        else {
            bankingAngles[i] = 45;
            yComponentAngles[i] = 90 + round(R * (1 - cos(PI * (x - 230) / 20)));
        }
    }
}

void loop() {
    if (digitalRead(MODE_PIN) == LOW) {
        joystickMode();
    } else {
        normalMode();
    }
}

// ----------------- Normal Mode Function -----------------
void normalMode() {
    Serial.println("Normal Mode Active");
    for (int i = 0; i < 250; i++) { 
        servo1.write(bankingAngles[i]);
        int servo2Angle = constrain(yComponentAngles[i], 0, 180);
        servo2.write(servo2Angle);
        
        delay(1000 / speed);
        
        if (i == 249) {
            servo1.write(90);
            servo2.write(90);
        }
        
        // Check if mode changed during execution
        if (digitalRead(MODE_PIN) == LOW) return;
    }
}

// ----------------- Joystick Mode Function -----------------
void joystickMode() {
    Serial.println("Joystick Mode Active");
    while (digitalRead(MODE_PIN) == LOW) {
        int xValue = analogRead(A0);
        int yValue = analogRead(A1);

        int xAngle = map(xValue, 0, 1023, 0, 180);
        int yAngle = map(yValue, 0, 1023, 0, 180);

        servo1.write(xAngle);
        servo2.write(yAngle);

        Serial.print("X: ");
        Serial.print(xValue);
        Serial.print(" -> ServoX: ");
        Serial.print(xAngle);
        Serial.print(" | Y: ");
        Serial.print(yValue);
        Serial.print(" -> ServoY: ");
        Serial.println(yAngle);

        delay(50);
    }
}
