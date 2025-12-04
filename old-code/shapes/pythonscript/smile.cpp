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


const float xAngles[] = {44.05, 28.52, 16.93, 5.19, -0.71, -4.2, -5.49, -5.96, -5.87, -4.8, -2.94, 2.49, 10.98, 21.37, 30.52, 41.42, 28.96, 17.49, 5.76, -0.59, -3.89, -5.0, -4.7, -2.83, -5.1, 22.79, 50.71, 64.71, 76.66, 85.36, 91.28, 93.99, 95.0, 94.5, 92.51, 87.25, 79.99, 65.14, 51.89, 35.54, 22.58, 32.77, 52.59, 63.86, 72.14, 83.95, 89.76, 94.09, 95.39, 95.87, 95.87, 95.0, 93.47, 89.15, 83.37, 70.41, 58.13, 42.61, 61.26, 39.05, 25.5, 13.17, 8.44, 6.78, 10.82, 10.65, 11.31, 14.04, 23.82, 36.24, 50.0, 64.71, 74.18, 78.19, 74.36, 83.37, 81.25, 79.99, 74.36, 60.82, 50.71, 73.63, 68.23, 64.71, 53.76, 52.59, 57.23, 67.42, 35.07, 9.21, 1.85, 4.36, 21.57, 21.57, 28.52, 36.47, 35.07, 36.47, 8.13, 56.54, 94.9, 66.8};
const float yAngles[] = {96.34, 95.39, 93.26, 87.51, 79.99, 68.43, 59.71, 48.58, 34.84, 25.07, 16.01, 5.19, -1.05, -4.09, -5.39, -5.96, -4.2, -2.18, 3.95, 12.99, 26.57, 40.95, 57.23, 69.82, 39.53, -3.37, -4.9, -3.68, -0.59, 5.19, 15.1, 27.65, 42.38, 58.59, 70.99, 82.78, 89.15, 93.78, 95.0, 94.9, 93.47, 94.6, 95.87, 95.1, 93.47, 88.28, 81.56, 69.43, 60.82, 50.24, 36.7, 27.0, 18.43, 7.52, 1.47, -3.78, -5.58, -5.1, -5.29, 3.54, 6.34, 15.82, 30.07, 45.24, 47.62, 39.76, 30.29, 20.57, 10.33, 6.49, 6.19, 10.01, 19.01, 33.69, 44.76, 47.15, 35.54, 25.92, 13.69, 5.48, 6.05, 43.09, 69.23, 81.1, 77.52, 67.62, 56.77, 75.96, 95.77, 88.78, 81.72, 86.59, 74.9, 66.18, 55.16, 65.77, 78.19, 73.81, 35.31, 4.36, 36.94, 93.37};

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
