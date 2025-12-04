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


// --- SMILEY FACE DATA ---
// 1. Head Outline (Circle)
// 2. Left Eye
// 3. Right Eye
// 4. Mouth (Arc)

const float xAngles[] = {
  // HEAD (Counter-Clockwise Circle)
  85.00, 75.00, 45.00, 15.00, 5.00, 15.00, 45.00, 75.00, 85.00,
  
  // Jump to Left Eye
  35.00, 35.00, 
  
  // Jump to Right Eye
  55.00, 55.00,
  
  // Jump to Mouth (Left corner)
  30.00, 45.00, 60.00
};

const float yAngles[] = {
  // HEAD
  45.00, 75.00, 85.00, 75.00, 45.00, 15.00, 5.00, 15.00, 45.00,
  
  // Left Eye (Position 60 high)
  60.00, 62.00, 
  
  // Right Eye (Position 60 high)
  60.00, 62.00,
  
  // Mouth (Smile Curve)
  35.00, 25.00, 35.00
};

const char* laserValues[] = {
  // Head: Draw the circle (True)
  "true", "true", "true", "true", "true", "true", "true", "true", "true",
  
  // Jump to Left Eye (False), Draw Eye (True)
  "false", "true",
  
  // Jump to Right Eye (False), Draw Eye (True)
  "false", "true",
  
  // Jump to Mouth Start (False), Draw Middle (True), Draw End (True)
  "false", "true", "true"
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
