////////// NOTES \\\\\\\\\\
//UNIVERSAL COMMANDS
//1 = Start Command
//2 = Stop Command
//3 = Started Command
//4 = Ended Command
//***DO NOT USE 1, 2, 3, or 4 FOR ANY OTHER SERIAL COMMUNICATION***

//Include files
#include <Servo.h>

//Define inputs and outputs
#define limitSwitch 3
#define clockwiseSwitch 4
#define counterClockwiseSwitch 5
#define firingAllowedSwitch 6
#define startingPositionSwitch 7
int firingBarrel = 22;

//Variables
bool firingDisabled = false;

//Create Servos
Servo barrelMotor;

void fire(){
  digitalWrite(firingBarrel,HIGH);
  delay(250);
  digitalWrite(firingBarrel,LOW);
  //barrelMotor.write(180);
  //delay(500);
  //while(digitalRead(limitSwitch) == 0){
  //  barrelMotor.write(180);
  //}
  //barrelMotor.write(90);
  firingBarrel = firingBarrel + 1;
  Serial1.println(4,DEC);
}

void setup(){
  Serial.begin(9600);   //Computer
  Serial1.begin(9600);  //CPU Arduino

  pinMode(2,INPUT);
  
  pinMode(22,OUTPUT);
  pinMode(23,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(28,OUTPUT);
  pinMode(29,OUTPUT);
  
  digitalWrite(22,LOW);
  digitalWrite(23,LOW);
  digitalWrite(24,LOW);
  digitalWrite(25,LOW);
  digitalWrite(26,LOW);
  digitalWrite(27,LOW);
  digitalWrite(28,LOW);
  digitalWrite(29,LOW);
  
  pinMode(limitSwitch,INPUT);
  pinMode(clockwiseSwitch,INPUT);
  pinMode(counterClockwiseSwitch,INPUT);
  
  //Give servos pin
  barrelMotor.attach(2);
  //Turn servos off
  barrelMotor.write(90);
}

void loop(){
  //Recieve fire command
  if(Serial1.available() > 0){
    if(Serial1.parseInt() == 1){
      digitalWrite(firingBarrel,HIGH);
      delay(1000);
      digitalWrite(firingBarrel,LOW);
      firingBarrel = firingBarrel + 1;
      Serial1.println(4,DEC);
    }
  }
  //Manual barrel rotation
  if(digitalRead(clockwiseSwitch) == 1){
    barrelMotor.write(180);
    delay(500);
    while(digitalRead(limitSwitch) == 0){
      barrelMotor.write(180); 
    }
    barrelMotor.write(90);
  }
  if(digitalRead(counterClockwiseSwitch) == 1){
    barrelMotor.write(0);
    delay(500);
    while(digitalRead(limitSwitch) == 0){
      barrelMotor.write(0); 
    }
    barrelMotor.write(90);
  }
}
