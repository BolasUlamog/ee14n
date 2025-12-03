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
const float xAngles[] = {79.77, 77.52, 71.66, 62.71, 52.23, 52.3, 62.79, 71.75, 77.52, 79.77, 10.01, 12.1, 17.83, 26.84, 37.45, 37.52, 26.92, 17.87, 12.1, 10.01, 23.07, 27.03, 39.36, 56.79, 62.09, 65.88, 58.16, 40.76, 26.33, 23.07, 23.18, 26.13, 40.56, 58.03, 65.88, 61.69, 56.93, 39.57, 27.11, 23.18, 52.11, 55.88, 57.25, 57.14, 55.45, 51.41, 48.98, 48.38, 49.22, 52.11, 36.13, 39.88, 41.15, 41.02, 39.38, 35.34, 32.92, 32.4, 33.21, 36.13, 52.25, 49.32, 48.38, 48.97, 51.24, 55.38, 57.07, 57.25, 56.0, 52.25, 36.25, 33.31, 32.4, 32.86, 35.17, 39.34, 41.04, 41.16, 39.98, 36.25, 44.3, 65.62, 77.5, 74.04, 56.91, 34.19, 16.53, 12.22, 23.25, 44.3, 44.42, 23.31, 12.21, 16.42, 34.02, 56.76, 74.0, 77.56, 65.79, 44.42, 45.58, 57.08, 67.07, 74.9, 79.19, 79.19, 74.95, 67.12, 57.1, 45.58, 44.3, 32.55, 22.36, 14.58, 10.59, 10.58, 14.61, 22.4, 32.56, 44.3}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {43.35, 32.15, 22.42, 14.87, 10.82, 10.81, 14.83, 22.39, 32.15, 43.35, 44.42, 32.89, 22.87, 15.1, 10.8, 10.81, 15.13, 22.89, 32.89, 44.42, 47.69, 36.15, 23.34, 27.15, 42.49, 41.21, 24.79, 20.18, 30.96, 47.69, 47.8, 31.08, 20.28, 24.6, 40.96, 42.44, 27.41, 23.31, 36.05, 47.8, 66.83, 64.68, 60.3, 55.49, 51.24, 50.13, 54.01, 58.72, 63.32, 66.83, 66.82, 64.63, 60.23, 55.46, 51.21, 50.21, 54.07, 58.78, 63.37, 66.82, 66.9, 63.5, 58.9, 54.17, 50.18, 51.05, 55.33, 60.14, 64.6, 66.9, 66.92, 63.54, 58.96, 54.19, 50.24, 51.01, 55.26, 60.08, 64.55, 66.92, 78.14, 70.93, 51.59, 29.14, 14.1, 13.7, 28.05, 50.27, 70.04, 78.14, 78.25, 70.21, 50.45, 28.14, 13.73, 14.09, 28.97, 51.45, 70.87, 78.25, 80.0, 77.91, 72.16, 63.18, 52.59, 52.63, 63.25, 72.24, 77.91, 80.0, 79.99, 77.79, 71.83, 62.53, 51.52, 51.46, 62.45, 71.74, 77.78, 79.99};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true"};


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
  stepperX.setCurrentPosition(0); 
  
  stepperY.setMaxSpeed(1000);       
  stepperY.setAcceleration(500);   
  stepperY.setCurrentPosition(0); 
  
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
