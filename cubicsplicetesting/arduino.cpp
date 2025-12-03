// Dual Stepper Motor X-Y Angle Control
// SMOOTH SPLINE DATA PLAYER

#include <AccelStepper.h>

// --- LASER SETUP ---
#define LASER_PIN 7

// --- PINS ---
#define X_STEP_PIN 2
#define X_DIR_PIN 3
#define Y_STEP_PIN 4
#define Y_DIR_PIN 5

// --- MOTOR SETTINGS ---
#define STEPS_PER_REV 200  
#define MICROSTEPS 0.25    

AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

// --- PASTE DATA HERE ---
// (Example data - REPLACE with contents of x.txt)
const float xAngles[] = {20.03, 18.24, 16.26, 20.94, 23.09, 25.3, 27.99, 30.33, 32.84, 35.6, 38.96, 41.48, 44.12, 47.2, 50.06, 53.0, 55.47, 57.9, 60.28, 62.62, 65.38, 67.57, 69.88, 71.93, 74.08, 71.14, 68.6, 66.43, 64.2, 61.81, 59.46, 56.64, 54.18, 51.7, 49.07, 46.54, 43.57, 41.04, 38.52, 35.92, 33.47, 30.64, 28.29, 25.99, 23.66, 21.49, 18.95, 16.94, 15.0, 72.89, 74.83, 38.41}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {51.64, 48.91, 46.38, 50.44, 47.92, 45.39, 47.04, 49.57, 50.33, 47.59, 45.28, 47.81, 50.44, 49.02, 46.27, 46.27, 48.8, 51.31, 48.8, 46.27, 46.27, 48.8, 51.1, 48.58, 45.83, 43.73, 44.06, 41.53, 39.01, 41.2, 43.73, 44.06, 41.53, 39.01, 41.2, 43.73, 44.06, 41.53, 39.01, 41.2, 43.73, 43.95, 41.42, 38.9, 41.31, 43.84, 43.95, 41.42, 38.9, 41.53, 39.01, 38.79};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};


// --- VARIABLES ---
int numAngles = sizeof(xAngles) / sizeof(xAngles[0]);
int currentIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); 
  
  // --- SPEED SETTINGS ---
  // Since points are now smoothed by Spline, we can run faster/smoother
  stepperX.setMaxSpeed(1000);       
  stepperX.setAcceleration(500);   
  stepperX.setCurrentPosition(40); 
  
  stepperY.setMaxSpeed(1000);       
  stepperY.setAcceleration(500);   
  stepperY.setCurrentPosition(40); 
  
  Serial.println("System Ready.");
  Serial.print("Points loaded: ");
  Serial.println(numAngles);
  delay(1000);
}

void loop() {
  stepperX.run();
  stepperY.run();

  if (stepperX.distanceToGo() == 0 && stepperY.distanceToGo() == 0) {
      
      // Sequence complete check
      if (currentIndex >= numAngles) {
          currentIndex = 0;
          digitalWrite(LASER_PIN, LOW);
          delay(2000);
      }
      
      if (currentIndex < numAngles) {
          
          // 1. SET LASER
          if (strcmp(laserValues[currentIndex], "true") == 0) {
             digitalWrite(LASER_PIN, HIGH);
          } else {
             digitalWrite(LASER_PIN, LOW);
          }
          
          // 2. MOVE MOTORS
          moveToAngles(xAngles[currentIndex], yAngles[currentIndex]);
          
          // 3. INCREMENT
          currentIndex++;
          
          // 4. WAIT
          // Minimal delay because the spline points are already smooth and dense
          delay(5); 
      }
  }
}

void moveToAngles(float targetXData, float targetYData) {
  // SWAPPED LOGIC (X Data -> Y Stepper)
  // Uses Absolute Positioning
  
  long stepsForStepperX = angleToAbsoluteSteps(targetYData);
  stepperX.moveTo(stepsForStepperX);

  long stepsForStepperY = angleToAbsoluteSteps(targetXData);
  stepperY.moveTo(stepsForStepperY);
}

long angleToAbsoluteSteps(float angle) {
  float stepsPerDegree = (STEPS_PER_REV * MICROSTEPS) / 360.0;
  return (long)(angle * stepsPerDegree);
}
