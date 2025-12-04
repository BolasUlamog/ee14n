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


const float xAngles[] = {42.46, 6.73, -9.89, -20.77, -24.9, -27.06, -27.81, -28.09, -28.03, -27.42, -26.31, -22.75, -15.95, -4.4, 10.44, 35.54, 7.52, -9.25, -20.34, -24.83, -26.88, -27.53, -27.36, -26.24, -27.59, -2.44, 59.93, 88.7, 103.7, 111.19, 115.27, 116.94, 117.53, 117.24, 116.04, 112.57, 106.82, 89.36, 62.86, 21.04, -2.73, 14.98, 64.57, 87.34, 98.81, 110.11, 114.29, 117.0, 117.76, 118.03, 118.03, 117.53, 116.63, 113.88, 109.65, 96.71, 76.89, 38.66, 82.87, 29.48, 1.64, -13.87, -18.18, -19.54, -16.09, -16.24, -15.64, -12.99, -0.94, 22.65, 58.13, 88.7, 101.11, 105.18, 101.31, 109.65, 107.92, 106.82, 101.31, 82.07, 59.93, 100.51, 93.85, 88.7, 67.35, 64.57, 75.02, 92.73, 19.98, -17.52, -23.2, -21.4, -4.13, -4.13, 6.73, 23.2, 19.98, 23.2, -18.43, 73.57, 117.47, 91.85};
const float yAngles[] = {118.3, 117.76, 116.5, 112.75, 106.82, 94.13, 79.99, 54.46, 19.46, 0.97, -10.91, -20.77, -25.13, -27.0, -27.76, -28.09, -27.06, -25.84, -21.7, -14.04, 3.37, 34.3, 75.02, 95.96, 30.66, -26.57, -27.47, -26.75, -24.83, -20.77, -11.89, 5.19, 38.03, 77.8, 97.43, 109.18, 113.88, 116.82, 117.53, 117.47, 116.63, 117.3, 118.03, 117.59, 116.63, 113.29, 108.18, 95.45, 82.07, 58.74, 23.75, 4.09, -8.13, -18.94, -23.46, -26.82, -27.87, -27.59, -27.7, -22.0, -19.89, -11.11, 9.58, 45.64, 51.97, 31.26, 10.01, -5.45, -16.53, -19.77, -20.0, -16.82, -7.43, 16.93, 44.36, 50.71, 21.04, 2.32, -13.35, -20.56, -20.11, 39.92, 95.19, 107.78, 104.53, 93.01, 74.05, 102.99, 117.98, 113.63, 108.31, 112.09, 101.89, 90.94, 70.54, 90.32, 105.18, 100.71, 20.51, -21.4, 24.3, 116.57};

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
