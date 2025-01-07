#include <Servo.h>  // Include the Servo library

class Spore {
private:
  Servo servo;                            // Servo object
  int servoPin;                           // Pin connected to the servo motor
  unsigned long lastActionTime;           // Last time an action was performed
  unsigned long randomDelay;              // Random delay between actions
  unsigned long systemStartTime;          // System start time
  const unsigned long MIN_DELAY = 10000;  // 3 minutes in milliseconds - 180000
  const unsigned long MAX_DELAY = 20000;  // 4 minutes in milliseconds - 240000
  const unsigned long PAUSE = 60000;
  const unsigned long MAX_RUNTIME = 1209600000;  // 2 weeks in milliseconds

  // Servo positions (angles) within valid range
  const int ANGLES[5] = { 15, 45, 90, 135, 180 };  // Adjusted to servo limits (0°-180°)
  const int NUM_ANGLES = 5;
  int chosenAngle = 0;
  int currentAngle = 0;  // Track the current position of the servo

  // State machine states
  enum State { IDLE,
               OPEN,
               CLOSE,
               BUTTERFLY } currentState;

public:
  // Constructor
  Spore(int pin)
    : servoPin(pin), lastActionTime(0), randomDelay(0), currentState(IDLE) {}

  // Initialize the servo and set the system start time
  void begin() {
    servo.attach(servoPin);
    systemStartTime = millis();
    randomDelay = random(MIN_DELAY, MAX_DELAY);
    currentAngle = ANGLES[0];   // Start at the initial angle
    servo.write(currentAngle);  // Set servo to the starting position
  }

  // Main function to manage states
  void update() {
    // Check if the system should stop after 2 weeks
    if (millis() - systemStartTime > MAX_RUNTIME) {
      servo.detach();
      return;
    }

    unsigned long currentTime = millis();

    switch (currentState) {
      case IDLE:
        if (currentTime - lastActionTime >= randomDelay) {
          currentState = OPEN;  // Passe à l'état OPEN
        }
        break;

      case OPEN:
        chosenAngle = random(1, NUM_ANGLES);             // Choose a random angle (from ANGLES array)
        openServo(ANGLES[chosenAngle], random(5, 40));  // Move to the chosen angle
        delay(PAUSE);
        currentState = CLOSE;
        break;

      case CLOSE:
        closeServo(ANGLES[0], random(5, 40));  // Close to the initial angle (15°)
        lastActionTime = currentTime;
        randomDelay = random(MIN_DELAY, MAX_DELAY);
        currentState = random(0, 2) == 0 ? IDLE : BUTTERFLY;  // Random next state
        break;

      case BUTTERFLY:
        butterflyMovement(5, random(10, 40));  // 5 repetitions with random speed
        lastActionTime = currentTime;
        randomDelay = random(MIN_DELAY, MAX_DELAY);
        currentState = IDLE;
        break;
    }
  }

private:
  // Function to open the servo to a specific angle at a given speed
  void openServo(int angle, int speed) {
    for (int pos = currentAngle; pos <= angle; pos++) {
      servo.write(pos);
      delay(speed);
    }
    currentAngle = angle;  // Update the current position
  }

  // Function to close the servo to a specific angle at a given speed
  void closeServo(int angle, int speed) {
    for (int pos = currentAngle; pos >= angle; pos--) {
      servo.write(pos);
      delay(speed);
    }
    currentAngle = angle;  // Update the current position
  }

  // Function to perform butterfly movement
  void butterflyMovement(int repetitions, int speed) {
    for (int i = 0; i < repetitions; i++) {
      openServo(ANGLES[chosenAngle], speed);
      delay(200);
      closeServo(ANGLES[chosenAngle - 1], speed);
    }
  }
};

Spore spore(9);  // Create an instance of the Spore class, using pin 9 for the servo

void setup() {
  Serial.begin(9600);
  spore.begin();  // Initialize the Spore instance
}

void loop() {
  spore.update();  // Update the Spore state machine
}
