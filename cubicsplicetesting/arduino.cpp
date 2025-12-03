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
const float xAngles[] = {79.77, 78.78, 76.16, 72.19, 66.77, 60.57, 53.83, 46.51, 53.9, 60.65, 66.85, 72.27, 76.22, 78.78, 79.77, 10.01, 10.94, 13.38, 17.33, 22.7, 28.98, 35.88, 43.37, 35.94, 29.05, 22.79, 17.37, 13.39, 10.94, 10.01, 23.07, 29.27, 26.33, 28.2, 32.47, 38.62, 45.87, 52.79, 58.61, 62.05, 63.2, 61.88, 67.58, 65.77, 63.99, 59.96, 54.14, 47.33, 40.0, 33.48, 28.03, 24.7, 23.53, 23.07, 23.18, 23.53, 24.6, 27.9, 33.25, 39.75, 47.13, 53.98, 59.86, 63.91, 65.77, 67.51, 62.1, 63.14, 62.2, 58.66, 52.98, 46.11, 38.81, 32.66, 28.22, 26.33, 29.34, 23.18, 52.11, 55.88, 57.25, 57.14, 55.45, 51.41, 48.98, 48.38, 49.22, 52.11, 36.13, 39.88, 41.15, 41.02, 39.38, 35.34, 32.92, 32.4, 33.21, 36.13, 52.25, 49.32, 48.38, 48.97, 51.24, 55.38, 57.07, 57.25, 56.0, 52.25, 36.25, 33.31, 32.4, 32.86, 35.17, 39.34, 41.04, 41.16, 39.98, 36.25, 44.3, 51.53, 58.22, 64.32, 69.85, 73.97, 76.74, 78.02, 77.79, 75.97, 72.85, 68.15, 62.46, 56.17, 49.27, 41.89, 34.92, 28.53, 22.71, 17.86, 14.39, 12.33, 11.87, 12.88, 15.37, 19.22, 24.52, 30.52, 37.1, 44.3, 44.42, 37.48, 31.04, 25.14, 19.91, 15.94, 13.26, 11.99, 12.04, 13.62, 16.42, 20.57, 25.95, 31.92, 38.43, 45.47, 52.43, 58.87, 64.73, 69.98, 74.0, 76.61, 78.02, 77.85, 76.3, 73.51, 69.32, 63.93, 57.97, 51.45, 44.42, 45.58, 53.16, 60.08, 66.41, 71.98, 76.11, 78.77, 79.86, 78.72, 76.14, 72.02, 66.46, 60.12, 53.17, 45.58, 44.3, 37.04, 30.38, 24.27, 18.8, 14.58, 11.75, 10.23, 10.23, 11.72, 14.61, 18.83, 24.29, 30.41, 37.04, 44.3}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {43.35, 35.97, 29.24, 23.07, 17.64, 13.73, 11.16, 10.14, 11.16, 13.72, 17.61, 23.04, 29.21, 35.97, 43.35, 44.42, 36.84, 29.88, 23.55, 17.94, 13.85, 11.27, 10.13, 11.25, 13.85, 17.98, 23.56, 29.88, 36.84, 44.42, 47.69, 44.52, 40.12, 33.22, 27.31, 23.61, 22.6, 24.44, 28.96, 35.21, 42.41, 45.82, 46.11, 39.55, 32.61, 26.6, 22.13, 20.04, 20.39, 23.19, 28.25, 34.55, 41.74, 47.69, 47.8, 41.96, 34.72, 28.39, 23.26, 20.39, 19.96, 22.0, 26.37, 32.38, 39.3, 45.9, 46.0, 42.73, 35.49, 29.26, 24.58, 22.6, 23.53, 27.24, 33.09, 40.0, 44.44, 47.8, 66.83, 64.68, 60.3, 55.49, 51.24, 50.13, 54.01, 58.72, 63.32, 66.83, 66.82, 64.63, 60.23, 55.46, 51.21, 50.21, 54.07, 58.78, 63.37, 66.82, 66.9, 63.5, 58.9, 54.17, 50.18, 51.05, 55.33, 60.14, 64.6, 66.9, 66.92, 63.54, 58.96, 54.19, 50.24, 51.01, 55.26, 60.08, 64.55, 66.92, 78.14, 77.54, 75.41, 71.86, 66.93, 61.06, 54.64, 47.59, 40.21, 33.39, 27.11, 21.49, 16.96, 13.85, 12.22, 11.98, 13.43, 16.29, 20.51, 26.07, 32.21, 38.93, 46.21, 53.37, 59.91, 65.89, 71.05, 74.84, 77.23, 78.14, 78.25, 77.31, 75.14, 71.7, 66.81, 61.12, 54.9, 48.1, 40.99, 34.32, 28.14, 22.53, 17.84, 14.55, 12.57, 11.87, 12.76, 14.9, 18.44, 23.31, 28.97, 35.22, 41.97, 49.13, 55.82, 61.99, 67.58, 72.26, 75.57, 77.54, 78.25, 80.0, 79.09, 76.62, 72.71, 67.27, 61.03, 54.18, 46.74, 54.21, 61.09, 67.33, 72.76, 76.66, 79.08, 80.0, 79.99, 79.19, 76.93, 73.35, 68.38, 62.53, 56.1, 49.13, 49.07, 56.05, 62.45, 68.3, 73.3, 76.86, 79.19, 79.99};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};


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
