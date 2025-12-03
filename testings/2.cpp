// Dual Stepper Motor X-Y Angle Control
// UPDATED: X Stepper is Y Coordinate | Fixed Gravity Drift

#include <AccelStepper.h>

// --- LASER SETUP ---
#define LASER_PIN 7

// --- PINS ---
// NOTE: You said X_STEPPER is the Y_COORDINATE.
// This code maps the logic accordingly.
#define X_STEP_PIN 2
#define X_DIR_PIN 3
#define Y_STEP_PIN 4
#define Y_DIR_PIN 5

// --- MOTOR SETTINGS ---
#define STEPS_PER_REV 200  
#define MICROSTEPS 2       // Set to 2 as per your code

// Create stepper objects
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

// --- DATA LISTS ---
const float xAngles[] = {0, 10, 20, 30, 40, 45, 45, 90, 30, 45};
const float yAngles[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

// Laser values
const char* laserValues[] = {"true", "true", "true", "true", "true", "true", "true", "true", "true", "true"};

int numAngles = sizeof(xAngles) / sizeof(xAngles[0]);
int currentIndex = 0;

void setup() {
  Serial.begin(9600);
  
  // Configure Laser
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); 
  
  // Configure X Stepper (Physically the Y-Axis)
  stepperX.setMaxSpeed(500);       
  stepperX.setAcceleration(200);   
  stepperX.setCurrentPosition(0);  // Assume starting at 0
  
  // Configure Y Stepper (Physically the X-Axis)
  stepperY.setMaxSpeed(500);       
  stepperY.setAcceleration(200);   
  stepperY.setCurrentPosition(0);  // Assume starting at 0
  
  Serial.println("Dual Stepper Motor X-Y Angle Control");
  delay(1000);
}

void loop() {
  // Always run motors - MUST be at top of loop
  stepperX.run();
  stepperY.run();

  // Check if we reached the target
  if (stepperX.distanceToGo() == 0 && stepperY.distanceToGo() == 0) {
      
      // End of loop check
      if (currentIndex >= numAngles) {
          Serial.println("Sequence Complete.");
          currentIndex = 0;
          digitalWrite(LASER_PIN, LOW);
          delay(2000);
      }
      
      if (currentIndex < numAngles) {
          
          // 1. SET LASER (Synchronized)
          if (strcmp(laserValues[currentIndex], "true") == 0) {
             digitalWrite(LASER_PIN, HIGH);
          } else {
             digitalWrite(LASER_PIN, LOW);
          }
          
          // 2. MOVE MOTORS
          // We pass the data from the lists to the mover function
          moveToAngles(xAngles[currentIndex], yAngles[currentIndex]);
          
          // 3. INCREMENT
          currentIndex++;
          
          // 4. WAIT
          delay(1000); 
      }
  }
}

// Function to move motors with SWAPPED AXIS LOGIC
void moveToAngles(float targetXData, float targetYData) {
  
  Serial.print("Moving to: X_Data=");
  Serial.print(targetXData);
  Serial.print(" Y_Data=");
  Serial.println(targetYData);

  // --- THE SWAP ---
  // You said "X STEPPER IS THE Y COORDINATE"
  
  // 1. Send Y-Data to the X-Stepper (because X-Stepper controls Y-Axis)
  long stepsForStepperX = angleToAbsoluteSteps(targetYData);
  stepperX.moveTo(stepsForStepperX);

  // 2. Send X-Data to the Y-Stepper (because Y-Stepper controls X-Axis)
  long stepsForStepperY = angleToAbsoluteSteps(targetXData);
  stepperY.moveTo(stepsForStepperY);
}

// Convert angle directly to absolute step count
// Uses ABSOLUTE positioning to prevent gravity drift
long angleToAbsoluteSteps(float angle) {
  float stepsPerDegree = (STEPS_PER_REV * MICROSTEPS) / 360.0;
  return (long)(angle * stepsPerDegree);
}
