#include <SoftwareSerial.h> //library to use the bluetooth module classes.
// HC-06 Bluetooth module connected to A0 (RX) and A1 (TX)
SoftwareSerial hc06(15, 14); // A0 = 14: RX, A1 = 15: TX Note: This is the object declaration for the hc06 bluetooth module.
//If you are wondering why there are no function prototypes, it's because Arduino IDE compiler does not require them.
char currentCommand = 'F'; // Initialize with 'F' (Stop)

// Motor driver pin definitions
// L298N-A (Left Side: Back Left and Front Left)
const int in1_A = 13; // Back Left (Motor 1) IN1
const int in2_A = 7;  // Back Left (Motor 1) IN2
const int in3_A = 8; // Front Left (Motor 4) IN3 Note: On rebuild, i accidentally flipped IN 3 & IN 4, so i flipped them in the code here, it's supposed to be 12 and in4_A = 8, but just leave it like this 
const int in4_A = 12;  // Front Left (Motor 4) IN4
const int ENA_A = 10; // PWM for Back Left (Motor 1) & Front Left (Motor 4) Note: the ENA pins are the speed controllers and they need to be on a PWM input pin, dont change any pins.

// L298N-B (Right Side: Back Right and Front Right)
const int in1_B = 2;  // Back Right (Motor 2) IN1
const int in2_B = 3;  // Back Right (Motor 2) IN2
const int in3_B = 5;  // Front Right (Motor 3) IN3 / I also had these flipped, so it's supposed to be in3_B = 4 in4_B = 5 but leave it as is.
const int in4_B = 4;  // Front Right (Motor 3) IN4
const int ENA_B = 11; // PWM for Back Right (Motor 2) & Front Right (Motor 3)

// Speed values (0-255 for PWM)
int speed_A = 50; // Speed for Left Side Note: can go up to 255 but i dont see a huge difference and prefer conserving the battery
int speed_B = 50; // Speed for Right Side

void setup() {
  // Set motor driver pins as outputs
  pinMode(in1_A, OUTPUT); //Positive Back Left
  pinMode(in2_A, OUTPUT); //Negative Back Left
  pinMode(in3_A, OUTPUT); //Positive Front Left
  pinMode(in4_A, OUTPUT); //Negative Front Left
  pinMode(ENA_A, OUTPUT); 

  pinMode(in1_B, OUTPUT); //Positive Back Right
  pinMode(in2_B, OUTPUT); //Negative Back Right
  pinMode(in3_B, OUTPUT); //Positive Front Right
  pinMode(in4_B, OUTPUT); //Negative Front Left
  pinMode(ENA_B, OUTPUT);

  // Start with motors off
  stopMotors();

  // Initialize serial communications
  Serial.begin(9600); // For debugging via Serial Monitor
  hc06.begin(9600);   // For HC-06 communication

  Serial.println("Bluetooth Control Ready. Enter commands: W (forward), S (backward), A (left), D (right), F (stop)");
}

void loop() {
  // Check for a new command from HC-06
  if (hc06.available()) {
    char command = hc06.read(); // Read the incoming character from HC-06
    command = toupper(command); // Convert to uppercase

    // Validate the command
    if (command == 'W' || command == 'S' || command == 'A' || command == 'D' || command == 'F' || command == 'Q'|| command == 'E') {
      currentCommand = command; // Update the current command / Note: This just ensures the command inputted stays looping until otherwise.
      Serial.print("Command received: ");
      Serial.println(currentCommand);
    } else {
      Serial.println("Invalid command.");
    }
  }

  // Execute the current command continuously
  switch (currentCommand) {
    case 'W': // Move Forward
      moveForward();
      break;
    case 'S': // Move Backward
      moveBackward();
      break;
    case 'A': // Strafe Left
      strafeLeft();
      break;
    case 'D': // Strafe Right
      strafeRight();
      break;
    case 'E': //Turn Right
      turnRight(); 
      break;
    case 'Q': //Turn Left
      turnLeft(); 
      break;
    case 'F': // Stop
      stopMotors();
      break;
    default:
      stopMotors(); // Default to stop if something goes wrong
      break;
  }
}

//Movement functions for mecanum wheels // Refer to the const pins for each wheel //HIGH, LOW = Forward ; LOW, HIGH = Backward ;LOW, LOW / HIGH, HIGH = OFF
void moveForward() {
  digitalWrite(in1_A, HIGH); digitalWrite(in2_A, LOW); 
  digitalWrite(in3_A, HIGH); digitalWrite(in4_A, LOW);
  digitalWrite(in1_B, HIGH); digitalWrite(in2_B, LOW);
  digitalWrite(in3_B, HIGH); digitalWrite(in4_B, LOW);
  analogWrite(ENA_A, speed_A);
  analogWrite(ENA_B, speed_B);
}

void moveBackward() {
  digitalWrite(in1_A, LOW); digitalWrite(in2_A, HIGH);
  digitalWrite(in3_A, LOW); digitalWrite(in4_A, HIGH);
  digitalWrite(in1_B, LOW); digitalWrite(in2_B, HIGH);
  digitalWrite(in3_B, LOW); digitalWrite(in4_B, HIGH);
  analogWrite(ENA_A, speed_A);
  analogWrite(ENA_B, speed_B);
}

void strafeRight() {
  digitalWrite(in1_A, LOW); digitalWrite(in2_A, HIGH); //Front Right moves backward
  digitalWrite(in3_A, HIGH); digitalWrite(in4_A, LOW); //Back Right moves Forward
  digitalWrite(in1_B, HIGH); digitalWrite(in2_B, LOW); //Front Left moves Forward
  digitalWrite(in3_B, LOW); digitalWrite(in4_B, HIGH); //Back Left moves Backward
  analogWrite(ENA_A, speed_A);
  analogWrite(ENA_B, speed_B);
}

void strafeLeft() {
  digitalWrite(in1_A, HIGH); digitalWrite(in2_A, LOW); //Front Right moves Forward
  digitalWrite(in3_A, LOW); digitalWrite(in4_A, HIGH); //Back Right moves Backward
  digitalWrite(in1_B, LOW); digitalWrite(in2_B, HIGH); //Front Left mvoes Backward
  digitalWrite(in3_B, HIGH); digitalWrite(in4_B, LOW); //Back Left moves Forward
  analogWrite(ENA_A, speed_A);
  analogWrite(ENA_B, speed_B);
}

//The turn right and left is optional, strafing should be sufficient, I understand it would be complicated to make the car turn from your hand gesture.
//But I included it just in case, if you don't wanna use it just comment it out.

 void turnRight() {
  // Left Side Motors (Back Left and Front Left)
  digitalWrite(in1_A, HIGH); digitalWrite(in2_A, LOW);  //Back Left moves forward
  digitalWrite(in3_A, HIGH); digitalWrite(in4_A, LOW);  //Front Left moves forward

  // Right Side Motors (Back Right and Front Right)
  digitalWrite(in1_B, LOW); digitalWrite(in2_B, HIGH);  //Back Right moves backward
  digitalWrite(in3_B, LOW); digitalWrite(in4_B, HIGH);  //Front Right moves backward

  // Set speed for all motors
  analogWrite(ENA_A, speed_A);
  analogWrite(ENA_B, speed_B);
}

void turnLeft() {
  // Left Side Motors
  digitalWrite(in1_A, LOW); digitalWrite(in2_A, HIGH);  //Back Left moves backward
  digitalWrite(in3_A, LOW); digitalWrite(in4_A, HIGH);  //Front Left moves backward

  // Right Side Motors
  digitalWrite(in1_B, HIGH); digitalWrite(in2_B, LOW);  //Back Right moves forward
  digitalWrite(in3_B, HIGH); digitalWrite(in4_B, LOW);  //Front Right moves forward

  // Set speed for all motors
  analogWrite(ENA_A, speed_A);
  analogWrite(ENA_B, speed_B);
}

void stopMotors() {
  digitalWrite(in1_A, LOW); digitalWrite(in2_A, LOW);
  digitalWrite(in3_A, LOW); digitalWrite(in4_A, LOW);
  digitalWrite(in1_B, LOW); digitalWrite(in2_B, LOW);
  digitalWrite(in3_B, LOW); digitalWrite(in4_B, LOW);
  analogWrite(ENA_A, 0);
  analogWrite(ENA_B, 0);
}
