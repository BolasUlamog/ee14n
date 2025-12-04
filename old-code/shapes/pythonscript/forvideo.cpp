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

// ADD SHAPE UNDER HERE


const float xAngles[] = {45.0, 42.85, 41.42, 40.0, 39.05, 36.94, 35.77, 34.61, 33.69, 32.09, 30.74, 29.4, 25.5, 21.57, 17.49, 13.86, 10.49, 7.67, 4.64, 3.41, 5.62, 7.37, 10.49, 12.65, 15.46, 18.82, 21.17, 19.98, 19.79, 18.82, 18.24, 18.05, 16.93, 16.56, 20.18, 22.79, 25.92, 28.96, 31.87, 34.84, 37.87, 41.19, 33.46, 21.97, 16.74, 15.64, 16.56, 19.98, 6.05, 6.63, 38.35, 46.43, 47.39, 48.58, 49.76, 51.42, 52.83, 54.23, 55.62, 56.77, 57.91, 59.26, 62.57, 66.6, 70.8, 74.36, 77.86, 80.94, 83.51, 86.19, 85.64, 83.22, 80.15, 77.18, 73.63, 70.41, 68.83, 69.62, 70.41, 70.99, 71.76, 72.32, 72.51, 70.02, 67.42, 64.5, 61.7, 57.91, 54.93, 51.42, 48.1, 50.0, 59.93, 67.62, 72.51, 73.63, 79.02, 85.09, 60.6, 45.48, 43.57, 42.14, 41.66};
const float yAngles[] = {83.0, 81.18, 78.61, 76.4, 73.9, 71.85, 69.92, 67.72, 65.66, 62.68, 60.04, 57.57, 56.42, 55.97, 55.04, 54.35, 53.65, 53.18, 52.95, 50.59, 47.98, 45.12, 40.35, 37.52, 33.8, 29.73, 27.54, 23.71, 19.3, 17.21, 13.77, 10.09, 7.0, 4.15, 4.02, 4.98, 5.55, 7.15, 8.21, 9.29, 10.41, 11.39, 8.67, 4.57, 2.42, 3.48, 2.17, 25.6, 52.01, 52.24, 74.81, 80.07, 78.44, 76.75, 74.99, 72.61, 70.11, 67.92, 65.03, 62.9, 60.71, 58.25, 55.97, 55.97, 55.27, 54.81, 54.58, 53.18, 52.71, 52.71, 50.12, 47.03, 42.26, 38.23, 33.58, 29.96, 25.6, 21.27, 17.02, 13.25, 9.61, 6.85, 4.15, 3.75, 4.7, 5.55, 6.56, 7.9, 8.98, 10.25, 11.56, 10.57, 7.75, 4.43, 2.3, 3.61, 42.26, 51.06, 57.57, 82.1, 12.91, 12.23, 11.06};

const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "false", "true", "true", "true", "true", "true", "true"};

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
