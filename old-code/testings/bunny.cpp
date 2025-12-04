// Dual Stepper Motor X-Y Angle Control
// SHAPE: WIDE BUNNY (Stretched Horizontally)

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

// --- WIDE BUNNY COORDINATES ---
// I stretched the X-values to fill the range 3 to 87 degrees.
// The Y-values (Height) remain the same.

const float xAngles[] = {45, 80, 73, 87, 45,  3, 17, 10, 45};
const float yAngles[] = {15, 35, 65, 85, 60, 85, 65, 35, 15};

// Laser ON for the drawing
const char* laserValues[] = {"true", "true", "true", "true", "true", "true", "true", "true", "true"};

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
  stepperX.setCurrentPosition(0); 
  
  // Y Stepper (Physically X Axis)
  stepperY.setMaxSpeed(800);       
  stepperY.setAcceleration(400);   
  stepperY.setCurrentPosition(0); 
  
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
  // SWAPPED LOGIC (X Data -> Y Stepper)
  
  long stepsForStepperX = angleToAbsoluteSteps(targetYData);
  stepperX.moveTo(stepsForStepperX);

  long stepsForStepperY = angleToAbsoluteSteps(targetXData);
  stepperY.moveTo(stepsForStepperY);
}

long angleToAbsoluteSteps(float angle) {
  float stepsPerDegree = (STEPS_PER_REV * MICROSTEPS) / 360.0;
  return (long)(angle * stepsPerDegree);
}
