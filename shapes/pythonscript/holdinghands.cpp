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


const float xAngles[] = {18.72, 22.89, 24.13, 21.07, 16.1, 14.65, 17.39, 17.96, 15.01, 13.08, 11.39, 10.09, 8.36, 8.21, 10.41, 11.39, 18.91, 19.69, 19.49, 19.49, 21.67, 28.19, 34.5, 39.88, 46.55, 52.48, 51.3, 47.74, 42.97, 41.3, 37.52, 35.42, 37.52, 43.69, 50.36, 55.73, 60.93, 65.87, 69.13, 69.72, 67.11, 64.4, 62.9, 58.02, 57.34, 56.65, 62.68, 66.08, 67.31, 68.13, 70.51, 68.33, 60.27, 55.97, 54.35, 54.11, 64.4, 34.73, 33.35, 38.7, 38.23, 35.19, 48.22, 52.01, 45.84, 43.21, 50.83, 57.34, 63.11, 65.03, 59.82, 60.27, 60.49, 60.27, 65.24, 66.49, 52.71, 50.83, 49.88, 49.88, 54.11, 21.67, 23.1, 24.55, 25.6, 26.46, 26.67, 29.96, 36.12, 31.08, 45.12, 52.01, 44.88, 15.01, 15.19, 15.55, 16.28, 17.39, 13.95, 8.98, 11.06, 15.37, 8.21};
const float yAngles[] = {78.52, 76.66, 72.51, 67.82, 69.03, 73.63, 78.02, 63.65, 58.81, 54.93, 51.18, 46.67, 41.66, 36.0, 40.71, 42.14, 40.24, 47.15, 54.0, 60.82, 63.43, 57.23, 50.24, 50.0, 57.23, 63.65, 68.23, 72.51, 70.8, 66.39, 63.65, 68.23, 72.51, 75.79, 75.96, 76.14, 76.66, 76.14, 72.32, 66.8, 64.93, 66.8, 71.76, 73.99, 69.82, 62.79, 56.54, 50.95, 46.43, 41.9, 42.38, 46.91, 40.47, 46.91, 53.06, 59.71, 62.79, 47.39, 41.66, 40.0, 45.48, 43.33, 39.76, 45.48, 36.0, 30.96, 31.19, 31.41, 31.64, 34.15, 26.35, 20.38, 14.57, 9.69, 8.44, 10.17, 9.37, 14.04, 19.2, 25.29, 54.46, 37.17, 31.64, 26.35, 21.17, 15.64, 10.49, 8.13, 8.9, 10.49, 8.75, 9.06, 10.49, 10.98, 16.01, 21.77, 27.86, 33.92, 9.37, 9.21, 7;

const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};

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
