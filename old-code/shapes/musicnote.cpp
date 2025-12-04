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


// --- MUSIC NOTE (Beamed Eighth Notes ♫) ---
// Range: X (25-70), Y (25-80)

const float xAngles[] = {
  // 1. LEFT NOTE HEAD (Circle-ish)
  30.00, 25.00, 30.00, 35.00, 
  
  // 2. LEFT STEM (Up)
  35.00, 
  
  // 3. BEAM (Rectangle Loop)
  35.00, 65.00, 65.00, 35.00,
  
  // 4. JUMP TO RIGHT STEM (Top)
  65.00,
  
  // 5. RIGHT STEM (Down)
  65.00,
  
  // 6. RIGHT NOTE HEAD (Circle-ish)
  60.00, 65.00, 70.00, 65.00, 65.00
};

const float yAngles[] = {
  // 1. LEFT NOTE HEAD
  25.00, 30.00, 35.00, 30.00,
  
  // 2. LEFT STEM (Up)
  70.00,
  
  // 3. BEAM (Top Left -> Top Right -> Bot Right -> Bot Left)
  80.00, 80.00, 70.00, 70.00,
  
  // 4. JUMP TO RIGHT STEM
  70.00,
  
  // 5. RIGHT STEM (Down)
  30.00,
  
  // 6. RIGHT NOTE HEAD
  30.00, 25.00, 30.00, 35.00, 30.00
};

const char* laserValues[] = {
  // Draw Left Head & Stem
  "true", "true", "true", "true", 
  "true",
  
  // Draw Beam
  "true", "true", "true", "true",
  
  // Turn OFF to jump across the beam to the right side
  "false",
  
  // Draw Right Stem & Head
  "true",
  "true", "true", "true", "true", "true"
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
