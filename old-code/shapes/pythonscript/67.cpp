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

// --- 100-SIDED SHAPE (Hectogon / Circle) ---
// Center: 45, 45 | Radius: 40 | Points: 100


const float xAngles[] = {26.78, 25.92, 21.37, 17.3, 13.86, 11.14, 9.37, 8.44, 9.53, 12.82, 16.74, 20.57, 24.44, 27.86, 31.41, 33.23, 34.15, 34.38, 33.69, 32.32, 30.07, 25.29, 21.17, 16.37, 12.99, 9.69, 7.67, 5.91, 5.05, 4.64, 4.64, 4.91, 5.48, 6.49, 8.13, 11.31, 15.1, 19.2, 23.82, 11.64, 5.05, 18.05, 22.18, 26.14, 27.86, 28.74, 28.52, 26.78, 23.4, 19.2, 15.1, 11.64, 9.21, 8.44, 8.44, 9.69, 12.31, 15.82, 13.51, 44.52, 50.47, 56.31, 61.92, 67.21, 72.14, 74.54, 72.7, 71.18, 69.43, 67.62, 65.56, 63.65, 61.92, 59.71, 57.68, 55.39, 53.3, 56.08, 59.93, 61.92, 64.08, 66.18, 68.03, 70.02, 71.95, 73.81, 75.61, 77.18, 78.86, 78.36, 73.99, 69.23, 64.08, 58.59, 52.83, 46.91, 43.33, 73.07, 60.82, 61.04, 79.51, 29.4, 25.71, 4.77, 11.48};
const float yAngles[] = {72.14, 68.43, 67.42, 65.97, 63.43, 59.48, 54.93, 50.0, 46.43, 50.0, 51.65, 51.65, 50.95, 48.34, 44.28, 39.53, 35.07, 31.41, 27.0, 23.4, 19.59, 16.37, 15.1, 15.1, 15.46, 17.86, 20.57, 24.44, 29.4, 34.61, 40.47, 46.19, 51.65, 57.0, 62.35, 66.8, 69.62, 70.99, 71.57, 68.23, 46.91, 47.39, 46.19, 42.38, 37.64, 32.55, 28.08, 23.82, 19.98, 18.24, 19.01, 21.57, 25.92, 30.74, 36.24, 41.66, 45.0, 47.15, 61.92, 67.82, 67.82, 67.82, 67.82, 67.82, 67.82, 65.77, 61.7, 57.68, 53.06, 48.58, 44.05, 39.53, 34.61, 30.07, 25.5, 21.17, 17.68, 15.46, 18.82, 23.2, 27.65, 32.77, 37.17, 42.61, 47.86, 53.3, 58.36, 63.22, 67.62, 71.57, 71.57, 71.57, 71.57, 71.57, 71.57, 71.57, 69.62, 51.65, 22.38, 34.84, 71.57, 45.24, 23.2, 30.52, 16.93};

const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true"};

// --- VARIABLES ---
int numAngles = sizeof(xAngles) / sizeof(xAngles[0]);
int currentIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); 
  
  // --- SPEED SETTINGS ---
  // Since points are now smoothed by Spline, we can run faster/smoother
  stepperX.setMaxSpeed(100);       
  stepperX.setAcceleration(50);   
  stepperX.setCurrentPosition(40); 
  
  stepperY.setMaxSpeed(100);       
  stepperY.setAcceleration(50);   
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
