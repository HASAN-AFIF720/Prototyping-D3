// =============================================================================
// Smooth Line Follower (Optimized for ~2.5 cm line, close sensor spacing)
// Recommended sensor spacing: 2.0–3.0 cm
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

// ---------------- TUNING (UPDATED FOR CLOSE SENSORS) ----------------
#define BASE_SPEED     100     // stable forward speed
#define CORRECTION     7     // reduced for less snaking
#define SEARCH_SPEED   61     // gentle recovery rotation

int lastDirection = 0;
float filteredError = 0;

// =============================================================================
void setup()
{
  // PWM stability improvement
  TCCR0B = TCCR0B & B11111000 | B00000010;

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
    error = 0;                 // centered on line
    lastDirection = 0;
  }
  else if (L == HIGH && R == LOW)
  {
    error = -1;               // line is left
    lastDirection = -1;
  }
  else if (L == LOW && R == HIGH)
  {
    error = 1;                // line is right
    lastDirection = 1;
  }
  else
  {
    searchForLine();
    return;
  }

  // ---------------- SMOOTHING FILTER ----------------
  filteredError = (0.65 * filteredError) + (0.35 * error);

  int correction = filteredError * CORRECTION;

  int rightSpeed = BASE_SPEED - correction;
  int leftSpeed  = BASE_SPEED + correction;

  rotateMotor(rightSpeed, leftSpeed);
}

// =============================================================================
void searchForLine()
{
  if (lastDirection == -1)
  {
    rotateMotor(-SEARCH_SPEED, SEARCH_SPEED);
  }
  else if (lastDirection == 1)
  {
    rotateMotor(SEARCH_SPEED, -SEARCH_SPEED);
  }
  else
  {
    rotateMotor(SEARCH_SPEED, SEARCH_SPEED);
  }
}

// =============================================================================
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  // RIGHT MOTOR
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

  // LEFT MOTOR
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
  analogWrite(enableLeftMotor,  constrain(abs(leftMotorSpeed), 0, 255));
}