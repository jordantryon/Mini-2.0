// include files
#include <Servo.h>

// define inputs and outputs
#define motorControllerRightPort 2
#define motorControllerLeftPort 3
#define motorControllerRotPort 4
#define launchCh 5
#define cSwitch 6
#define ccSwitch 7
#define enableLaunchSwitch 8
#define zeroSwitchRight 9
#define zeroSwitchLeft 10
#define zeroSwitchRot 11
#define angleCh 12

#define encoderRightPortA 22
#define encoderRightPortB 23
#define encoderLeftPortA 24
#define encoderLeftPortB 25
#define encoderRotPortA 26
#define encoderRotPortB 27

#define relayGray 53    // gray
#define relayGrayS 51   // gray -
#define relayPurple 49  // purple
#define relayPurpleS 47 // purple -
#define relayWhite 45   // white
#define relayWhiteS 43  // white -
#define relayGreen 41   // green
#define relayGreenS 39  // green -

// create servo objects (motors)
Servo rightMotor;
Servo leftMotor;
Servo rotMotor;

// initiallize right encoder vars
int rightPos = 0;
int rightAState;
int rightALastState;

// initiallize left encoder vars
int leftPos = 0;
int leftAState;
int leftALastState;

// initiallize rotation encoder vars
int rotPos = 0;
int rotAState;
int rotALastState;

// initiallize channel variables
int durationLaunchCh = 0;
int durationAngleCh = 0;
int previousTimeLaunchCh = micros();
int previousTimeAngleCh = micros();

// initiallize safety variables (this does nothing with the new DX8 transmitter and AR8000 reciever)
int killTimer = millis();
int killDelay = 250;  //Wait this many millisecs for reciever response before shutting off systems

// initiallize motor speed vars
const int minVal = 6;                     //Min value able to send to servo (victor abs min: 6)
const int maxVal = 205;                   //Max value able to be sent to servo (victor abs max: 205)
const int neutralVal = (maxVal-minVal)/2; //Neutral servo value
const int speedRange = 100;               //Min and max val (+/-) able to be used in convertSpeed(int val), goToPos(int p,int s)

// initiallize speed matching vars
const int checkDelay = 1000;              //Delay (ms) between checking the differnce in two motors positions
int prevTime = millis();                  //Time of previous check

// initiallize angle change vars
int lastAngleState = -1;

// initiallize barrel vars
int currentBarrel = 1;

bool lastEnableSwitchState = true;

void setup(){
  // open serial communication
  Serial.begin(9600);

  // give servo objects their pin
  rightMotor.attach(motorControllerRightPort);
  leftMotor.attach(motorControllerLeftPort);
  rotMotor.attach(motorControllerRotPort);

  // set motor controllers to neutral
  rightMotor.write(neutralVal);
  leftMotor.write(neutralVal);
  rotMotor.write(neutralVal);

  // define encoder pins as inputs
  pinMode(encoderLeftPortA,INPUT);
  pinMode(encoderLeftPortB,INPUT);
  pinMode(encoderRightPortA,INPUT);
  pinMode(encoderRightPortB,INPUT);

  // initialize encoder state vars
  rightALastState = digitalRead(encoderRightPortA);
  leftALastState = digitalRead(encoderLeftPortA);
  rotALastState = digitalRead(encoderRotPortA);

  // define zeroSwitch pins as inputs
  pinMode(zeroSwitchRight,INPUT_PULLUP);
  pinMode(zeroSwitchLeft,INPUT_PULLUP);
  pinMode(zeroSwitchRot,INPUT_PULLUP);

  // define push buttons and switches as inputs
  pinMode(enableLaunchSwitch,INPUT_PULLUP);
  pinMode(cSwitch,INPUT_PULLUP);
  pinMode(ccSwitch,INPUT_PULLUP);

  // define relays as outputs
  pinMode(relayGreenS,OUTPUT);
  pinMode(relayGreen,OUTPUT);
  pinMode(relayWhiteS,OUTPUT);
  pinMode(relayWhite,OUTPUT);
  pinMode(relayPurpleS,OUTPUT);
  pinMode(relayPurple,OUTPUT);
  pinMode(relayGrayS,OUTPUT);
  pinMode(relayGray,OUTPUT);

  // set all relays to the off state initially
  digitalWrite(relayGreenS,LOW);
  digitalWrite(relayGreen,LOW);
  digitalWrite(relayWhiteS,LOW);
  digitalWrite(relayWhite,LOW);
  digitalWrite(relayPurpleS,LOW);
  digitalWrite(relayPurple,LOW);
  digitalWrite(relayGrayS,LOW);
  digitalWrite(relayGray,LOW);

  delay(500);
  zeroLift();
  delay(500);
  zeroRot();
  delay(500);
}

void loop(){
  // update rx channels
  updateChannels();

  // changing barrel angle
  if(lastAngleState != 2 && (durationAngleCh < 1367)){                            // switch G position 2, barrel angle 50
    liftToPos(28000,80);
    lastAngleState = 2;
  }
  if(lastAngleState != 1 && (durationAngleCh > 1367 && durationAngleCh < 1634)){  // switch G position 1, barrel angle 33
    liftToPos(20000,80);
    lastAngleState = 1;
  }
  if(lastAngleState != 0 && (durationAngleCh > 1634)){                            // switch G position 0, barrel angle 0
    liftToPos(1000,80);
    lastAngleState = 0;
  }

  // barrel rotation and launching sequence
  if(digitalRead(enableLaunchSwitch) == LOW){  // if the enable switch is on, disable manual barrel rotation and enable the launching sequence
    if(lastEnableSwitchState == false){
      rotToPos((currentBarrel-1)*1375,35);
      lastEnableSwitchState = true;
    }
    checkEncoders();
    if(durationLaunchCh <= 1500){ // if the launch switch is pressed
      int launchSwitchCount = 0;
      for(int i=0;i<8;i++){
        updateChannels();
        if(durationLaunchCh <= 1500){
          launchSwitchCount++;
        }
      }
      if(launchSwitchCount > 6){
        switch(currentBarrel){
          case 1:
            digitalWrite(relayGreen,HIGH);
            delay(500);
            digitalWrite(relayGreen,LOW);
            break;
          case 2:
            digitalWrite(relayGrayS,HIGH);
            delay(500);
            digitalWrite(relayGrayS,LOW);
            break;
          case 3:
            digitalWrite(relayGray,HIGH);
            delay(500);
            digitalWrite(relayGray,LOW);
            break;
          case 4:
            digitalWrite(relayPurpleS,HIGH);
            delay(500);
            digitalWrite(relayPurpleS,LOW);
            break;
          case 5:
            digitalWrite(relayPurple,HIGH);
            delay(500);
            digitalWrite(relayPurple,LOW);
            break;
          case 6:
            digitalWrite(relayWhiteS,HIGH);
            delay(500);
            digitalWrite(relayWhiteS,LOW);
            break;
          case 7:
            digitalWrite(relayGreenS,HIGH);
            delay(500);
            digitalWrite(relayGreenS,LOW);
            break;
          case 8:
            digitalWrite(relayGrayS,HIGH);
            delay(500);
            digitalWrite(relayGrayS,LOW);
            break;
          default:
            break;
        }
        if(currentBarrel < 8){
          rotToPos((currentBarrel)*1375,35);
          currentBarrel++;
        }
      }
    }
  } else {  // if the enable switch is off, enable manual barrel rotation and disable the launching sequence
    if(lastEnableSwitchState == true){
      lastEnableSwitchState = false;
    }
    while(digitalRead(cSwitch) == LOW){  // while the clockwise switch is pressed
      rotMotor.write(convertSpeed(-35));
      checkEncoders();
    }
    while(digitalRead(ccSwitch) == LOW){ // while the counter-clockwise switch is pressed
      rotMotor.write(convertSpeed(35));
      checkEncoders();
    }
    rotMotor.write(neutralVal);
    checkEncoders();
  }
}

void rotToPos(int p,int s){
  s = constrain(s,0,speedRange);
  int rotSpeed = s;

  if(p > rotPos){ //If target pos > current pos
    rotMotor.write(convertSpeed(-rotSpeed));
    while(rotPos < p){checkEncoders();}
  } else {        //If target pos < current pos
    rotMotor.write(convertSpeed(rotSpeed));
    while(rotPos > p){checkEncoders();}
  }

  rotMotor.write(neutralVal);
}

void liftToPos(int p,int s){
  p = constrain(p,0,28000);
  s = constrain(s,0,speedRange);
  int rightSpeed = s;
  int leftSpeed = s;
  prevTime = millis();

  if(p > rightPos){ //If target pos > current pos
    rightSpeed = -rightSpeed;
    leftSpeed = -leftSpeed;
    rightMotor.write(convertSpeed(rightSpeed));
    leftMotor.write(convertSpeed(leftSpeed));
    while(rightPos < p || leftPos < p){
      if(millis() - prevTime >= checkDelay){
        if(rightPos > leftPos){
          rightMotor.write(convertSpeed(++rightSpeed));
        } else {
          rightMotor.write(convertSpeed(--rightSpeed));
        }
        prevTime = millis();
      }
      if(rightPos >= p){
        rightMotor.write(neutralVal);
      }
      if(leftPos >= p){
        leftMotor.write(neutralVal);
      }
      checkEncoders();
    }
  } else {  //If target pos < current pos
    rightMotor.write(convertSpeed(rightSpeed));
    leftMotor.write(convertSpeed(leftSpeed));
    while(rightPos > p || leftPos > p){
      if(millis() - prevTime >= checkDelay){
        if(rightPos < leftPos){
          rightMotor.write(convertSpeed(--rightSpeed));
        } else {
          rightMotor.write(convertSpeed(++rightSpeed));
        }
        prevTime = millis();
      }
      if(rightPos <= p){
        rightMotor.write(neutralVal);
      }
      if(leftPos <= p){
        leftMotor.write(neutralVal);
      }
      checkEncoders();
    }
  }

  //Ensure motors are stopped
  rightMotor.write(neutralVal);
  leftMotor.write(neutralVal);
}

void zeroRot(){
  while(digitalRead(zeroSwitchRot) == HIGH){
    rotMotor.write(neutralVal+25);
  }
  rotMotor.write(neutralVal);
  delay(500);
  rotPos = 0;
}

void zeroLift(){
  rightMotor.write(maxVal);
  leftMotor.write(maxVal);
  while(digitalRead(zeroSwitchRight) == HIGH || digitalRead(zeroSwitchLeft) == HIGH){
    if(digitalRead(zeroSwitchRight) == LOW){
      rightMotor.write(neutralVal);
    }
    if(digitalRead(zeroSwitchLeft) == LOW){
      leftMotor.write(neutralVal);
    }
  }
  // ensure motors are stopped
  rightMotor.write(neutralVal);
  leftMotor.write(neutralVal);

  rightPos = 0;
  leftPos = 0;
  delay(1000);
  liftToPos(500,100);
  delay(500);

  rightMotor.write(neutralVal + 10);
  leftMotor.write(neutralVal + 10);
  while(digitalRead(zeroSwitchRight) == HIGH || digitalRead(zeroSwitchLeft) == HIGH){
    if(digitalRead(zeroSwitchRight) == LOW){
      rightMotor.write(neutralVal);
    }
    if(digitalRead(zeroSwitchLeft) == LOW){
      leftMotor.write(neutralVal);
    }
  }
  // ensure motors are stopped
  rightMotor.write(neutralVal);
  leftMotor.write(neutralVal);

  delay(500);

  rightPos = 0;
  leftPos = 0;
}

void checkEncoders(){
  rightAState = digitalRead(encoderRightPortA);
  leftAState = digitalRead(encoderLeftPortA);
  rotAState = digitalRead(encoderRotPortA);

  if(rightAState != rightALastState){
    if(digitalRead(encoderRightPortB) != rightAState){
      rightPos++;
    } else {
      rightPos--;
    }
  }

  if(leftAState != leftALastState){
    if(digitalRead(encoderLeftPortB) != leftAState){
      leftPos++;
    } else {
      leftPos--;
    }
  }

  if(rotAState != rotALastState){
    if(digitalRead(encoderRotPortB) != rotAState){
      rotPos++;
    } else {
      rotPos--;
    }
  }

  rightALastState = rightAState;
  leftALastState = leftAState;
  rotALastState = rotAState;
}

void updateChannels(){
  killTimer = millis();
  while(digitalRead(launchCh) == LOW){
    if((millis() - killTimer) > killDelay){ // if this receiver channel signal stays low for the duration of the killDelay, turn everything off (transmitter and receiver have lost connection)
      // turn off all relays
      digitalWrite(relayGreenS,LOW);
      digitalWrite(relayGreen,LOW);
      digitalWrite(relayWhiteS,LOW);
      digitalWrite(relayWhite,LOW);
      digitalWrite(relayPurpleS,LOW);
      digitalWrite(relayPurple,LOW);
      digitalWrite(relayGrayS,LOW);
      digitalWrite(relayGray,LOW);
    }
  }
  if(digitalRead(launchCh) == HIGH){
    previousTimeLaunchCh = micros();
    while(digitalRead(launchCh) == HIGH){}
    durationLaunchCh = micros() - previousTimeLaunchCh;
  }
  while(digitalRead(angleCh) == LOW){}
  if(digitalRead(angleCh) == HIGH){
    previousTimeAngleCh = micros();
    while(digitalRead(angleCh) == HIGH){}
    durationAngleCh = micros() - previousTimeAngleCh;
  }
}

int convertSpeed(int input){
  return map(input,-speedRange,speedRange,minVal,maxVal);
}

void calibrateMotorControllers(){
  int iterations = 3; // number of times to move virtual stick
  int x = neutralVal;   // current virtual stick position

  // set virtual stick to neutral value
  rotMotor.write(x);
  delay(250);

  for(int t=0;t<iterations;t++){
    // move virtual stick to maximum value
    while(x < maxVal){
      rotMotor.write(++x);
      delay(4);
    }
    delay(250);
    // move virtual stick to minimum value
    while(x > minVal){
      rotMotor.write(--x);
      delay(4);
    }
    delay(250);
  }

  // move virtual stick to neutral value
  while(x < neutralVal){
    rotMotor.write(++x);
    delay(4);
  }

  delay(250);
}
