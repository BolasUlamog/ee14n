// Dual Stepper Motor X-Y Angle Control
// Uses AccelStepper library for smooth movement

#include <AccelStepper.h>

// --- LASER SETUP ---
#define LASER_PIN 7

// Define X-axis stepper motor connections
#define X_STEP_PIN 2
#define X_DIR_PIN 3

// Define Y-axis stepper motor connections
#define Y_STEP_PIN 4
#define Y_DIR_PIN 5

// Motor specifications
#define STEPS_PER_REV 200  // 200 steps for 1.8° stepper
#define MICROSTEPS 0.25    // Microstepping setting (use 1, 2, 4, 8, or 16)
                           // Note: 0.25 is unusual - verify your driver setting!

// Create stepper objects
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

// List of target angles for X-axis (in degrees)
const float xAngles[] = {0, 1, 0, 1, 90, 89, 90, 89, 0};

// List of target angles for Y-axis (in degrees)
const float yAngles[] = {0, 1, 90, 89, 90, 89, 0, 0, 0};

// List of True/False values to turn laser on/off
const char* laserValues[] = {"true", "true", "true", "true", "true", "false", "true", "false", "true"};

int numAngles = sizeof(xAngles) / sizeof(xAngles[0]);
int currentIndex = 0;
int laserState = 0; // Laser State

// Current angle trackers
float currentXAngle = 0.0;
float currentYAngle = 0.0;

void setup() {
  Serial.begin(9600);
  
  // Configure Laser
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); // Start with laser OFF
  
  // Configure X stepper
  stepperX.setMaxSpeed(1000);       // Steps per second
  stepperX.setAcceleration(500);    // Steps per second²
  stepperX.setCurrentPosition(45);   // Set current position as zero
  
  // Configure Y stepper
  stepperY.setMaxSpeed(1000);       // Steps per second
  stepperY.setAcceleration(500);    // Steps per second²
  stepperY.setCurrentPosition(45);   // Set current position as zero
  
  Serial.println("Dual Stepper Motor X-Y Angle Control");
  Serial.println("Ready to move to angles from lists");
  Serial.println();
}

void loop() {
  // Check laser values based on current index
  // In C++, we use strcmp: it returns 0 if strings match exactly
  if (strcmp(laserValues[currentIndex], "true") == 0 && laserState == 0) {
    laserState = 1;
  }
  
  if (strcmp(laserValues[currentIndex], "false") == 0 && laserState == 1) {
    laserState = 0;
  }
  
  // Actuate the laser based on state
  if (laserState == 1) {
    digitalWrite(LASER_PIN, HIGH); // SET LASER TO BE ON
  }
  
  if (laserState == 0) {
    digitalWrite(LASER_PIN, LOW);  // SET LASER TO BE OFF
  }

  // Motor Logic
  if (stepperX.distanceToGo() == 0 && 
      stepperY.distanceToGo() == 0 && 
      currentIndex < numAngles) {
    
    // Move both motors to next position
    moveToAngles(xAngles[currentIndex], yAngles[currentIndex]);
    currentIndex++;
    
    // Wait at each position
    delay(10);
  }
  
  if (currentIndex >= numAngles) {
    Serial.println("\nCompleted all angles. Restarting...\n");
    currentIndex = 0;
    // Reset Laser to off on restart just in case
    laserState = 0; 
    delay(2000);
  }
  
  // Run both steppers (must be called frequently)
  stepperX.run();
  stepperY.run();
}

// Function to move both motors to specific angles
void moveToAngles(float targetX, float targetY) {
  Serial.print("Position ");
  Serial.print(currentIndex + 1);
  Serial.print(": X=");
  Serial.print(targetX);
  Serial.print("°, Y=");
  Serial.print(targetY);
  Serial.println("°");
  
  // Calculate steps needed for X-axis
  long xStepsToMove = angleToSteps(targetX - currentXAngle);
  stepperX.move(xStepsToMove);
  
  // Calculate steps needed for Y-axis
  long yStepsToMove = angleToSteps(targetY - currentYAngle);
  stepperY.move(yStepsToMove);
  
  // Update current angles
  currentXAngle = targetX;
  currentYAngle = targetY;
  
  Serial.print("Steps - X: ");
  Serial.print(xStepsToMove);
  Serial.print(", Y: ");
  Serial.println(yStepsToMove);
}

// Convert angle difference to steps
long angleToSteps(float angleDelta) {
  // Calculate total steps per revolution including microstepping
  float stepsPerDegree = (STEPS_PER_REV * MICROSTEPS) / 360.0;
  return (long)(angleDelta * stepsPerDegree);
}

// Optional: Move to custom X-Y position
void moveToCustomPosition(float x, float y) {
  moveToAngles(x, y);
}
