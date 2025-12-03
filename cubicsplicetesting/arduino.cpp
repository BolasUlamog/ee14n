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
const float xAngles[] = {44.56, 37.22, 31.16, 23.95, 19.67, 16.85, 15.75, 15.33, 15.42, 16.34, 17.89, 22.05, 27.68, 33.58, 38.2, 43.35, 37.44, 31.47, 24.33, 19.76, 17.11, 16.17, 16.43, 17.98, 16.09, 34.32, 47.64, 54.4, 60.9, 66.43, 70.78, 72.98, 73.83, 73.4, 71.76, 67.76, 62.92, 54.61, 48.19, 40.6, 34.21, 39.29, 48.52, 53.97, 58.32, 65.47, 69.61, 73.06, 74.17, 74.58, 74.58, 73.83, 72.54, 69.15, 65.09, 57.37, 51.15, 43.9, 52.67, 42.25, 35.71, 29.0, 26.09, 25.01, 27.58, 27.48, 27.89, 29.51, 34.85, 40.93, 47.31, 54.4, 59.46, 61.81, 59.56, 65.09, 63.71, 62.92, 59.56, 52.46, 47.64, 59.15, 56.21, 54.4, 49.07, 48.52, 50.71, 55.79, 40.38, 26.59, 21.59, 23.37, 33.68, 33.68, 37.22, 41.04, 40.38, 41.04, 25.89, 50.39, 73.74, 55.47}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {75.0, 74.17, 72.37, 67.95, 62.92, 56.32, 51.91, 46.65, 40.27, 35.5, 30.64, 23.95, 19.4, 16.94, 15.83, 15.33, 16.85, 18.51, 23.09, 28.9, 36.25, 43.13, 50.71, 57.06, 42.47, 17.54, 16.26, 17.28, 19.76, 23.95, 30.13, 36.79, 43.79, 51.37, 57.69, 64.7, 69.15, 72.8, 73.83, 73.74, 72.54, 73.49, 74.58, 73.91, 72.54, 68.51, 63.91, 56.85, 52.46, 47.42, 41.15, 36.46, 32.0, 25.5, 21.31, 17.2, 15.67, 16.09, 15.92, 22.8, 24.72, 30.54, 37.98, 45.11, 46.21, 42.58, 38.09, 33.15, 27.28, 24.82, 24.62, 27.08, 32.31, 39.72, 44.89, 45.99, 40.6, 35.92, 29.31, 24.14, 24.53, 44.12, 56.74, 63.61, 61.41, 55.89, 50.5, 60.49, 74.5, 68.88, 64.01, 67.29, 59.87, 55.15, 49.73, 54.93, 61.81, 59.25, 40.49, 23.37, 41.26, 72.46};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};


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
