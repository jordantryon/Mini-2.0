// reciever pins
#define hornCh 4

// relay pins
#define rightHorn 3
#define leftHorn 2

// safety Variables
int killTimer = millis();
int killDelay = 250;  //Wait this many millisecs for reciever response before shutting off systems

// channel Variables
int durationHornCh = 0;
int previousTimeHornCh = micros();

void setup(){
  // open serial communication
  Serial.begin(9600);

  // input definitions
  pinMode(hornCh,INPUT);
  
  // output definitions
  pinMode(rightHorn,OUTPUT);
  pinMode(leftHorn,OUTPUT);

  // turn off horns
  digitalWrite(rightHorn,LOW);
  digitalWrite(leftHorn,LOW);
}

void loop(){
  updateChannels();
  if(durationHornCh <= 1500){
    digitalWrite(rightHorn,HIGH);
    digitalWrite(leftHorn,HIGH);
  } else {
    digitalWrite(rightHorn,LOW);
    digitalWrite(leftHorn,LOW);
  }
}

void updateChannels(){
  killTimer = millis();
  while(digitalRead(hornCh) == LOW){
    if((millis() - killTimer) > killDelay){ // if this receiver channel signal stays low for the duration of the killDelay, turn everything off (transmitter and receiver have lost connection)
      // turn off horns
      digitalWrite(rightHorn,LOW);
      digitalWrite(leftHorn,LOW);
    }
  }
  if(digitalRead(hornCh) == HIGH){
    previousTimeHornCh = micros();
    while(digitalRead(hornCh) == HIGH){}
    durationHornCh = micros() - previousTimeHornCh;
  }
}
