// =============================================================================
// Milestone 2 Robot
// Line following + two-obstacle handling + ultrasonic avoidance
//
// This version UNDOES the "slow down at 20cm" obstacle-approach changes.
// Obstacle detection/trigger remains ONLY at 15 cm (OBSTACLE_DISTANCE_CM).
//
// Keeps the second obstacle behavior changes:
//  - timed 180° spin
//  - then spin until ANY sensor detects the line (stop immediately)
//  - pause 0.5s
//  - spin LEFT in place 0.5s
//  - drive forward briefly
//  - resume line following
// =============================================================================

#define IR_LINE_STATE      HIGH

// ---------------- LINE SENSOR PINS ----------------
#define IR_SENSOR_RIGHT 13
#define IR_SENSOR_LEFT  12

// ---------------- ULTRASONIC PINS ----------------
#define LEFT_TRIG  A2
#define LEFT_ECHO  A3
#define RIGHT_TRIG 2
#define RIGHT_ECHO 3

// ---------------- MOTOR PINS ----------------
const int enableRightMotor = 10;
const int rightMotorPin1 = 5;
const int rightMotorPin2 = 4;
const int enableLeftMotor = 11;
const int leftMotorPin1 = 7;
const int leftMotorPin2 = 6;

// ---------------- LINE FOLLOWING TUNING ----------------
#define SPEED_90(value) (((value) * 9 + 5) / 10)
#define TIME_FOR_90_SPEED(value) (((value) * 10 + 4) / 9)

#define MOTOR_SAFE_MIN_SPEED SPEED_90(65)
#define BASE_SPEED      SPEED_90(95)
#define MIN_SPEED       MOTOR_SAFE_MIN_SPEED
#define SEARCH_SPEED    MOTOR_SAFE_MIN_SPEED
#define LINE_MAX_SPEED  SPEED_90(125)

#define KP              0.90
#define FILTER          0.95
#define DEADZONE        0.25
#define CORNER_THR      0.65
#define STABLE_COUNT    3

// ---------------- OBSTACLE AVOIDANCE TUNING ----------------
#define OBSTACLE_DISTANCE_CM     15              // trigger avoidance ONLY at 15 cm
#define CLEAR_DISTANCE_CM        400
#define ECHO_TIMEOUT_US          9000UL
#define ULTRASONIC_SAMPLE_MS     40              // restored (no approach-slow feature)
#define OBSTACLE_COOLDOWN_MS     1200UL

#define BACK_UP_SPEED            SPEED_90(70)
#define AVOID_FORWARD_SPEED      SPEED_90(70)
#define AVOID_PARALLEL_FORWARD_SPEED SPEED_90(105)
#define AVOID_REJOIN_SPEED       SPEED_90(70)
#define AVOID_SPIN_SPEED         SPEED_90(95)

#define RECOVER_ARC_OUTER_SPEED  SPEED_90(95)
#define RECOVER_ARC_INNER_SPEED  MOTOR_SAFE_MIN_SPEED
#define REJOIN_ARC_OUTER_SPEED   SPEED_90(70)
#define REJOIN_ARC_INNER_SPEED   MOTOR_SAFE_MIN_SPEED

#define LINE_CENTER_SPIN_SPEED   MOTOR_SAFE_MIN_SPEED
#define REJOIN_HIT_BACKUP_SPEED  MOTOR_SAFE_MIN_SPEED

#define BACK_UP_10CM_MS          TIME_FOR_90_SPEED(640UL)
#define AVOID_TURN_OUT_MS        TIME_FOR_90_SPEED(300UL)
#define AVOID_SIDE_FORWARD_MS    TIME_FOR_90_SPEED(610UL)
#define AVOID_PARALLEL_TURN_MS   TIME_FOR_90_SPEED(300UL)
#define AVOID_PASS_OBJECT_MS     TIME_FOR_90_SPEED(456UL)
#define AVOID_RETURN_TURN_MS     TIME_FOR_90_SPEED(400UL)

#define LINE_REJOIN_TIMEOUT_MS   TIME_FOR_90_SPEED(2990UL)
#define RECOVERY_SWEEP_MS        TIME_FOR_90_SPEED(2220UL)
#define LINE_CENTER_TIMEOUT_MS   TIME_FOR_90_SPEED(1400UL)
#define REJOIN_HIT_BACKUP_MS     TIME_FOR_90_SPEED(200UL)

// ---------------- FIRST OBSTACLE REJOIN HOOK ----------------
#define FIRST_REJOIN_TURN90_SPEED        SPEED_90(75)
#define FIRST_REJOIN_TURN90_TIMEOUT_MS   TIME_FOR_90_SPEED(950UL)

// ---------------- SECOND OBSTACLE TURN (TIME-BASED + FIND LINE + ACTIONS) ----------------
#define TURN_180_RIGHT           true
#define TURN_180_SPEED           SPEED_90(61)
#define TURN_180_MS              TIME_FOR_90_SPEED(590UL)

#define SECOND_FIND_LINE_SPEED   SPEED_90(50)
#define SECOND_FIND_LINE_TIMEOUT TIME_FOR_90_SPEED(1800UL)

#define SECOND_LINE_HIT_PAUSE_MS         500UL
#define SECOND_LINE_HIT_SPIN_LEFT_MS     500UL
#define SECOND_LINE_HIT_SPIN_LEFT_SPEED  SPEED_90(60)

#define SECOND_REJOIN_FORWARD_MS    250UL
#define SECOND_REJOIN_FORWARD_SPEED SPEED_90(70)

#define SECOND_SETTLE_MS         120UL

// ---------------- DEBUG ----------------
#define DEBUG_SERIAL             1

// =============================================================================
// Structs + function prototypes
// =============================================================================
struct LineSample
{
  int leftRaw;
  int rightRaw;
  bool leftLine;
  bool rightLine;
};

LineSample readLineSensors();
void updateObstacleReadings();
bool obstacleDetected();
void followLine(LineSample line);
void avoidObstacle();
bool recoverLine(bool avoidedRight);
void performSecondObstacleTurn();
void resetLineTracking();
void searchForLine();
void driveForward(int speed);
void driveBackward(int speed);
bool lineDetected();
void rememberCurrentLineDirection();
bool arcTurnUntilLineAfterFirstObstacle(bool turnRight, int outerSpeed, int innerSpeed, unsigned long timeoutMs);
bool arcTurnUntilLine(bool turnRight, int outerSpeed, int innerSpeed, unsigned long timeoutMs);
void arcTurnRobot(bool turnRight, int outerSpeed, int innerSpeed);
void spinRobot(bool turnRight, int speed);
void spinRobotFor(bool turnRight, int speed, unsigned long durationMs);
int getDistance(int trigPin, int echoPin);
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed);

bool hookLeftOntoLine90(unsigned long timeoutMs);
bool spinUntilLineDetected(bool turnRight, int speed, unsigned long timeoutMs);

// =============================================================================
// Globals
// =============================================================================
int lastDirection = 0;
float filteredError = 0;

int lastL = -1;
int lastR = -1;
int stableCount = 0;

int lastRightSpeed = 0;
int lastLeftSpeed = 0;

int lastLeftDistance = CLEAR_DISTANCE_CM;
int lastRightDistance = CLEAR_DISTANCE_CM;

int confirmedObstacleCount = 0;

unsigned long lastUltrasonicReadMs = 0;
unsigned long ignoreObstacleUntilMs = 0;
unsigned long lastSerialPrintMs = 0;

// =============================================================================
void setup()
{
#if DEBUG_SERIAL
  Serial.begin(9600);
#endif

  pinMode(enableRightMotor, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);

  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  digitalWrite(LEFT_TRIG, LOW);
  digitalWrite(RIGHT_TRIG, LOW);

  rotateMotor(0, 0);
}

// =============================================================================
void loop()
{
  updateObstacleReadings();
  LineSample line = readLineSensors();

#if DEBUG_SERIAL
  if (millis() - lastSerialPrintMs > 500)
  {
    lastSerialPrintMs = millis();
    Serial.print("IR L: ");
    Serial.print(line.leftRaw);
    Serial.print(" | IR R: ");
    Serial.print(line.rightRaw);
    Serial.print(" || US L: ");
    Serial.print(lastLeftDistance);
    Serial.print(" cm | US R: ");
    Serial.print(lastRightDistance);
    Serial.println(" cm");
  }
#endif

  if (obstacleDetected())
  {
    avoidObstacle();
    return;
  }

  followLine(line);
}

// =============================================================================
LineSample readLineSensors()
{
  LineSample line;
  line.leftRaw = digitalRead(IR_SENSOR_LEFT);
  line.rightRaw = digitalRead(IR_SENSOR_RIGHT);
  line.leftLine = (line.leftRaw == IR_LINE_STATE);
  line.rightLine = (line.rightRaw == IR_LINE_STATE);
  return line;
}

// =============================================================================
void followLine(LineSample line)
{
  int L = line.leftLine ? 1 : 0;
  int R = line.rightLine ? 1 : 0;

  if (L == lastL && R == lastR) stableCount++;
  else stableCount = 1;

  lastL = L;
  lastR = R;

  if (stableCount < STABLE_COUNT)
  {
    rotateMotor(lastRightSpeed, lastLeftSpeed);
    return;
  }

  int error;
  if (L == 1 && R == 1)
  {
    error = 0;
    lastDirection = 0;
  }
  else if (L == 1 && R == 0)
  {
    error = -1;
    lastDirection = -1;
  }
  else if (L == 0 && R == 1)
  {
    error = 1;
    lastDirection = 1;
  }
  else
  {
    searchForLine();
    return;
  }

  filteredError = (FILTER * filteredError) + ((1.0 - FILTER) * error);
  if (abs(filteredError) < DEADZONE) filteredError = 0;

  if (abs(filteredError) > CORNER_THR)
  {
    if (filteredError > 0)
    {
      lastRightSpeed = MOTOR_SAFE_MIN_SPEED;
      lastLeftSpeed  = LINE_MAX_SPEED;
    }
    else
    {
      lastRightSpeed = LINE_MAX_SPEED;
      lastLeftSpeed  = MOTOR_SAFE_MIN_SPEED;
    }
    rotateMotor(lastRightSpeed, lastLeftSpeed);
    return;
  }

  float turn = filteredError * KP;

  int rightSpeed = (int)(BASE_SPEED * (1.0 - turn));
  int leftSpeed  = (int)(BASE_SPEED * (1.0 + turn));

  int penalty = (int)(abs(turn) * 6);
  rightSpeed -= penalty;
  leftSpeed  -= penalty;

  rightSpeed = constrain(rightSpeed, MIN_SPEED, LINE_MAX_SPEED);
  leftSpeed = constrain(leftSpeed, MIN_SPEED, LINE_MAX_SPEED);

  lastRightSpeed = rightSpeed;
  lastLeftSpeed = leftSpeed;

  rotateMotor(rightSpeed, leftSpeed);
}

// =============================================================================
void updateObstacleReadings()
{
  unsigned long now = millis();
  if (now - lastUltrasonicReadMs < ULTRASONIC_SAMPLE_MS) return;

  lastUltrasonicReadMs = now;

  lastLeftDistance = getDistance(LEFT_TRIG, LEFT_ECHO);
  delay(5);
  lastRightDistance = getDistance(RIGHT_TRIG, RIGHT_ECHO);
}

// =============================================================================
bool obstacleDetected()
{
  if (millis() < ignoreObstacleUntilMs) return false;

  return (lastLeftDistance <= OBSTACLE_DISTANCE_CM ||
          lastRightDistance <= OBSTACLE_DISTANCE_CM);
}

// =============================================================================
void avoidObstacle()
{
  rotateMotor(0, 0);
  delay(80);

  int freshLeft = getDistance(LEFT_TRIG, LEFT_ECHO);
  delay(5);
  int freshRight = getDistance(RIGHT_TRIG, RIGHT_ECHO);

  lastLeftDistance = freshLeft;
  lastRightDistance = freshRight;

  if (freshLeft > OBSTACLE_DISTANCE_CM && freshRight > OBSTACLE_DISTANCE_CM)
  {
    resetLineTracking();
    return;
  }

  confirmedObstacleCount++;

  // NEW LOGIC: When count hits 2, reset it immediately and do the turn.
  if (confirmedObstacleCount == 2)
  {
    confirmedObstacleCount = 0;
    performSecondObstacleTurn();
    return;
  }

  bool avoidRight = false;

#if DEBUG_SERIAL
  Serial.print("First obstacle detected. Clearance L/R: ");
  Serial.print(freshLeft);
  Serial.print(" cm / ");
  Serial.print(freshRight);
  Serial.println(" cm. Avoiding left.");
#endif

  driveBackward(BACK_UP_SPEED);
  delay(BACK_UP_10CM_MS);
  rotateMotor(0, 0);
  delay(80);

  spinRobotFor(avoidRight, AVOID_SPIN_SPEED, AVOID_TURN_OUT_MS);

  driveForward(AVOID_FORWARD_SPEED);
  delay(AVOID_SIDE_FORWARD_MS);
  rotateMotor(0, 0);
  delay(50);

  spinRobotFor(!avoidRight, AVOID_SPIN_SPEED, AVOID_PARALLEL_TURN_MS);

  driveForward(AVOID_PARALLEL_FORWARD_SPEED);
  delay(AVOID_PASS_OBJECT_MS);
  rotateMotor(0, 0);
  delay(50);

  rotateMotor(0, 0);
  delay(50);
  spinRobotFor(!avoidRight, AVOID_SPIN_SPEED, AVOID_RETURN_TURN_MS);

  bool lineRecovered = arcTurnUntilLineAfterFirstObstacle(!avoidRight,
                                                         REJOIN_ARC_OUTER_SPEED,
                                                         REJOIN_ARC_INNER_SPEED,
                                                         LINE_REJOIN_TIMEOUT_MS);

  if (!lineRecovered)
  {
    lineRecovered = recoverLine(avoidRight);
  }

  rotateMotor(0, 0);

  ignoreObstacleUntilMs = millis() + OBSTACLE_COOLDOWN_MS;
  resetLineTracking();
  lastDirection = lineRecovered ? 0 : 1;
}

// =============================================================================
void performSecondObstacleTurn()
{
#if DEBUG_SERIAL
  Serial.println("Second obstacle: timed 180 + spin until line + pause + left spin + forward");
#endif

  rotateMotor(0, 0);
  delay(120);

  spinRobotFor(TURN_180_RIGHT, TURN_180_SPEED, TURN_180_MS);

  bool found = spinUntilLineDetected(TURN_180_RIGHT, SECOND_FIND_LINE_SPEED, SECOND_FIND_LINE_TIMEOUT);

#if DEBUG_SERIAL
  Serial.print("Second obstacle line reacquire: ");
  Serial.println(found ? "FOUND" : "TIMEOUT");
#endif

  if (found)
  {
    rotateMotor(0, 0);
    delay(SECOND_LINE_HIT_PAUSE_MS);

    // spin LEFT for 0.5s
    spinRobot(false, SECOND_LINE_HIT_SPIN_LEFT_SPEED);
    delay(SECOND_LINE_HIT_SPIN_LEFT_MS);
    rotateMotor(0, 0);
    delay(60);

    driveForward(SECOND_REJOIN_FORWARD_SPEED);
    delay(SECOND_REJOIN_FORWARD_MS);

    rotateMotor(0, 0);
    delay(80);
  }
  else
  {
    rotateMotor(0, 0);
    delay(SECOND_SETTLE_MS);
  }

  ignoreObstacleUntilMs = millis() + OBSTACLE_COOLDOWN_MS;
  resetLineTracking();
  lastDirection = 0;
}

// =============================================================================
bool spinUntilLineDetected(bool turnRight, int speed, unsigned long timeoutMs)
{
  unsigned long start = millis();
  while (millis() - start < timeoutMs)
  {
    if (lineDetected())
    {
      rotateMotor(0, 0);
      return true;
    }
    spinRobot(turnRight, speed);
    delay(5);
  }
  rotateMotor(0, 0);
  return false;
}

// =============================================================================
bool recoverLine(bool avoidedRight)
{
  bool lineIsOnLeft = avoidedRight;

  if (arcTurnUntilLine(!lineIsOnLeft, RECOVER_ARC_OUTER_SPEED, RECOVER_ARC_INNER_SPEED, RECOVERY_SWEEP_MS))
    return true;

  if (arcTurnUntilLine(lineIsOnLeft, RECOVER_ARC_OUTER_SPEED, RECOVER_ARC_INNER_SPEED, RECOVERY_SWEEP_MS))
    return true;

  rotateMotor(0, 0);
  return false;
}

// =============================================================================
void resetLineTracking()
{
  filteredError = 0;
  lastL = -1;
  lastR = -1;
  stableCount = 0;
  lastRightSpeed = 0;
  lastLeftSpeed = 0;
}

// =============================================================================
void searchForLine()
{
  if (lastDirection < 0) rotateMotor(-SEARCH_SPEED, SEARCH_SPEED);
  else if (lastDirection > 0) rotateMotor(SEARCH_SPEED, -SEARCH_SPEED);
  else rotateMotor(SEARCH_SPEED, -SEARCH_SPEED);
}

// =============================================================================
void driveForward(int speed)
{
  rotateMotor(speed, speed);
}

// =============================================================================
void driveBackward(int speed)
{
  rotateMotor(-speed, -speed);
}

// =============================================================================
bool lineDetected()
{
  LineSample line = readLineSensors();
  return (line.leftLine || line.rightLine);
}

// =============================================================================
bool hookLeftOntoLine90(unsigned long timeoutMs)
{
  rotateMotor(0, 0);
  delay(70);

  driveBackward(REJOIN_HIT_BACKUP_SPEED);
  delay(REJOIN_HIT_BACKUP_MS);

  rotateMotor(0, 0);
  delay(70);

  unsigned long start = millis();
  while (millis() - start < timeoutMs)
  {
    LineSample line = readLineSensors();

    if (line.leftLine && line.rightLine)
    {
      rotateMotor(0, 0);
      delay(80);
      lastDirection = 0;
      return true;
    }

    spinRobot(false, FIRST_REJOIN_TURN90_SPEED);
    delay(5);
  }

  rotateMotor(0, 0);
  delay(80);
  rememberCurrentLineDirection();
  LineSample l = readLineSensors();
  return (l.leftLine || l.rightLine);
}

// =============================================================================
void rememberCurrentLineDirection()
{
  LineSample line = readLineSensors();
  if (line.leftLine && !line.rightLine) lastDirection = -1;
  else if (!line.leftLine && line.rightLine) lastDirection = 1;
  else if (line.leftLine && line.rightLine) lastDirection = 0;
}

// =============================================================================
bool arcTurnUntilLineAfterFirstObstacle(bool turnRight, int outerSpeed, int innerSpeed, unsigned long timeoutMs)
{
  unsigned long startTime = millis();
  while (millis() - startTime < timeoutMs)
  {
    if (lineDetected())
    {
      return hookLeftOntoLine90(FIRST_REJOIN_TURN90_TIMEOUT_MS);
    }
    arcTurnRobot(turnRight, outerSpeed, innerSpeed);
    delay(5);
  }
  return false;
}

// =============================================================================
bool arcTurnUntilLine(bool turnRight, int outerSpeed, int innerSpeed, unsigned long timeoutMs)
{
  unsigned long startTime = millis();
  while (millis() - startTime < timeoutMs)
  {
    if (lineDetected())
    {
      rememberCurrentLineDirection();
      return true;
    }
    arcTurnRobot(turnRight, outerSpeed, innerSpeed);
    delay(5);
  }
  return false;
}

// =============================================================================
void arcTurnRobot(bool turnRight, int outerSpeed, int innerSpeed)
{
  if (turnRight) rotateMotor(innerSpeed, outerSpeed);
  else rotateMotor(outerSpeed, innerSpeed);
}

// =============================================================================
void spinRobot(bool turnRight, int speed)
{
  if (turnRight) rotateMotor(speed, -speed);
  else rotateMotor(-speed, speed);
}

// =============================================================================
void spinRobotFor(bool turnRight, int speed, unsigned long durationMs)
{
  spinRobot(turnRight, speed);
  delay(durationMs);
  rotateMotor(0, 0);
  delay(50);
}

// =============================================================================
int getDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0) return CLEAR_DISTANCE_CM;

  int distance = (int)(duration * 0.0343 / 2.0);
  if (distance <= 0 || distance > CLEAR_DISTANCE_CM) return CLEAR_DISTANCE_CM;
  return distance;
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

  int rightPwm = constrain(abs(rightMotorSpeed), 0, 255);
  int leftPwm  = constrain(abs(leftMotorSpeed), 0, 255);

  if (rightPwm > 0 && rightPwm < MOTOR_SAFE_MIN_SPEED) rightPwm = MOTOR_SAFE_MIN_SPEED;
  if (leftPwm  > 0 && leftPwm  < MOTOR_SAFE_MIN_SPEED) leftPwm  = MOTOR_SAFE_MIN_SPEED;

  analogWrite(enableRightMotor, rightPwm);
  analogWrite(enableLeftMotor, leftPwm);
}