////////// NOTES \\\\\\\\\\
//UNIVERSAL COMMANDS
//1 = Start Command
//2 = End Command
//3 = Started Response
//4 = Ended Response
//***DO NOT USE 1, 2, 3, or 4 FOR ANY OTHER SERIAL COMMUNICATION***

//LCD pins
#define RS 2
#define EN 3
#define DI4 4
#define DI5 5
#define DI6 6
#define DI7 7

//Reciever pins
#define testCh 8
#define rightDriveCh 8
#define leftDriveCh 9
#define Ch3 10
#define Ch4 11
#define airHornCh 12
#define Ch6 13

//Realy pins
#define airHornRelay 30

//Servo pins
#define frontRightMotorController 22
#define frontLeftMotorController 24
#define backRightMotorController 26
#define backLeftMotorController 28

//Include files
#include <Servo.h>

//Define inputs and outputs
Servo frontRight;
Servo frontLeft;
Servo backRight;
Servo backLeft;

//Control Variables
bool firing = false;
bool horns = false;
bool fireReleased = true;

//Safety Variables
int killTimer = millis();
int killDelay = 250;  //Wait this many millisecs for reciever response before shutting off systems

//Drive Variables
int yValue = 90;
int xValue = 90;
int rightPower = 90;
int leftPower = 90;
int turningDeadZone = 5;

//Channel Variables
int durationTestCh = 0;
int durationrightDriveCh = 0;
int durationleftDriveCh = 0;
int durationCh3 = 0;
int durationCh4 = 0;
int durationairHornCh = 0;
int durationCh6 = 0;
int previousTimeTestCh = micros();
int previousTimerightDriveCh = micros();
int previousTimeleftDriveCh = micros();
int previousTimeCh3 = micros();
int previousTimeCh4 = micros();
int previousTimeairHornCh = micros();
int previousTimeCh6 = micros();

void setup(){
  //Serial definitions
  Serial.begin(9600);   //Computer
  Serial1.begin(9600);  //Barrel Controller
  Serial2.begin(9600);  //Air Horn / Lighting Controller
  Serial3.begin(9600);  //AUX

  //Input definitions
  pinMode(testCh,INPUT);
  pinMode(rightDriveCh,INPUT); //xJoystick
  pinMode(leftDriveCh,INPUT); //yJoystick
  pinMode(Ch3,INPUT);
  pinMode(Ch4,INPUT);
  pinMode(airHornCh,INPUT); //Fire Cannon
  pinMode(Ch6,INPUT); //Horn enable

  //Output definitions
  //pinMode(airHornRelay,OUTPUT);
  //digitalWrite(airHornRelay,LOW);

  frontRight.attach(frontRightMotorController);
  frontLeft.attach(frontLeftMotorController);
  backRight.attach(backRightMotorController);
  backLeft.attach(backLeftMotorController);

  frontRight.write(90);
  frontLeft.write(90);
  backRight.write(90);
  backLeft.write(90);

  //Create GND pins for motor controller data wires
  pinMode(23,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(29,OUTPUT);
  digitalWrite(23,LOW);
  digitalWrite(25,LOW);
  digitalWrite(27,LOW);
  digitalWrite(29,LOW);
}

void loop(){
  updateChannels();
  tankDrive(1);
  //airHornControl();
}

void updateChannels(){
  killTimer = millis();
  //Update right drive channel
  while(digitalRead(rightDriveCh) == LOW){
    if((millis() - killTimer) > killDelay){ //If this channel signal stays low for this amount of time it turns everything off
      frontRight.write(90);
      frontLeft.write(90);
      backRight.write(90);
      backLeft.write(90);
      //Turn other things off
      //digitalWrite(airHornRelay,LOW);
    }
  }
  if(digitalRead(rightDriveCh) == HIGH){
    previousTimerightDriveCh = micros();
    while(digitalRead(rightDriveCh) == HIGH){}
    durationrightDriveCh = micros() - previousTimerightDriveCh;
  }
  //Update left drive channel
  while(digitalRead(leftDriveCh) == LOW){}
  if(digitalRead(leftDriveCh) == HIGH){
    previousTimeleftDriveCh = micros();
    while(digitalRead(leftDriveCh) == HIGH){}
    durationleftDriveCh = micros() - previousTimeleftDriveCh;
  }
  /*
  //Update air horn channel
  while(digitalRead(airHornCh) == LOW){}
  if(digitalRead(airHornCh) == HIGH){
    previousTimeairHornCh = micros();
    while(digitalRead(airHornCh) == HIGH){}
    durationairHornCh = micros() - previousTimeairHornCh;
  }
  //Update channel 6
  while(digitalRead(Ch6) == LOW){}
  if(digitalRead(Ch6) == HIGH){
    previousTimeCh6 = micros();
    while(digitalRead(Ch6) == HIGH){}
    durationCh6 = micros() - previousTimeCh6;
  }
  */
}

void oldTankDrive(float multiplier){
  //Drive using tank controls (2 sticks up and down)
  //This converts the channel duration (1070 to 1910) to 0 <= x <= 180 for servo control, constrain ensures the # is between 0 and 180
  rightPower = constrain((durationrightDriveCh-1070)/4.7,0,180);
  leftPower = constrain((durationleftDriveCh-1070)/4.7,0,180);
  //Converts power to -90 <= x <= 90
  rightPower = rightPower - 90;
  leftPower = leftPower - 90;
  //Divide the power by the divider, intended to limit the power of drive motors while still using the joysticks full range of motion
  rightPower = rightPower*multiplier;
  leftPower = leftPower*multiplier;
  //Converts power to 0 <= x <= 180
  rightPower = rightPower + 90;
  leftPower = leftPower + 90;
  //reverse right side
  rightPower = 180-rightPower;
  //Pass motor power to the motor controllers
  frontRight.write(constrain(rightPower,6,180));
  frontLeft.write(constrain(leftPower,6,180));
  backRight.write(constrain(rightPower,6,180));
  backLeft.write(constrain(leftPower,6,180));
}

void tankDive(float multiplier){
  //Convert joystic values to useable range
  rightPower = map(durationrightDriveCh,1000,2000,-90,90);
  leftPower = map(durationleftDriveCh,1000,2000,-90,90);

  //Account for multiplier
  rightPower *= multiplier;
  leftPower *= multiplier;
  
  //Convet useable range to motor controller values
  rightPower += 90;
  leftPower += 90;

  //Pass motor ocntroller values to motors
  frontRight.write(constrain(rightPower,6,180));
  frontLeft.write(constrain(leftPower,6,180));
  backRight.write(constrain(rightPower,6,180));
  backLeft.write(constrain(leftPower,6,180));
}

void arcadeDrive(){
  //Drive using arcade controls (1 stick up, down, left, and right)
  //turningDeadZone compensates for incidental turning when stick is almost stait up or down
  yValue = map(durationrightDriveCh,1000,2000,-90,90); //Converts to 90 > y > -90
  xValue = map(durationrightDriveCh,1000,2000,-90,90); //Converts to 90 > x > -90

  if(yValue > 0){ //Forward
    if(xValue > turningDeadZone){ //Right
      rightPower = yValue - xValue;
      leftPower = (yValue + xValue)/2;
    }
    if(xValue < -turningDeadZone){ //Left
      rightPower = (yValue - xValue)/2;
      leftPower = yValue + xValue;
    }
    if(xValue >= -turningDeadZone && xValue <= turningDeadZone){  //Strait
      rightPower = 90;
      leftPower = 90;
    }
  }
  if(yValue < 0){ //Reverse
    if(xValue > turningDeadZone){ //Right
      rightPower = (yValue - xValue)/2;
      leftPower = yValue + xValue;
    }
    if(xValue < -turningDeadZone){ //Left
      rightPower = yValue - xValue;
      leftPower = (yValue + xValue)/2;
    }
    if(xValue >= -turningDeadZone && xValue <= turningDeadZone){  //Strait
      rightPower = -90;
      leftPower = -90;
    }
  }
}

void barrelControl(){
  //Barrel Control
  if((durationairHornCh >= 2000) && (firing == false) && (fireReleased == true)){
    Serial1.println(1,DEC);
    firing = true;
    fireReleased = false;
  }
  if(Serial1.available() > 0){
    if(Serial1.parseInt() == 4){
      firing = false;
    }
  }
  if((fireReleased == false) && (durationairHornCh <= 1000)){
    fireReleased = true;
  }
}
