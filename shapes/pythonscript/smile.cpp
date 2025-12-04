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


const float xAngles[] = {44.75, 40.49, 36.91, 32.47, 29.71, 27.83, 27.08, 26.79, 26.85, 27.49, 28.53, 31.26, 34.8, 38.35, 41.06, 44.05, 40.62, 37.09, 32.71, 29.77, 28.01, 27.37, 27.54, 28.59, 27.31, 38.79, 46.53, 50.46, 54.34, 57.77, 60.58, 62.05, 62.63, 62.34, 61.23, 58.62, 55.57, 50.58, 46.85, 42.46, 38.72, 41.69, 47.04, 50.21, 52.78, 57.16, 59.81, 62.11, 62.86, 63.15, 63.15, 62.63, 61.76, 59.51, 56.92, 52.22, 48.56, 44.36, 49.45, 43.41, 39.6, 35.6, 33.81, 33.14, 34.73, 34.67, 34.92, 35.91, 39.1, 42.65, 46.34, 50.46, 53.47, 54.9, 53.53, 56.92, 56.06, 55.57, 53.53, 49.32, 46.53, 53.28, 51.53, 50.46, 47.35, 47.04, 48.31, 51.28, 42.33, 34.12, 30.96, 32.11, 38.41, 38.41, 40.49, 42.71, 42.33, 42.71, 33.69, 48.12, 62.57, 51.09};
const float yAngles[] = {63.43, 62.86, 61.64, 58.74, 55.57, 51.59, 49.0, 45.95, 42.26, 39.48, 36.59, 32.47, 29.54, 27.89, 27.14, 26.79, 27.83, 28.95, 31.93, 35.54, 39.92, 43.92, 48.31, 52.03, 43.54, 28.3, 27.43, 28.13, 29.77, 32.47, 36.28, 40.24, 44.3, 48.69, 52.41, 56.68, 59.51, 61.93, 62.63, 62.57, 61.76, 62.4, 63.15, 62.69, 61.76, 59.1, 56.19, 51.91, 49.32, 46.4, 42.77, 40.05, 37.41, 33.45, 30.78, 28.07, 27.02, 27.31, 27.2, 31.75, 32.96, 36.53, 40.93, 45.06, 45.7, 43.6, 41.0, 38.09, 34.55, 33.02, 32.9, 34.43, 37.59, 41.95, 44.94, 45.57, 42.46, 39.73, 35.79, 32.59, 32.84, 44.49, 51.84, 56.0, 54.65, 51.34, 48.18, 54.09, 63.09, 59.34, 56.25, 58.32, 53.72, 50.9, 47.74, 50.77, 54.9, 53.34, 42.39, 32.11, 42.84, 61.7};

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
