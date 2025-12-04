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


const float xAngles[] = {-5.87, 7.37, 0.36, 4.36, 35.07, 21.97, 5.91, -4.5, -3.58, -4.09, -5.58, -2.94, 19.4, 41.42, 47.15, 71.76, 71.57, 57.23, 29.85, 27.43, 76.31, 94.7, 80.79, 76.14, 88.91, 85.78, 79.67, 91.4, 91.96, 93.89, 89.64, 88.53, 60.15, 55.85, 21.57, 20.38, 1.22, 16.37, 12.31, 5.48, -0.59, 5.76, -2.83, 6.49, 3.68, 11.81, 45.95, 58.59, 67.62, 76.66, 63.0, 64.93, 95.68, 83.95, 92.07, 80.31, 83.07, 89.52, 77.18, 85.64, 89.52, 90.47, 69.23, 69.23, 51.65, 48.34, 22.99, 37.64, 38.35, 22.58, 11.81, -4.9, 16.19, 19.59, -5.39, -0.59, 4.77, 10.01, 16.74, 34.15, 51.89, 76.66, 66.8, 95.58, 91.05, 88.66, 85.09, 67.62, 66.8, 54.46, 31.19, 28.08, 52.83, 37.87, 27.65, 12.48, -1.4, 45.48, 49.76, 26.14, 96.25};
const float yAngles[] = {96.34, 93.15, 89.27, 82.63, 83.51, 85.78, 96.34, 67.01, 35.31, 57.91, 76.49, 70.02, 71.18, 60.82, 73.44, 50.24, 63.0, 76.14, 67.82, 78.02, 67.82, 59.71, 23.82, 10.17, 6.78, -5.77, 0.6, 0.36, 2.36, 11.64, 15.28, 16.56, 19.98, 22.79, 50.71, 33.92, 52.59, 56.54, 43.81, 19.2, 12.48, 0.48, 3.81, -3.99, -0.59, -3.05, -2.83, -1.4, 4.36, -2.51, 4.77, 12.48, 25.5, 39.53, 57.0, 35.77, 67.21, 76.66, 81.72, 90.71, 83.37, 68.83, 39.76, 37.17, 39.76, 39.76, 8.28, 10.01, 25.92, 20.38, 5.19, 8.28, 15.64, -2.62, 20.57, 31.87, 56.08, 22.79, 20.38, 18.05, 51.18, 87.12, 94.7, 90.0, 90.71, 96.06, 95.96, 92.07, 77.01, 93.15, 92.51, 93.47, 95.58, 63.0, 89.64, 77.35, 82.78, 26.14, 6.19, 1.22, 96.06};

const char* laserValues[] = {"false", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};

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
