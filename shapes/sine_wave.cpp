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


// --- SINE WAVE DATA (2 Cycles) ---
// Horizontal: 10 to 80 degrees
// Vertical: Oscillates around 45 (Amplitude 20)

const float xAngles[] = {
  10.00, 11.43, 12.86, 14.29, 15.71, 17.14, 18.57, 20.00, 21.43, 22.86, 
  24.29, 25.71, 27.14, 28.57, 30.00, 31.43, 32.86, 34.29, 35.71, 37.14, 
  38.57, 40.00, 41.43, 42.86, 44.29, 45.71, 47.14, 48.57, 50.00, 51.43, 
  52.86, 54.29, 55.71, 57.14, 58.57, 60.00, 61.43, 62.86, 64.29, 65.71, 
  67.14, 68.57, 70.00, 71.43, 72.86, 74.29, 75.71, 77.14, 78.57, 80.00
};

const float yAngles[] = {
  45.00, 50.07, 54.81, 58.91, 62.10, 64.17, 64.99, 64.50, 62.73, 59.81, 
  55.91, 51.30, 46.28, 41.18, 36.32, 32.04, 28.60, 26.23, 25.09, 25.26, 
  26.71, 29.36, 33.04, 37.49, 42.44, 47.56, 52.51, 56.96, 60.64, 63.29, 
  64.74, 64.91, 63.77, 61.40, 57.96, 53.68, 48.82, 43.72, 38.70, 34.09, 
  30.19, 27.27, 25.50, 25.01, 25.83, 27.90, 31.09, 35.19, 39.93, 45.00
};

// 50 points, Laser always ON
const char* laserValues[] = {
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true", 
  "true", "true", "true", "true", "true", "true", "true", "true", "true", "true"
};

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
