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
const float xAngles[] = {85.0, 84.25, 82.27, 79.08, 74.39, 68.33, 61.03, 52.52, 52.74, 61.22, 68.51, 74.54, 79.2, 82.33, 84.27, 85.0, 5.0, 5.58, 7.41, 10.44, 14.96, 20.99, 28.22, 36.72, 36.92, 28.4, 21.17, 15.12, 10.55, 7.46, 5.6, 5.0, 44.28, 35.3, 27.38, 20.65, 15.07, 11.0, 8.36, 6.79, 6.31, 6.94, 8.64, 11.51, 15.84, 21.58, 28.52, 36.6, 45.72, 54.67, 62.59, 69.34, 74.91, 79.02, 81.66, 83.24, 83.71, 83.05, 81.34, 78.5, 74.13, 68.39, 61.45, 53.37, 44.28, 48.17, 55.82, 63.81, 70.68, 76.23, 80.36, 83.09, 84.69, 84.84, 83.4, 80.93, 77.05, 71.7, 65.04, 57.22, 48.17, 41.03, 31.9, 24.04, 17.44, 12.19, 8.54, 6.22, 5.02, 5.17, 6.58, 9.2, 13.17, 18.74, 25.57, 33.7, 41.03}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {41.04, 32.4, 24.58, 18.02, 12.67, 8.87, 6.4, 5.04, 5.05, 6.44, 8.89, 12.7, 18.07, 24.66, 32.52, 41.04, 42.46, 33.57, 25.51, 18.67, 13.17, 9.15, 6.59, 5.11, 5.09, 6.55, 9.13, 13.14, 18.62, 25.43, 33.45, 42.46, 83.6, 82.96, 81.26, 78.37, 74.04, 68.27, 61.32, 53.24, 44.17, 35.18, 27.27, 20.54, 14.97, 10.89, 8.24, 6.7, 6.2, 6.87, 8.57, 11.41, 15.75, 21.48, 28.41, 36.47, 45.6, 54.57, 62.48, 69.25, 74.83, 78.93, 81.59, 83.15, 83.6, 84.93, 84.41, 82.66, 79.72, 75.37, 69.56, 62.52, 54.29, 52.95, 61.36, 68.55, 74.6, 79.18, 82.29, 84.19, 84.93, 84.96, 84.04, 81.95, 78.6, 73.65, 67.31, 59.69, 50.81, 52.67, 61.27, 68.65, 74.71, 79.35, 82.45, 84.34, 84.96};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};


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
