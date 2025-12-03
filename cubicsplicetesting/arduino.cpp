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
const float xAngles[] = {20.03, 19.22, 18.24, 17.2, 16.26, 15.17, 20.94, 21.96, 23.09, 24.14, 25.3, 26.78, 27.99, 29.1, 30.33, 31.47, 32.84, 34.32, 35.6, 37.22, 38.96, 40.16, 41.48, 42.8, 44.12, 45.66, 47.2, 48.52, 50.06, 51.48, 53.0, 54.29, 55.47, 56.74, 57.9, 59.15, 60.28, 61.51, 62.62, 64.2, 65.38, 66.53, 67.57, 68.69, 69.88, 70.96, 71.93, 73.15, 74.08, 72.11, 71.14, 69.97, 68.6, 67.57, 66.43, 65.38, 64.2, 63.02, 61.81, 60.69, 59.46, 58.21, 56.64, 55.47, 54.18, 53.0, 51.7, 50.39, 49.07, 47.86, 46.54, 45.22, 43.57, 42.36, 41.04, 39.83, 38.52, 37.22, 35.92, 34.75, 33.47, 32.1, 30.64, 29.41, 28.29, 27.08, 25.99, 24.72, 23.66, 22.52, 21.49, 20.21, 18.95, 17.98, 16.94, 16.0, 15.0, 55.79, 72.89, 73.83, 74.83, 38.41}; 

// (Example data - REPLACE with contents of y.txt)
const float yAngles[] = {51.64, 50.22, 48.91, 47.7, 46.38, 45.06, 50.44, 49.13, 47.92, 46.6, 45.39, 45.83, 47.04, 48.36, 49.57, 50.88, 50.33, 48.8, 47.59, 45.83, 45.28, 46.6, 47.81, 49.24, 50.44, 50.55, 49.02, 47.81, 46.27, 44.83, 46.27, 47.48, 48.8, 50.0, 51.31, 50.11, 48.8, 47.59, 46.27, 44.94, 46.27, 47.48, 48.8, 50.0, 51.1, 49.89, 48.58, 47.15, 45.83, 42.41, 43.73, 45.06, 44.06, 42.74, 41.53, 40.21, 39.01, 40.0, 41.2, 42.52, 43.73, 45.06, 44.06, 42.74, 41.53, 40.21, 39.01, 40.0, 41.2, 42.52, 43.73, 45.06, 44.06, 42.74, 41.53, 40.21, 39.01, 40.0, 41.2, 42.52, 43.73, 45.17, 43.95, 42.74, 41.42, 40.21, 38.9, 40.0, 41.31, 42.52, 43.84, 45.17, 43.95, 42.63, 41.42, 40.11, 38.9, 48.58, 41.53, 40.21, 39.01, 38.79};

// (Example data - REPLACE with contents of laser.txt)
const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true"};


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
