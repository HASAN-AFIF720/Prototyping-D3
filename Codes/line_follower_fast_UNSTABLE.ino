// =============================================================================
// Ultra Stable Line Follower (Noise Removal + Stable Tracking)
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
#define BASE_SPEED     128
#define MIN_SPEED      100
#define SEARCH_SPEED   65

#define KP             1.05
#define FILTER         0.995     // stronger smoothing (important fix)
#define DEADZONE       0.12
#define CORNER_THR     0.65

// ---------------- INPUT STABILITY ----------------
#define STABLE_COUNT   3   // must see same reading 3 times

int lastDirection = 0;
float filteredError = 0;

int lastL = -1, lastR = -1;
int stableCount = 0;

int lastRightSpeed = 0;
int lastLeftSpeed = 0;

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

  // ---------------- NOISE FILTER (KEY FIX) ----------------
  if (L == lastL && R == lastR)
    stableCount++;
  else
    stableCount = 0;

  lastL = L;
  lastR = R;

  if (stableCount < STABLE_COUNT)
  {
    // hold last motor output → removes twitching completely
    rotateMotor(lastRightSpeed, lastLeftSpeed);
    return;
  }

  int error;

  // ---------------- LINE STATE ----------------
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

  // ---------------- SMOOTH FILTER ----------------
  filteredError = (FILTER * filteredError) + ((1 - FILTER) * error);

  if (abs(filteredError) < DEADZONE)
    filteredError = 0;

  // ---------------- CORNER MODE ----------------
  if (abs(filteredError) > CORNER_THR)
  {
    if (filteredError > 0)
    {
      lastRightSpeed = 35;
      lastLeftSpeed = 150;
    }
    else
    {
      lastRightSpeed = 150;
      lastLeftSpeed = 35;
    }

    rotateMotor(lastRightSpeed, lastLeftSpeed);
    return;
  }

  // ---------------- TRACK MODE ----------------
  float turn = filteredError * KP;

  int rightSpeed = BASE_SPEED * (1.0 - turn);
  int leftSpeed  = BASE_SPEED * (1.0 + turn);

  int penalty = abs(turn) * 6;

  rightSpeed -= penalty;
  leftSpeed  -= penalty;

  rightSpeed = constrain(rightSpeed, MIN_SPEED, 140);
  leftSpeed  = constrain(leftSpeed, MIN_SPEED, 140);

  lastRightSpeed = rightSpeed;
  lastLeftSpeed  = leftSpeed;

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
