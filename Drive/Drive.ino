// include files
#include <Servo.h>

// reciever pins
#define rightDriveCh 2
#define leftDriveCh 3

// servo pins
#define frontRightMotorController 4
#define frontLeftMotorController 5
#define backRightMotorController 6
#define backLeftMotorController 7

// define inputs and outputs
Servo frontRight;
Servo frontLeft;
Servo backRight;
Servo backLeft;

// safety Variables
int killTimer = millis();
int killDelay = 250;  //Wait this many millisecs for reciever response before shutting off systems 250

//Declare motor speed vars
const int minVal = 6;                     //Min value able to send to servo (victor abs min: 6)
const int maxVal = 205;                   //Max value able to be sent to servo (victor abs max: 205)
const int neutralVal = (maxVal-minVal)/2; //Neutral servo value
const int speedRange = 100;               //Min and max val (+/-) able to be used in convertSpeed(int val), goToPos(int p,int s)

// channel Variables
int durationrightDriveCh = 0;
int durationleftDriveCh = 0;
int previousTimerightDriveCh = micros();
int previousTimeleftDriveCh = micros();

void setup(){
  // open serial communication
  Serial.begin(9600);

  // input definitions
  pinMode(rightDriveCh,INPUT);
  pinMode(leftDriveCh,INPUT);

  // give servo objects their pins
  frontRight.attach(frontRightMotorController);
  frontLeft.attach(frontLeftMotorController);
  backRight.attach(backRightMotorController);
  backLeft.attach(backLeftMotorController);

  // put motor controllers in neutral
  frontRight.write(neutralVal);
  frontLeft.write(neutralVal);
  backRight.write(neutralVal);
  backLeft.write(neutralVal);
}

void loop(){
  updateChannels();
  tankDrive();
}

void tankDrive(){
  int rightPower;
  int leftPower;
  int minRxVal = 1100;
  int neutralRxVal = 1500;
  int maxRxVal = 1900;
  int joystickDeadZone = 4;

  if(abs(neutralRxVal-durationrightDriveCh) <= joystickDeadZone){
    rightPower = neutralVal;
  } else {
    rightPower = constrain(durationrightDriveCh,minRxVal,maxRxVal);
    rightPower = map(durationrightDriveCh,minRxVal,maxRxVal,maxVal,minVal);
  }

  if(abs(neutralRxVal-durationleftDriveCh) <= joystickDeadZone){
    leftPower = neutralVal;
  } else {
    leftPower = constrain(durationleftDriveCh,minRxVal,maxRxVal);
    leftPower = map(durationleftDriveCh,minRxVal,maxRxVal,minVal,maxVal);
  }
  
  //Pass motor power to the motor controllers
  frontRight.write(rightPower);
  frontLeft.write(leftPower);
  backRight.write(rightPower);
  backLeft.write(leftPower);
}

void updateChannels(){
  killTimer = millis();
  while(digitalRead(rightDriveCh) == LOW){
    if((millis() - killTimer) > killDelay){ // if this receiver channel signal stays low for the duration of the killDelay, turn everything off (transmitter and receiver have lost connection)
      // set drive motors to neutral values
      frontRight.write(neutralVal);
      frontLeft.write(neutralVal);
      backRight.write(neutralVal);
      backLeft.write(neutralVal);
    }
  }
  if(digitalRead(rightDriveCh) == HIGH){
    previousTimerightDriveCh = micros();
    while(digitalRead(rightDriveCh) == HIGH){}
    durationrightDriveCh = micros() - previousTimerightDriveCh;
  }
  while(digitalRead(leftDriveCh) == LOW){}
  if(digitalRead(leftDriveCh) == HIGH){
    previousTimeleftDriveCh = micros();
    while(digitalRead(leftDriveCh) == HIGH){}
    durationleftDriveCh = micros() - previousTimeleftDriveCh;
  }
}

void calibrateMotorControllers(){
  int iterations = 3; // number of times to move virtual stick
  int x = neutralVal;   // current virtual stick position

  // set virtual stick to neutral value
  frontLeft.write(x);
  delay(250);

  for(int t=0;t<iterations;t++){
    // move virtual stick to maximum value
    while(x < maxVal){
      frontLeft.write(++x);
      delay(4);
    }
    delay(250);
    // move virtual stick to minimum value
    while(x > minVal){
      frontLeft.write(--x);
      delay(4);
    }
    delay(250);
  }

  // move virtual stick to neutral value
  while(x < neutralVal){
    frontLeft.write(++x);
    delay(4);
  }

  delay(250);
}
