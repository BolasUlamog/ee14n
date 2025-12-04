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


const float xAngles[] = {43.73, 23.47, 9.58, -3.01, -8.81, -12.08, -13.26, -13.7, -13.61, -12.63, -10.91, -5.71, 3.01, 14.74, 25.99, 40.24, 24.03, 10.22, -2.44, -8.7, -11.79, -12.81, -12.54, -10.81, -12.9, 16.43, 52.59, 70.54, 84.43, 93.58, 99.35, 101.89, 102.81, 102.36, 100.51, 95.45, 88.03, 71.05, 54.15, 32.47, 16.19, 28.89, 55.08, 69.49, 79.35, 92.14, 97.9, 101.98, 103.17, 103.61, 103.61, 102.81, 101.41, 97.31, 91.55, 77.35, 62.28, 41.82, 66.25, 37.09, 19.72, 5.38, 0.32, -1.4, 2.84, 2.66, 3.37, 6.34, 17.68, 33.38, 51.65, 70.54, 81.67, 86.1, 81.87, 91.55, 89.36, 88.03, 81.87, 65.7, 52.59, 81.05, 74.78, 70.54, 56.62, 55.08, 61.11, 73.81, 31.87, 1.14, -6.34, -3.85, 14.98, 14.98, 23.47, 33.69, 31.87, 33.69, 0.0, 60.23, 102.72, 73.07};
const float yAngles[] = {104.04, 103.17, 101.21, 95.71, 88.03, 75.02, 64.29, 49.76, 31.56, 19.2, 8.54, -3.01, -9.14, -11.98, -13.17, -13.7, -12.08, -10.2, -4.26, 5.19, 21.04, 39.6, 61.11, 76.66, 37.72, -11.31, -12.72, -11.6, -8.7, -3.01, 7.52, 22.38, 41.5, 62.86, 78.02, 90.94, 97.31, 101.7, 102.81, 102.72, 101.41, 102.45, 103.61, 102.9, 101.41, 96.46, 89.68, 76.2, 65.7, 51.97, 34.0, 21.57, 11.31, -0.63, -6.71, -11.7, -13.35, -12.9, -13.08, -4.67, -1.85, 8.33, 25.43, 45.32, 48.5, 38.03, 25.71, 13.8, 2.32, -1.7, -2.0, 1.97, 11.98, 30.07, 44.68, 47.86, 32.47, 20.25, 5.95, -2.73, -2.14, 42.46, 75.96, 89.19, 85.36, 74.05, 60.52, 83.66, 103.52, 96.95, 89.84, 94.8, 82.48, 72.32, 58.44, 71.82, 86.1, 81.25, 32.17, -3.85, 34.3, 101.31};

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
