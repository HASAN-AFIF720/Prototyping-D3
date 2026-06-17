// =============================================================================
// Minimum-Snaking Line Follower (98% Memory, 2% Reaction)
// =============================================================================

#define IR_SENSOR_RIGHT 13
#define IR_SENSOR_LEFT  12

// ---------------- MOTOR PINS ----------------
const int enableRightMotor = 10;
const int rightMotorPin1 = 5;
const int rightMotorPin2 = 4;

const int enableLeftMotor = 11;
const int leftMotorPin1 = 7;
const int leftMotorPin2 = 6;

// ---------------- TUNING ----------------
#define BASE_SPEED     110     // Normal straight speed
#define MIN_SPEED      85      // Minimum speed 
#define CORRECTION     1       // Barely any steering power
#define SEARCH_SPEED   63

int lastDirection = 0;
float filteredError = 0;

// =============================================================================
void setup()
{
  TCCR0B = (TCCR0B & B11111000) | B00000010;

  pinMode(enableRightMotor, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);

  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);

  rotateMotor(0, 0);
}

// =============================================================================
void loop()
{
  int L = digitalRead(IR_SENSOR_LEFT);
  int R = digitalRead(IR_SENSOR_RIGHT);

  int error;

  // ---------------- LINE DETECTION ----------------
  if (L == HIGH && R == HIGH)
  {
    error = 0;
    lastDirection = 0;
  }
  else if (L == HIGH && R == LOW)
  {
    error = -1;
    lastDirection = -1;
  }
  else if (L == LOW && R == HIGH)
  {
    error = 1;
    lastDirection = 1;
  }
  else
  {
    searchForLine();
    return;
  }

  // ---------------- 98/2 SMOOTHING ----------------
  // 98% Memory, 2% Reaction
  filteredError = (0.99 * filteredError) + (0.01 * error);

  // Correction set to 2 for almost zero steering force
  int targetCorrection = filteredError * CORRECTION;

  // ---------------- SPEED CONTROL ----------------
  int speed = BASE_SPEED - abs(targetCorrection);

  if (speed < MIN_SPEED)
    speed = MIN_SPEED;

  int rightSpeed = speed - targetCorrection;
  int leftSpeed  = speed + targetCorrection;

  rotateMotor(rightSpeed, leftSpeed);
}

// =============================================================================
void searchForLine()
{
  if (lastDirection < 0)
    rotateMotor(-SEARCH_SPEED, SEARCH_SPEED);
  else if (lastDirection > 0)
    rotateMotor(SEARCH_SPEED, -SEARCH_SPEED);
  else
    rotateMotor(SEARCH_SPEED, SEARCH_SPEED);
}

// =============================================================================
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  }
  else if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  }
  else
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  }
  else if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  }
  else
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }

  analogWrite(enableRightMotor, constrain(abs(rightMotorSpeed), 0, 255));
  analogWrite(enableLeftMotor, constrain(abs(leftMotorSpeed), 0, 255));
}