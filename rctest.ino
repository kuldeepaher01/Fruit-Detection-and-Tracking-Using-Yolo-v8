#include <Servo.h>
// Define Input Connections
#define CH1 13
#define CH2 12
#define CH3 11
#define CH4 10
#define CH5 9
#define CH6 5
#define cam_servo 8
#define arm_servo 7
#define base_servo 6

// Integers to represent values from sticks and pots
int ch1Value;  //ch3 and ch1
int ch2Value;  //back or forward
int ch3Value;  //speed
int ch4Value;  //ch2 and ch4
int ch5Value;
// Define the servo objects
Servo camServo;
Servo armServo;
Servo baseServo;
// // Boolean to represent switch value
bool ch6Value;
int camPos = 90;
int armPos = 90;
int basePos = 90;
boolean locked = true;
int previousCamPos = camPos;
int previousArmPos = armPos;
// Motor A Control Connections
#define pwmA 2
#define in1A 24
#define in2A 26

// Motor B Control Connections
#define pwmB 4
#define in1B 28
#define in2B 30

// Motor Speed Values - Start at zero
int MotorSpeedA = 0;
int MotorSpeedB = 0;
int camd = 0;
int based = 0;
int armd = 0;

// Motor Direction Values - 0 = backward, 1 = forward
int MotorDirA = 1;
int MotorDirB = 1;
// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100)
    return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}
// Control Motor A
void mControlA(int mspeed, int mdir) {

  // Determine direction
  if (mdir == 0) {
    // Motor backward
    digitalWrite(in1A, LOW);
    digitalWrite(in2A, HIGH);
  } else {
    // Motor forward
    digitalWrite(in1A, HIGH);
    digitalWrite(in2A, LOW);
  }

  // Control motor
  analogWrite(pwmA, mspeed);
}
// Control Motor B
void mControlB(int mspeed, int mdir) {

  // Determine direction
  if (mdir == 0) {
    // Motor backward
    digitalWrite(in1B, LOW);
    digitalWrite(in2B, HIGH);
  } else {
    // Motor forward
    digitalWrite(in1B, HIGH);
    digitalWrite(in2B, LOW);
  }

  // Control motor
  analogWrite(pwmB, mspeed);
}
void gradualServoMove(Servo servo, int targetPos) {
  int currentPos = servo.read();
  int increment = (targetPos > currentPos) ? 1 : -1;

  while (currentPos != targetPos) {
    currentPos += increment;
    servo.write(currentPos);
    delay(10);
  }
}

void setup() {
  // Set up serial monitor
  Serial.begin(115200);

  // Set all pins as inputs
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);

  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(in1A, OUTPUT);
  pinMode(in2A, OUTPUT);
  pinMode(in1B, OUTPUT);
  pinMode(in2B, OUTPUT);
  pinMode(cam_servo, OUTPUT);
  pinMode(arm_servo, OUTPUT);
  pinMode(base_servo, OUTPUT);

  // Attach the servos to their pins
  camServo.attach(cam_servo);
  armServo.attach(arm_servo);
  baseServo.attach(base_servo);

  // Set the initial servo positions
  camServo.write(camPos);
  armServo.write(armPos);
  baseServo.write(basePos);
}

void loop() {

  // Get values for each channel
  ch1Value = readChannel(CH1, -100, 100, 0);
  ch2Value = readChannel(CH2, -100, 100, 0);
  ch3Value = readChannel(CH3, 0, 155, 0);
  ch4Value = readChannel(CH4, -100, 100, 0);
  ch5Value = readChannel(CH5, 100, -100, 0);
  ch6Value = readSwitch(CH6, false);
  locked = ch6Value;
  // Print to Serial Monitor
  Serial.print("Ch1: ");
  Serial.print(ch1Value);
  Serial.print(" | Ch2: ");
  Serial.print(ch2Value);
  Serial.print(" | Ch3: ");
  Serial.print(ch3Value);
  Serial.print(" | Ch4: ");
  Serial.print(ch4Value);
  // Serial.print(ch5Value);
  Serial.print(" | Ch6: ");
  Serial.println(ch6Value);
  MotorSpeedA = ch3Value;
  MotorSpeedB = ch3Value;

  // Normal Mode

  // Turn off LED
  if (locked) {  // Set forward/backward direction with channel 2 value
    if (ch2Value >= -30) {
      // Forward
      MotorDirA = 1;
      MotorDirB = 1;
      Serial.println("Forward");
    } else {
      // Backward
      MotorDirA = 0;
      MotorDirB = 0;
      Serial.println("Backward");
    }
    // Add channel 2 speed
    if (ch2Value <= -30) {
      MotorSpeedA = MotorSpeedA + abs(ch2Value);
      MotorSpeedB = MotorSpeedB + abs(ch2Value);
    } else {
      MotorSpeedA = MotorSpeedA + abs(ch2Value + 30);
      MotorSpeedB = MotorSpeedB + abs(ch2Value + 30);
    }

    // Set left/right offset with channel 4 value
    MotorSpeedA = MotorSpeedA + ch4Value;

    MotorSpeedB = MotorSpeedB - ch4Value;
    Serial.print("CH1 Motor A Speed = ");
    Serial.print(MotorSpeedA);
    Serial.print(" | CH1 Motor B Speed = ");
    Serial.println(MotorSpeedB);

    // Ensure that speeds are between 0 and 255
    MotorSpeedA = constrain(MotorSpeedA, 0, 255);
    MotorSpeedB = constrain(MotorSpeedB, 0, 255);

    // Drive Motors
    mControlA(MotorSpeedA, MotorDirA);
    mControlB(MotorSpeedB, MotorDirB);

    // Print speed values to serial monitor for debugging
    Serial.print("Motor A Speed = ");
    Serial.print(MotorSpeedA);
    Serial.print(" | Motor B Speed = ");
    Serial.println(MotorSpeedB);

    // Slight delay
    delay(50);
  } else {
    mControlA(0, 1);
    mControlB(0, 1);
    // control servos based on the values
    camPos = map(ch3Value, 0, 155, 0, 180);
    armPos = map(ch2Value, -100, 100, 0, 180);
    if (previousCamPos != camPos || previousArmPos != armPos) {
          // Control the arm servo
          gradualServoMove(armServo, armPos);

          // Control the camera servo
          gradualServoMove(camServo, camPos);

          // Update previous servo positions
          previousCamPos = camPos;
          previousArmPos = armPos;
        }

        delay(50);
      
  }
}

