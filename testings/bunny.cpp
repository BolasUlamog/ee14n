// Dual Stepper Motor X-Y Angle Control
// SHAPE: GENERIC BUNNY HEAD (Centered in 0-90 workspace)

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

// --- BUNNY COORDINATES ---
// Outline: Chin -> Right Cheek -> Right Ear Base -> Right Ear Tip -> 
//          Forehead -> Left Ear Tip -> Left Ear Base -> Left Cheek -> Chin

const float xAngles[] = {45,  70,  65,  75,  45,  15,  25,  20,  45};
const float yAngles[] = {0,  30,  70,  90,  55,  90,  70,  30,  0};

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
  // SWAPPED LOGIC
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
