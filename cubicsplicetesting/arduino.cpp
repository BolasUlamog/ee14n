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
const float xAngles[] = {84.96, 84.93, 84.69, 84.33, 83.86, 83.29, 82.62, 81.86, 80.98, 79.96, 78.8, 77.49, 76.04, 74.46, 72.76, 70.93, 68.98, 66.91, 64.72, 62.43, 60.04, 57.54, 54.9, 52.14, 49.17, 49.39, 52.36, 55.1, 57.73, 60.23, 62.62, 64.91, 67.09, 69.16, 71.1, 72.92, 74.62, 76.19, 77.62, 78.92, 80.07, 81.07, 81.93, 82.67, 83.33, 83.88, 84.34, 84.7, 84.94, 84.96, 5.04, 5.04, 5.18, 5.52, 5.94, 6.46, 7.07, 7.8, 8.64, 9.61, 10.71, 11.95, 13.34, 14.88, 16.58, 18.41, 20.35, 22.39, 24.53, 26.81, 29.22, 31.74, 34.37, 37.1, 40.1, 40.31, 37.31, 34.57, 31.94, 29.4, 26.99, 24.72, 22.57, 20.53, 18.58, 16.75, 15.04, 13.48, 12.08, 10.82, 9.71, 8.72, 7.86, 7.12, 6.49, 5.96, 5.53, 5.19, 5.04, 5.04, 44.28, 38.31, 32.78, 27.7, 23.13, 19.08, 15.51, 12.58, 10.32, 8.63, 7.42, 6.67, 6.42, 6.52, 7.17, 8.25, 9.8, 11.91, 14.64, 18.07, 22.0, 26.42, 31.37, 36.79, 42.67, 48.74, 54.48, 59.8, 64.62, 68.93, 72.75, 76.06, 78.63, 80.6, 82.04, 83.05, 83.58, 83.61, 83.23, 82.34, 81.02, 79.21, 76.81, 73.67, 70.0, 65.82, 61.12, 55.94, 50.3, 44.28, 48.17, 49.43, 52.34, 55.1, 57.7, 60.18, 62.56, 64.84, 67.02, 69.09, 71.03, 72.85, 74.54, 76.09, 77.52, 78.81, 79.96, 80.99, 81.89, 82.65, 83.3, 83.85, 84.31, 84.69, 84.93, 84.97, 84.84, 84.51, 84.11, 83.6, 82.99, 82.29, 81.49, 80.57, 79.5, 78.28, 76.92, 75.42, 73.79, 72.04, 70.16, 68.16, 66.04, 63.82, 61.5, 59.07, 56.51, 53.81, 51.0, 48.17, 41.04, 38.13, 35.32, 32.62, 30.04, 27.59, 25.25, 23.05, 20.96, 18.98, 17.11, 15.36, 13.76, 12.32, 11.03, 9.89, 8.88, 8.0, 7.24, 6.59, 6.05, 5.61, 5.25, 5.04, 5.03, 5.04, 5.18, 5.46, 5.86, 6.38, 7.01, 7.75, 8.61, 9.58, 10.68, 11.92, 13.3, 14.84, 16.54, 18.38, 20.34, 22.39, 24.55, 26.82, 29.23, 31.77, 34.43, 37.18, 40.1, 41.04}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {41.17, 38.66, 35.89, 33.24, 30.67, 28.22, 25.9, 23.71, 21.64, 19.67, 17.78, 16.01, 14.37, 12.89, 11.56, 10.37, 9.31, 8.38, 7.58, 6.88, 6.3, 5.81, 5.41, 5.12, 5.01, 5.01, 5.14, 5.44, 5.85, 6.33, 6.91, 7.6, 8.41, 9.34, 10.39, 11.58, 12.92, 14.41, 16.05, 17.83, 19.72, 21.71, 23.79, 25.99, 28.32, 30.79, 33.35, 36.01, 38.79, 41.17, 42.59, 40.05, 37.18, 34.42, 31.79, 29.28, 26.88, 24.59, 22.42, 20.36, 18.43, 16.62, 14.94, 13.39, 11.98, 10.72, 9.61, 8.65, 7.82, 7.09, 6.46, 5.93, 5.5, 5.19, 5.0, 5.0, 5.17, 5.47, 5.89, 6.42, 7.06, 7.79, 8.62, 9.58, 10.69, 11.96, 13.36, 14.91, 16.58, 18.38, 20.3, 22.35, 24.52, 26.8, 29.18, 31.68, 34.3, 37.05, 39.92, 42.59, 83.64, 83.35, 82.58, 81.39, 79.71, 77.45, 74.53, 70.96, 66.91, 62.34, 57.28, 51.74, 45.75, 39.78, 34.13, 28.93, 24.24, 20.06, 16.36, 13.23, 10.81, 8.97, 7.66, 6.8, 6.4, 6.4, 6.96, 7.93, 9.36, 11.32, 13.88, 17.17, 20.98, 25.28, 30.1, 35.38, 41.14, 47.22, 53.12, 58.53, 63.47, 67.92, 71.85, 75.29, 78.06, 80.17, 81.71, 82.81, 83.47, 83.64, 84.96, 84.95, 84.85, 84.54, 84.15, 83.66, 83.06, 82.36, 81.54, 80.61, 79.54, 78.35, 77.01, 75.54, 73.92, 72.17, 70.29, 68.29, 66.18, 63.98, 61.69, 59.29, 56.73, 54.03, 51.25, 49.68, 52.7, 55.37, 57.98, 60.49, 62.87, 65.13, 67.26, 69.29, 71.23, 73.06, 74.77, 76.33, 77.73, 78.99, 80.11, 81.1, 81.97, 82.72, 83.36, 83.89, 84.34, 84.7, 84.92, 84.96, 85.0, 84.87, 84.6, 84.2, 83.7, 83.1, 82.41, 81.61, 80.69, 79.62, 78.4, 77.02, 75.49, 73.83, 72.05, 70.14, 68.1, 65.94, 63.67, 61.27, 58.77, 56.16, 53.41, 50.55, 47.38, 49.49, 52.4, 55.19, 57.85, 60.39, 62.81, 65.13, 67.33, 69.41, 71.36, 73.19, 74.88, 76.45, 77.88, 79.16, 80.3, 81.29, 82.14, 82.87, 83.51, 84.05, 84.48, 84.8, 84.96, 85.0};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "true", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "true", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "true", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "true", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "false", "true", "true"};


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
