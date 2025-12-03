// Dual Stepper Motor X-Y Angle Control
// SHAPE: HEXAGON (Centered at 45,45 | Radius 40)

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

const float xAngles[] = {
  85.00, 84.92, 84.68, 84.29, 83.74, 83.04, 82.20, 81.21, 80.08, 78.82, 
  77.43, 75.91, 74.27, 72.52, 70.66, 68.69, 66.62, 64.45, 62.19, 59.84, 
  57.42, 54.92, 52.36, 49.74, 47.08, 44.37, 41.64, 38.89, 36.13, 33.38, 
  30.65, 27.95, 25.29, 22.68, 20.14, 17.67, 15.29, 13.00, 10.82, 8.76, 
  6.83, 5.05, 3.44, 2.02, 0.79, -0.24, -1.06, -1.65, -2.00, -2.11, 
  -1.97, -1.59, -0.96, -0.10, 0.98, 2.27, 3.75, 5.41, 7.24, 9.22, 
  11.33, 13.56, 15.90, 18.32, 20.82, 23.38, 26.00, 28.66, 31.36, 34.09, 
  36.83, 39.58, 42.33, 45.08, 47.81, 50.51, 53.18, 55.80, 58.36, 60.86, 
  63.29, 65.63, 67.89, 70.05, 72.10, 74.04, 75.86, 77.56, 79.13, 80.56, 
  81.85, 82.99, 83.98, 84.82, 85.51, 86.04, 86.41, 86.63, 86.69, 86.59, 
  85.00
};

const float yAngles[] = {
  45.00, 47.51, 50.01, 52.49, 54.92, 57.29, 59.60, 61.82, 63.95, 65.98, 
  67.89, 69.67, 71.32, 72.82, 74.16, 75.34, 76.35, 77.19, 77.85, 78.33, 
  78.63, 78.75, 78.68, 78.43, 78.00, 77.39, 76.59, 75.61, 74.45, 73.12, 
  71.62, 69.96, 68.15, 66.18, 64.08, 61.84, 59.48, 57.01, 54.43, 51.76, 
  49.00, 46.18, 43.30, 40.37, 37.41, 34.42, 31.42, 28.42, 25.44, 22.48, 
  19.57, 16.71, 13.91, 11.20, 8.59, 6.09, 3.73, 1.52, -0.52, -2.38, 
  -4.04, -5.48, -6.69, -7.66, -8.38, -8.84, -9.04, -8.97, -8.63, -8.03, 
  -7.18, -6.09, -4.77, -3.24, -1.52, 0.36, 2.38, 4.51, 6.75, 9.07, 
  11.47, 13.93, 16.44, 18.99, 21.57, 24.16, 26.75, 29.33, 31.89, 34.43, 
  36.93, 39.38, 41.77, 44.09, 46.33, 48.48, 50.53, 52.46, 54.27, 55.94, 
  45.00
};

// 101 "true" values
const char* laserValues[] = {
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true"
};

int numAngles = sizeof(xAngles) / sizeof(xAngles[0]);
int currentIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); 
  
  // --- SPEED SETTINGS ---
  // X Stepper (Physically Y Axis)
  stepperX.setMaxSpeed(800);       
  stepperX.setAcceleration(400);   
  stepperX.setCurrentPosition(45); 
  
  // Y Stepper (Physically X Axis)
  stepperY.setMaxSpeed(800);       
  stepperY.setAcceleration(400);   
  stepperY.setCurrentPosition(45); 
  
  Serial.println("Dual Stepper Motor X-Y Angle Control");
  delay(1000);
}

void loop() {
  stepperX.run();
  stepperY.run();

  if (stepperX.distanceToGo() == 0 && stepperY.distanceToGo() == 0) {
      
      if (currentIndex >= numAngles) {
          Serial.println("Sequence Complete.");
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
          
          // 4. WAIT (Fast 10ms delay)
          delay(10); 
      }
  }
}

void moveToAngles(float targetXData, float targetYData) {
  // SWAPPED LOGIC AS REQUESTED
  // Send Y-Data to X-Stepper
  long stepsForStepperX = angleToAbsoluteSteps(targetYData);
  stepperX.moveTo(stepsForStepperX);

  // Send X-Data to Y-Stepper
  long stepsForStepperY = angleToAbsoluteSteps(targetXData);
  stepperY.moveTo(stepsForStepperY);
}

long angleToAbsoluteSteps(float angle) {
  float stepsPerDegree = (STEPS_PER_REV * MICROSTEPS) / 360.0;
  return (long)(angle * stepsPerDegree);
}
