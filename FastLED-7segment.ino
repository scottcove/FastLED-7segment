////////////////////////////////////////////////////////////////////////////////////
//Begin 01.Variables

#include <FastLED.h>

//Verbose debugging via Terminal.
bool debugOn = true;


//Sets up some Variables.
const byte NUM_DIGITS = 2;
const byte LEDS_PER_SEG = 2;
const byte LEDS_PER_DIGIT = (LEDS_PER_SEG * 7);
const byte PIN0 = 22;
const byte PIN1 = 24;


//Initial HSV Values.  Used to set the values later
byte hueInt = 0;
byte satInt = 255;
byte valInt = 255;

//Sets the segments to light for each digit.  See notes in segLight();
byte digitPixel[10][7] = { {1,1,1,0,1,1,1},
                         {0,0,1,0,0,0,1},
                         {0,1,1,1,1,1,0},
                         {0,1,1,1,0,1,1},
                         {1,0,1,1,0,0,1},
                         {1,1,0,1,0,1,1},
                         {1,1,0,1,1,1,1},
                         {0,1,1,0,0,0,1},
                         {1,1,1,1,1,1,1},
                         {1,1,1,1,0,1,1},
};

//Defines the names of the strip arrays.
CRGB testStrip[NUM_DIGITS][LEDS_PER_DIGIT];


////////////////////////////////////////////////////////////////////////////////////
//Begin 02.Main

void setup() { 
  Serial.begin(115200);
  if (debugOn) Serial.println("Verbose debugging Started...");
  delay(500); //pause a moment to let capacitors on board settle.

  //Set up the strips to an array of arrays.
  if(debugOn) Serial.println("DEBUG: Initialising Strips");
  FastLED.addLeds<WS2812B, PIN0, GRB>(testStrip[0], LEDS_PER_DIGIT); 
  FastLED.addLeds<WS2812B, PIN1, GRB>(testStrip[1], LEDS_PER_DIGIT);
  //If you have more digits - these will need to be set here, one by one!
  
  delay(15); //Settle the LEDs.  There was some funny business going on before this was set.
  Serial.println("Strips initialised.");
  Serial.println("");

}

void loop() {
  rainbowFromWhite(15,15,500);
  rainbowToWhite(15,15,500);
  slowLoop(1000);
  FastLED.delay(1000);
  initialZero(1000);
  FastLED.delay(1000);
  countAll();
}


////////////////////////////////////////////////////////////////////////////////////
//Begin 03.segLight
void segLight(byte digit, byte seg, byte col){ 
  //Use this to light up a single segment.  NOTE: use FastLED.show() as well if using independently of digitWrite.
  //digit picks which neopixel strip(s), 0 indexed.
  //seg calls a segment (1-9).
  //col is color.  0 = off, 25 = white.  All else (1-24) is a fraction (1/15) of the colour wheel.
  byte endPixel = (seg * LEDS_PER_SEG);
  byte beginPixel = (endPixel - LEDS_PER_SEG);

  convertColourVal(col);


  //This Code will work with both single Pixel and multiple pixel segments, but will require a loop for each board, and any dots.
  for(int i=ld_beginPixel; i<endPixel; i++){
    testStrip[digit][i] = CHSV( hueInt, satInt, valInt);
  }
}

void convertColourVal(byte col){
  //Used to set the HSV colour correctly
  if(col==0){
    satInt = 255;
    valInt = 0;
  }
  else if(col==25){
    satInt = 0;
    valInt = 255;
  }
  else {
    //Converts the Colour Number (1-24 into a standard 360 degree Hue value, then into the one byte integer needed for the FastLED Library.
    //This Stack Exchange answer covers the math involved here:  https://stackoverflow.com/a/46821498
    satInt = 255;
    valInt = 255;
    byte hueValue = (col*15);
    hueInt = round((hueValue/1.41176470588235294117));
  }
}

////////////////////////////////////////////////////////////////////////////////////
//Begin 04.digitWrite

void digitWrite(byte digit, byte val, byte col){
  //use this to light up a digit
  //digit is which digit panel one (right to left, 0 indexed)
  //val is the character value to set on the digit
  //col is the predefined color to use (0-25), see notes on segLight.
  //example:
  //        digitWrite(0, 4, 1);
  //would set the first digit to a "4" in red.

  /*
  // Letters are the standard segment naming, as seen from the front,
  // numbers are based upon the wiring sequence.
  // If you modify the wiring, be sure to modify the order in the digitPixel array above.

       A 2     
      ----------
     |          |
     |          |
 F 1 |          | B 3
     |          |
     |     G 4  |
      ----------
     |          |
     |          |
 E 5 |          | C 7
     |          |
     |     D 6  |
      ---------- 
  */


  //Pixel Definitions Array.
  //This sets which segments will light (0 off, 1 on) for each value.  
  //Remember - Arrays are zero indexed, so the value does not need to change (0-9), but the segment to light will.
  if(debugOn) {Serial.print("DEBUG: From digitWrite... Writing Number: "); Serial.print(val); Serial.print(" in colour: "); Serial.println(col);}
  for(byte seg=0;seg<7;seg++){
    if(!digitPixel[val][seg]) {
      segLight(digit,(seg+1),0);
      delay(5);
    }
    else {
      segLight(digit,(seg+1),col);
      delay(5);
    }
  }
  FastLED.show();
}

////////////////////////////////////////////////////////////////////////////////////
//Begin 06.Test_Digits

//Chase Segments in a slow loop.  Now designed to show on all digits, as per segLight/ digitWrite:
void slowLoop(byte wait){
  if(debugOn) Serial.println("DEBUG: Called slowLoop.");
  for(byte seg=1;seg<8;seg++){
    if(debugOn) {Serial.print("DEBUG: from slowLoop - Lighting Segment: "); Serial.println(seg);}
    FastLED.clear();  //Clear Everything.
    if(seg != 1) segLight(0,(seg-1),0); //clear the last segment - without ever passing a zero segment.
    segLight(0,seg,(seg*2)); //display it
    FastLED.show();
    delay(wait);
  }
}

void countAll(){
  byte colourVal;
  byte digitVal;
  //this uses digitWrite() command to run through all numbers with one second 
  //timing on each of the digit panels in the display simultaneously.
    for(int i=1;i<26;i++){
      Serial.print("DEBUG: colour value:"); Serial.println(i);
      if(digitVal>9) digitVal = 0;
      if(i>26) colourVal = 1;
      else colourVal = i;
      digitWrite(0,digitVal,colourVal);
      delay(1000);
      digitVal++;
    }
}

void rainbowFromWhite(byte waitWhite, byte waitColour, byte finalWait){
  if (debugOn) Serial.println("DEBUG: Called rainbowFromWhite.");
  for(byte s;s<255;s++){
    if(debugOn) {Serial.print("DEBUG: s="); Serial.println(s);}
    fill_solid(testStrip,LEDS_PER_DIGIT,CHSV(0,s,128));
    fill_solid(ultraHighDensity,UHD_LEDS_PER_DIGIT,CHSV(0,s,128));
    FastLED.show();
    FastLED.delay(waitWhite);
  }
  for(byte c;c<255;c++){
    if(debugOn) {Serial.print("DEBUG: c="); Serial.println(c);}
    fill_solid( testStrip, LEDS_PER_DIGIT, CHSV(c,255,128) );
    fill_solid( ultraHighDensity, UHD_LEDS_PER_DIGIT, CHSV(c,255,128) );
    FastLED.show();
    FastLED.delay(waitColour);
  }
  FastLED.clear();
  FastLED.delay(finalWait);
}

void rainbowToWhite(byte waitColour, byte waitWhite, byte finalWait){
  if (debugOn) Serial.println("DEBUG: Called rainbowToWhite.");
  for(byte c=255;c>0;c--){
    if(debugOn) {Serial.print("DEBUG: c="); Serial.println(c);}
    fill_solid( lowDensity, LEDS_PER_DIGIT, CHSV(c,255,255) );
    fill_solid( ultraHighDensity, UHD_LEDS_PER_DIGIT, CHSV(c,255,255) );
    FastLED.show();
    FastLED.delay(waitColour);
  }
  for(byte s=255;s>0;s--){
    if(debugOn) {Serial.print("DEBUG: s="); Serial.println(s);}
    fill_solid(lowDensity,LEDS_PER_DIGIT,CHSV(255,s,255));
    fill_solid(ultraHighDensity,UHD_LEDS_PER_DIGIT,CHSV(255,s,255));
    FastLED.show();
    FastLED.delay(waitWhite);
  }
  FastLED.clear();
  FastLED.delay(finalWait);
}

//Lands on a Zero for all digits
void initialZero(byte wait){
  if(debugOn) Serial.println("DEBUG: Called initialZero.");
  byte displayOrder[6] = {6,7,3,2,1,5};
  FastLED.clear();
  for(byte index=0;index<6;index++){
    segLight(0,displayOrder[index],25);
    FastLED.show();
    delay(wait);
  }
}
