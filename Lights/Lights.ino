#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <gamma.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#ifndef PSTR
  #define PSTR // Make Arduino Due happy
#endif

// reciever pins
#define lightCh 4

// channel Variables
int durationLightCh = 0;
int previousTimeLightCh = micros();

// define right display
#define pin1 2  //data pin the matrix is connected to
#define hPixels1 32 //number of horixontal pixels per board
#define vPixels1 8 //number of vertical pixels per board
#define hBoards1 2 //number of horizontal boards
#define vBoards1 1 //number of vertical boards
Adafruit_NeoMatrix rightPanel = Adafruit_NeoMatrix(hPixels1,vPixels1,hBoards1,vBoards1,pin1,NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,NEO_GRB + NEO_KHZ800);

// define left display
#define pin2 3  //data pin the matrix is connected to
#define hPixels2 32 //number of horixontal pixels per board
#define vPixels2 8 //number of vertical pixels per board
#define hBoards2 2 //number of horizontal boards
#define vBoards2 1 //number of vertical boards
Adafruit_NeoMatrix leftPanel = Adafruit_NeoMatrix(hPixels2,vPixels2,hBoards2,vBoards2,pin2,NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,NEO_GRB + NEO_KHZ800);

////////////////////////////////////
// |                            | //
// |  List Of Helpful Functions | //
// V                            V //
////////////////////////////////////

//PRINTING TEXT
//matrix.Color(r,g,b) determines the color value based on r g b
//matrix.setCursor(x,y) moves cursor to coordinates x,y
//matrix.setTextColor(c) determines the color c of text
//matrix.setTextSize(s) sets the size s of text, 1 = 8 pixels high
//matrix.print() print charachers to cursor posistion

//PRINTING SHAPES
//matrix.drawPixel(x,y,c) dispalys c color at coordinates x,y
//matrix.drawLine(x1,y1,x2,x2,c) draws a line from (x1,y1) to (x2,y2) in c color
//matrix.drawRect(x1,y1,x2,x2,c) draws a rectangle outline from (x1,y1) to (x2,y2) in c color
//matrix.fillRect(x1,y1,x2,x2,c) draws a solid rectangle from (x1,y1) to (x2,y2) in c color
//matrix.drawCircle(cp,r,c) draws a circle outline from cp center point to r radius in c color
//matrix.fillCircle(x,y,r,c) draws a solid circle from x,y center point to r radius in c color
//matrix.fillScreen(c) fills screen with c color

void setup(){
  // input definitions
  pinMode(lightCh,INPUT);

  // set up right display
  rightPanel.begin();
  rightPanel.fillScreen(0);
  rightPanel.setTextWrap(false);
  rightPanel.setBrightness(64);
  rightPanel.setCursor(0,0);
  rightPanel.setTextColor(rightPanel.Color(0,255,255));
  rightPanel.show();

  // set up left display
  leftPanel.begin();
  leftPanel.fillScreen(0);
  leftPanel.setTextWrap(false);
  leftPanel.setBrightness(64);
  leftPanel.setCursor(0,0);
  leftPanel.setTextColor(leftPanel.Color(0,255,255));
  leftPanel.show();
}

void loop(){
  // update rx channels
  updateChannels();

  // changing display mode
  if(durationLightCh < 1367){                           // switch B position 2
    teamMiniScroll();
    goLakersScroll();
  }
  if(durationLightCh > 1367 && durationLightCh < 1634){ // switch B position 1
    leftPanel.fillScreen(leftPanel.Color(100,0,0));
    rightPanel.fillScreen(rightPanel.Color(100,0,0));
    leftPanel.show();
    rightPanel.show();
    delay(250);
  }
  if(durationLightCh > 1634){                           // switch B position 0
    leftPanel.fillScreen(leftPanel.Color(0,100,0));
    rightPanel.fillScreen(rightPanel.Color(0,100,0));
    leftPanel.show();
    rightPanel.show();
    delay(250);
  }
}

void updateChannels(){
  while(digitalRead(lightCh) == LOW){}
  if(digitalRead(lightCh) == HIGH){
    previousTimeLightCh = micros();
    while(digitalRead(lightCh) == HIGH){}
    durationLightCh = micros() - previousTimeLightCh;
  }
}

void scrollText(Adafruit_NeoMatrix matrix,String str){
  int len = (str.length()+1)*5;
  for(int i=matrix.width();i>-len;i--){
    matrix.setCursor(i,0);
    matrix.print(str);
    matrix.show();
    matrix.fillScreen(0);
    delay(100);
    Serial.println("loop");
  }
  Serial.println("not");
}

void mini2(){
  leftPanel.fillScreen(0);
  rightPanel.fillScreen(0);
  rightPanel.setTextColor(rightPanel.Color(0,255,0));
  leftPanel.setTextColor(leftPanel.Color(0,255,0));
  rightPanel.setCursor(8,0);
  leftPanel.setCursor(8,0);
  rightPanel.print("MINI 2.0");
  leftPanel.print("MINI 2.0");
  rightPanel.show();
  leftPanel.show();
}

void mini2Scroll(){
  for(int i=64; i>-64; i--){
    leftPanel.fillScreen(leftPanel.Color(0,175,0));
    rightPanel.fillScreen(rightPanel.Color(0,175,0));
    rightPanel.setTextColor(rightPanel.Color(255,255,0));
    leftPanel.setTextColor(leftPanel.Color(255,255,0));
    rightPanel.setCursor(i,0);
    leftPanel.setCursor(i,0);
    rightPanel.print("MINI 2.0");
    leftPanel.print("MINI 2.0");
    rightPanel.show();
    leftPanel.show();
    rightPanel.fillScreen(0);
    leftPanel.fillScreen(0);
    //delay(30);
  }
}

void goBucsScroll(){
  for(int i=64; i>-64; i--){
    leftPanel.fillScreen(leftPanel.Color(0,0,200));
    rightPanel.fillScreen(rightPanel.Color(0,0,200));
    rightPanel.setTextColor(rightPanel.Color(255,255,255));
    leftPanel.setTextColor(leftPanel.Color(255,255,255));
    rightPanel.setCursor(i,0);
    leftPanel.setCursor(i,0);
    rightPanel.print("GO BUCS!");
    leftPanel.print("GO BUCS!");
    leftPanel.show();
    rightPanel.show();
  }
}

void goBucsStrobe(){
  leftPanel.fillScreen(leftPanel.Color(0,0,200));
  rightPanel.fillScreen(rightPanel.Color(0,0,200));
  rightPanel.setTextColor(rightPanel.Color(255,255,255));
  leftPanel.setTextColor(leftPanel.Color(255,255,255));
  rightPanel.setCursor(9,0);
  leftPanel.setCursor(9,0);
  rightPanel.print("GO BUCS!");
  leftPanel.print("GO BUCS!");
  rightPanel.show();
  leftPanel.show();
  delay(250);
  leftPanel.fillScreen(leftPanel.Color(255,255,255));
  rightPanel.fillScreen(rightPanel.Color(255,255,255));
  rightPanel.setTextColor(rightPanel.Color(0,0,200));
  leftPanel.setTextColor(leftPanel.Color(0,0,200));
  rightPanel.setCursor(9,0);
  leftPanel.setCursor(9,0);
  rightPanel.print("GO BUCS!");
  leftPanel.print("GO BUCS!");
  rightPanel.show();
  leftPanel.show();
  delay(250);
}

void goLakersScroll(){
  for(int i=64; i>-64; i--){
    leftPanel.fillScreen(0);
    rightPanel.fillScreen(0);
    rightPanel.setTextColor(rightPanel.Color(0,175,0));
    leftPanel.setTextColor(leftPanel.Color(0,175,0));
    rightPanel.setCursor(i,0);
    leftPanel.setCursor(i,0);
    rightPanel.print("GO Lakers!");
    leftPanel.print("GO Lakers!");
    leftPanel.show();
    rightPanel.show();
  }
}

void teamMiniScroll(){
  for(int i=64; i>-64; i--){
    leftPanel.fillScreen(0);
    rightPanel.fillScreen(0);
    rightPanel.setTextColor(rightPanel.Color(175,175,0));
    leftPanel.setTextColor(leftPanel.Color(175,175,0));
    rightPanel.setCursor(i,0);
    leftPanel.setCursor(i,0);
    rightPanel.print("Team Mini");
    leftPanel.print("Team Mini");
    leftPanel.show();
    rightPanel.show();
  }
}

void underConstructionScroll(){
  rightPanel.setTextColor(rightPanel.Color(255,255,0));
  leftPanel.setTextColor(leftPanel.Color(255,255,0));
  for(int i=64; i>-120; i--){
    leftPanel.fillScreen(0);
    rightPanel.fillScreen(0);
    rightPanel.setCursor(i,0);
    leftPanel.setCursor(i,0);
    rightPanel.print("Under Construction");
    leftPanel.print("Under Construction");
    leftPanel.show();
    rightPanel.show();
  }
}
