#define NOTDEBUGGING false
#include <UTFT.h>
#include <UTFT_Geometry.h>
#include <URTouch.h>
#include <SdFat.h>
#include <UTFT_SdRaw.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */


const uint8_t SD_CHIP_SELECT = 53;
 
const int8_t DISABLE_CHIP_SELECT = -1;
SdFat sd;
UTFT myGLCD(SSD1963_480,38,39,40,41); //myGLCD(RS,WR,CS,RST,ALE,mode);
URTouch  myTouch(6,5,4,3,2); //myTouch(TCLK,TCS,DIN,DOUT,IRQ);
UTFT_SdRaw myFiles(&myGLCD);
UTFT_Geometry geo(&myGLCD);
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegmentFull[];
unsigned long previousMillis = 0;  
unsigned long previousMillis2 = 0;  
float multiplier = 0.1875F;
float adc0v;
float adc0vP=0;
long timepast=0;
int c=0;
long theinterval=100;
int debounce=0;
int mystatus=0;
int logv[200]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void setup()
{
  pinMode(53,OUTPUT);
  delay(250);
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_EXTREME);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  ads.begin(); 
  Serial.begin(115200);
    Serial.println("Initialising board");
  bool mysd = 0;
  
  myGLCD.print("Digital Arduino Voltmeter",0,0);
  myGLCD.print("by HX2003",0,21);
  // see if the card is present and can be initialized:
  myGLCD.print("Waiting for SD card",7, 60, 0);
  //while (!mysd)
  //{
    if (!sd.begin(SD_CHIP_SELECT,4)) {
      Serial.println(F("Card failed, or not present"));
      Serial.println(F("Retrying...."));
    }
    else
    {
      mysd = 1;
      Serial.println(F("Card initialised."));
      myGLCD.setColor(VGA_BLACK);
      myGLCD.fillRect(0,60,479,271);
      myGLCD.setColor(VGA_WHITE);
    }

  myGLCD.setFont(SmallFont);
  myGLCD.print("5V",0,100);
  myGLCD.print("0V",0,174);
  myGLCD.print("-5V",0,248);
  long calced = theinterval*200/1000;
  myGLCD.print(String(calced)+"S",240,175);
  createButton(250,100,100,35,VGA_WHITE,VGA_GRAY,VGA_WHITE, "SCALE");
  createButton(365,100,100,35,VGA_WHITE,VGA_RED,VGA_WHITE, "STOPPED");
  createButton(365,50,100,35,VGA_WHITE,VGA_RED,VGA_WHITE, "X");
  myGLCD.setFont(BigFont);
  drawBaseGraph();
  }
//}
//END OF SETUP
void createButton(int x, int y, int w, int h, int colorB, int colorInt, int colorText, String text){
 myGLCD.setFont(SmallFont);
 myGLCD.setColor(colorInt);
 myGLCD.fillRect(x+1,y+1,x+w-1,y+h-1);
 myGLCD.setBackColor(colorInt);
 myGLCD.setColor(colorText);
 //int xc = x+(w/2);
 //int yc = y+(h/2);
 //int fx = myGLCD.getFontXsize();
 //int fy = myGLCD.getFontYsize();
 //int tl = text.length();
 //int th = yc-(fy/2);
 //int tx = xc-((fx*tl)/2);
 myGLCD.print(text, (x+(w/2))-(myGLCD.getFontXsize()*text.length()/2),(y+(h/2)-(myGLCD.getFontYsize()/2)));
 myGLCD.setColor(colorB);
 myGLCD.drawRect(x,y,x+w,y+h);
 myGLCD.setBackColor(VGA_BLACK);
}
//LOOP
void loop(){
  //LOOP DO
    if((millis()- previousMillis >= theinterval)&&mystatus==1) {
  previousMillis = millis();
 float adc0, adc1, adc2, adc3;
  
  adc0 = ads.readADC_Differential_0_1();  
  adc0v = adc0*multiplier;
  myGLCD.setFont(SevenSegmentFull);
  //if(adc0vP!=adc0v){
  //myGLCD.setColor(VGA_BLACK);
 // myGLCD.print(String(adc0vP),0,100);
  //}
  myGLCD.setColor(VGA_WHITE);
  myGLCD.print(String(adc0v)+"   ",0,50);
  //adc0vP=adc0v;
  Serial.println(adc0v);
  myGLCD.setColor(VGA_BLUE);
   
  if(c<200){
    logv[c] = adc0v;
  for(int i=0;i<c;i++){
  //myGLCD.drawPixel(31+i,map(logv[i],-5300,5300,260,100));  
  int d;
  if(i!=0){d = i-1;}
  else {d=0;}
  myGLCD.drawLine(31-1+i,map(logv[d],-5300,5300,260,100),31+i,map(logv[i],-5300,5300,260,100));
  }
    c=c+1;
    Serial.println("S");
  } 
  //AFTER THIS
  if(false){
  if(c>199){
   myGLCD.setColor(VGA_BLACK);
   for(int i=0;i<199;i++){
      myGLCD.drawPixel(31+i,map(logv[i],-5300,5300,260,100));
      logv[i]=logv[i+1];
   }
    myGLCD.drawPixel(31+199,map(logv[199],-5300,5300,260,100));
    logv[199]=adc0v;
    
     myGLCD.setColor(VGA_BLUE);
  for(int i=0;i<200;i++){
  myGLCD.drawPixel(31+i,map(logv[i],-5300,5300,260,100));
  }
  }
  }
 //NEW
   if(c>199){
   myGLCD.setColor(VGA_BLACK);
   for(int i=0;i<199;i++){
  int d;
  if(i!=0){d = i-1;}
  else {d=0;}
      myGLCD.drawLine(31-1+i,map(logv[d],-5300,5300,260,100),31+i,map(logv[i],-5300,5300,260,100));
   }
  myGLCD.drawLine(31-1+199,map(logv[198],-5300,5300,260,100),31+199,map(logv[199],-5300,5300,260,100));
for(int i=0;i<199;i++){
   logv[i]=logv[i+1];
}
    logv[199]=adc0v;
    
     myGLCD.setColor(VGA_BLUE);
  for(int i=0;i<200;i++){
  int d;
  if(i!=0){d = i-1;}
  else {d=0;}
      myGLCD.drawLine(31-1+i,map(logv[d],-5300,5300,260,100),31+i,map(logv[i],-5300,5300,260,100)); 
  }
  }
    drawBaseGraph();
    }
    if((myTouch.dataAvailable() == false)&&(debounce==1)){
    debounce=0;
    timepast = millis();
    //Serial.println("NOT AVAIL");
    }
   if (myTouch.dataAvailable() == true){
  if((millis()-timepast>=100)&&(debounce==0)){
    myTouch.read();
      int touchx=myTouch.getX();
      int touchy=myTouch.getY();
           if((touchy>=100) && (touchy<=100+35)){  
      if((touchx>=250 && (touchx<=250+100))){ 
        if(theinterval==100){theinterval=300;}
        else if(theinterval==300){theinterval=1500;}
        else if(theinterval==1500){ theinterval=9000;}
        else if(theinterval==9000){ theinterval=100;}
        long calced = theinterval*200/1000;
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(SmallFont);
  myGLCD.print(String(calced)+"S   ",240,175);
      }
 if((touchx>=365 && (touchx<=365+100))){ 
      if(mystatus==0){mystatus=1; createButton(365,100,100,35,VGA_WHITE,VGA_LIME,VGA_WHITE, "RUNNING");}
        else if(mystatus==1){ mystatus=0; createButton(365,100,100,35,VGA_WHITE,VGA_RED,VGA_WHITE, "STOPPED");}
 }
      }
  if((touchy>=50) && (touchy<=50+35)){  
      if((touchx>=365 && (touchx<=365+100))){
    myGLCD.setColor(VGA_BLACK);
   for(int i=0;i<200;i++){
   int d;
   if(i!=0){d = i-1;}
   else {d=0;}
      myGLCD.drawLine(31-1+i,map(logv[d],-5300,5300,260,100),31+i,map(logv[i],-5300,5300,260,100));
   }
   for(int i=0;i<199;i++){
    logv[i]=0;
   }
   c=0;
   drawBaseGraph();   
      }
  }
  debounce=1;
   }
}
}
void drawBaseGraph(){
  myGLCD.setColor(VGA_WHITE);
  myGLCD.drawLine(25,180,235,180);
  myGLCD.drawLine(30,100,30,271);
  //Y AXIS detail label
  for(int i=0;i<20;i++){
  myGLCD.drawLine(25,100+i*8,35,100+i*8);    
  }
  //X AXIS detail label
   for(int i=0;i<21;i++){
  myGLCD.drawLine(30+i*10,175,30+i*10,185);    
  }
}
