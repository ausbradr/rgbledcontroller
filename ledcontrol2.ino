//FastLED Light control box:
// The purpose of this code is to power/control WS2812 based LED accessories, by way of rotary encoder

//EEPROM inits:
#include <EEPROM.h>

//SSD1306 Screen inits
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Let's select the pin we'll use for the loudspeaker:
int tonePin = 8;
//let's have some music: Note: Probably don't need this
//#include "pitches.h"  // must include open source pitches.h found online in libraries folder or make a new tab => https://www.arduino.cc/en/Tutorial/toneMelody



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Let's init the application vars:
byte screencode = 0;
byte redval = 255;
byte greenval = 255;
byte blueval = 255;
byte brightval = 255;
byte loadpresetnum = 0;
byte savepresetnum = 0;
byte modenum = 0;
byte numberval = 0;

byte testdata = 0; //This can be removed when done with. It's a test for dumping the EEPROM

/*
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
*/






//Rotary Encoder inits
#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_BTN 4
//setup ints for buttonstates
int encodertotal = 0;



//FastLED inits
#include "FastLED.h"                                          // FastLED library.
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
// Fixed definitions cannot change on the fly.
#define LED_DT 6                                             // Data pin to connect to the strip.
//#define LED_CK 15
#define COLOR_ORDER GRB                                       // It's GRB for WS2812B and BGR for APA102
#define LED_TYPE WS2812B                                       // What kind of strip are you using (WS2801, WS2812B or APA102)?
#define NUM_LEDS 50                                           // Number of LED's. 50 is good for this project. Given current / RAM
// Initialize changeable global variables.
uint8_t max_bright = 255;                                     // Overall brightness definition. It can be changed on the fly.
struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.
// Initialize global variables for sequences
uint8_t thisdelay = 40;                                       // A delay value for the sequence(s)
uint8_t thishue = 0;                                          // Starting hue value.
int8_t thisrot = 1;                                           // Hue rotation speed. Includes direction.
uint8_t deltahue = 1;                                         // Hue change between pixels.
bool thisdir = 0;                                             // I use a direction variable instead of signed math so I can use it in multiple routines.











//let's put some functions here because the compiler is being shitty:
void startupsound(){
  tone(tonePin,100,75);
  delay(75);
  tone(tonePin,250,75);
  delay(75);
  tone(tonePin,500,75);
  delay(75);
  tone(tonePin,1000,75);
  delay(75);
  tone(tonePin,4000,75);
  delay(75);
  tone(tonePin,8000,75);
  delay(75);
  tone(tonePin,10000,75);
  }

















void setup() {

  Serial.begin(115200);                                        // Initialize serial port for debugging.
  delay(1000);                                                // Soft startup to ease the flow of electrons.

//SSD1306 Screen Setups:
 // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
     Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  



  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  startupsound();
  delay(1000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.display();
// text display tests
  
//Rotary Encoder setups
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_BTN, INPUT_PULLUP);

//FastLED setups
    LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);      // Use this for WS2812B
//  LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2801 or APA102
  FastLED.setBrightness(max_bright);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);                // FastLED 2.1 Power management set at 5V, 500mA
  
} // setup()

int lastClk = HIGH; //We leave this here for encoder to work








void loop() {

loopstart:
updatelights();
FastLED.show();
loadthepreset(0);

menured: 
  drawscreen(5);

do{
  checkrotary();
}while(encodertotal == 0);
//up
if(encodertotal == 10){
 selectbeep();
 goto menugreen;
}

//down
if(encodertotal == 50){
  selectbeep();
  goto menumode;
}

//click
if(encodertotal ==100){
  clickbeep();
  goto setred;
}








menugreen:
  drawscreen(6);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  selectbeep();
  goto menublue;
}
//down
if(encodertotal == 50){
  selectbeep();
  goto menured;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto setgreen;
}



menublue:
  drawscreen(7);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  selectbeep();
  goto menubright;
}
//down
if(encodertotal == 50){
  selectbeep();
  goto menugreen;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto setblue;
}

menubright:
  drawscreen(8);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  selectbeep();
  goto menuload;
}
//down
if(encodertotal == 50){
  selectbeep();
  goto menublue;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto setbright;
}




menuload:
  drawscreen(9);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  selectbeep();
  goto menusave;
}
//down
if(encodertotal == 50){
  selectbeep();
  goto menubright;
}

//click
if(encodertotal == 100){
  goto setload;
}



menusave:
  drawscreen(10);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  selectbeep();
  goto menumode;
}
//down
if(encodertotal == 50){
  selectbeep();
  goto menuload;
}

//click
if(encodertotal == 100){
  goto setsave;
}


menumode:
selectbeep();
  drawscreen(11);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  goto menured;
}
//down
if(encodertotal == 50){
  goto menusave;
}

//click
if(encodertotal == 100){
  clickbeep();
  drawstandby();
  blanklight();
    do{
    checkrotary();
    }while(encodertotal == 0);

    //click
    if(encodertotal == 100){
    clickbeep();
    updatelights();
    goto menumode;

}
}

setred:
  drawscreen(1);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
    
    if(redval == 255){
    redval = 0;
    //The reason we skip in the next line is because if the first test is successful, we don't wish to conduct the second test. It causes 255/0 to be skipped when scrolling if we don't do this.
    goto setredupskip;
    }
    
    if(redval<255){
    redval = redval + 1;
    }
    setredupskip:
    freqbeep(redval);
    updatelights();
  goto setred;
}
//down
    
    if(encodertotal == 50){
    if(redval > 0){
    redval = redval - 1;
    goto setreddownskip;
    }
    
    if(redval == 0){
    redval = 255;
    }
    
    setreddownskip:
    updatelights();
    freqbeep(redval);
  goto setred;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto menured;
}


setgreen:
  drawscreen(2);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
    
    if(greenval == 255){
    greenval = 0;
    goto setgreenupskip;
    }
    
    if(greenval<255){
    greenval = greenval + 1;
    }
    
    setgreenupskip:
    freqbeep(greenval);
    updatelights();

  goto setgreen;
}
//down
if(encodertotal == 50){
    
    if(greenval > 0){
    greenval = greenval - 1;
    goto setgreendownskip;
    }


    if(greenval == 0){
    greenval = 255;
    }

    setgreendownskip:
    updatelights();
    freqbeep(greenval);
  
  goto setgreen;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto menugreen;
}



setblue:
  drawscreen(3);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  
  if(blueval == 255){
  blueval = 0;
  goto setblueupskip;
  }
  
  if(blueval<255){
  blueval = blueval + 1;
  }
  
  setblueupskip:
  freqbeep(blueval);
  updatelights();
  
  goto setblue;
}
//down
if(encodertotal == 50){
  
  if(blueval > 0){
  blueval = blueval - 1;
  goto setbluedownskip;
  }

  if(blueval == 0){
  blueval = 255;
  }
  
  setbluedownskip:
  updatelights();
  freqbeep(blueval);
  
  goto setblue;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto menublue;
}




setbright:
  drawscreen(4);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
  
  if(brightval == 255){
  brightval = 0;
  goto setbrightupskip;
  }

  if(brightval<255){
  brightval = brightval + 1;
  }

setbrightupskip:
  freqbeep(brightval);
  updatelights();

  goto setbright;
}
//down
if(encodertotal == 50){
   
  if(brightval > 0){
  brightval = brightval - 1;
  goto setbrightdownskip;
  }
  
  if(brightval == 0){
  brightval = 255;
  }
  
  setbrightdownskip:
  updatelights();
  freqbeep(brightval);

  goto setbright;
}

//click
if(encodertotal == 100){
  clickbeep();
  goto menubright;
}


setload:
confirmbeep();
  drawscreen(12);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
    if(loadpresetnum<10){
      loadpresetnum = loadpresetnum + 1;
    }
  goto setload;
}
//down
if(encodertotal == 50){
    if(loadpresetnum > 0){
      loadpresetnum = loadpresetnum - 1;
    }
  goto setload;
}

//click
if(encodertotal == 100){
  loadthepreset(loadpresetnum);
  goto menuload;
}


setsave:
confirmbeep();
  drawscreen(13);

do{
  checkrotary();
}while(encodertotal == 0);

//up
if(encodertotal == 10){
    if(savepresetnum<10){
      savepresetnum = savepresetnum + 1;
    }
  goto setsave;
}
//down
if(encodertotal == 50){
    if(savepresetnum > 0){
      savepresetnum = savepresetnum - 1;
    }
  goto setsave;
}

//click
if(encodertotal == 100){
  savethepreset(savepresetnum);
  goto menusave;
}




//this brace links to voidloop!
}




void checkrotary() {
//zero the result values
encodertotal = 0;

    int newClk = digitalRead(ENCODER_CLK);
  if (newClk != lastClk) {
    // There was a change on the CLK pin
    lastClk = newClk;
    int dtValue = digitalRead(ENCODER_DT);
    if (newClk == LOW && dtValue == HIGH) {
      Serial.println("UP");
      encodertotal = 10;
     delay(5);
      goto rotarycheckdone;
    }
    if (newClk == LOW && dtValue == LOW) {
      Serial.println("DOWN");
      encodertotal = 50;
    delay(5);
    }
    rotarycheckdone:
    delay(1);   
  }

  if (digitalRead(ENCODER_BTN) == LOW) {
    Serial.println("Button pressed");
    encodertotal = 100;
    delay(200);
  

}
}

void drawstandby(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0,0);
  display.println(F("STANDBY"));
  display.println(F("Click to Resume"));
  display.display();

}

void blanklight(){
  fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();
}


void drawscreen (int screencode) {
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0,0);
  display.print(F("  ")); 
  formatval(redval);
  display.print(F("  "));
  formatval(greenval);
  display.print(F("  "));
  formatval(blueval);
  display.print(F("  "));
  formatval(brightval);
  display.println("");
  display.println(F("  RED  GRN  BLU  BRT"));
  display.println("");
  display.print(F("  Load Preset   "));
  formatval(loadpresetnum);
  display.println("");
  display.print(F("  Save Preset   "));
  formatval(savepresetnum);
  display.println("");
  display.println(F("  Turn Off      "));
  //formatval(modenum);
  /*
  if(modenum == 0){
  display.println(F("---"));
  }
  if(modenum > 0){
  display.println(F("YES"));
  }
  */
  display.println(F(""));
  display.println(F(""));
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(F("Select an option     "));
  display.display();  


//Now lets work out where we want a cursor....
  display.setTextColor(WHITE, BLACK);
if(screencode == 0){
//probably nothing here, we don't need a cursor here

}
if(screencode == 1){
  display.setCursor(5,0);
  display.print(F(">"));
}

if(screencode == 2){
  display.setCursor(35,0);
  display.print(F(">"));
}

if(screencode == 3){
  display.setCursor(65,0);
  display.print(F(">"));
}

if(screencode == 4){
  display.setCursor(95,0);
  display.print(">");
}

if(screencode == 5){
  display.setCursor(5,8);
  display.print(">");
}

if(screencode == 6){
  display.setCursor(35,8);
  display.print(">");
}

if(screencode == 7){
  display.setCursor(65,8);
  display.print(">");
}

if(screencode == 8){
  display.setCursor(95,8);
  display.print(">");
}

if(screencode == 9){
  display.setCursor(5,24);
  display.print(">");
}

if(screencode == 10){
  display.setCursor(5,32);
  display.print(">");
}

if(screencode == 11){
  display.setCursor(5,40);
  display.print(">");
}

if(screencode == 12){
  display.setCursor(88,24);
  display.print(">");
}

if(screencode == 13){
  display.setCursor(88,32);
  display.print(">");
}

if(screencode == 14){
  display.setCursor(88,40);
  display.print(">");
}

//now that we've decided cursor position, let's show it:
  display.display();


}

void formatval(int numberval){
  if(numberval<100){
    display.print("0");

    if(numberval<10){
      display.print("0");
    }

  }

display.print(numberval);

}


void updatelights(){
  //in this subroutine, we'll update the lights whenever this is called. 
fill_solid( leds, NUM_LEDS, CRGB(redval,greenval,blueval));
FastLED.setBrightness(brightval);
FastLED.show();
}

void loadthepreset(byte memloc){
  loadtone();
  //Here we'll load the preset requested
  if(memloc == 0){
    redval = EEPROM.read(0);
    greenval = EEPROM.read(1);
    blueval = EEPROM.read(2);
/*
 //test routine, keep it commented unless diags needed
    Serial.print(F("Loaded preset: "));
    Serial.print(memloc);
    Serial.print(F(" with values: "));
    Serial.print(redval);
    Serial.print(F(" , "));
    Serial.print(blueval);
    Serial.print(F(" , "));
    Serial.println(greenval);
    Serial.print(F("At EEPROM locations: "));
    Serial.println(F("0, 1 and 2 because memloc is zero!"));
    //test dump
    Serial.println(F("Dumping EEPROM limited to 50 bytes"));
for (int i = 0; i <= 50; i++) {
   Serial.print(i);
   Serial.print(F(": "));
  testdata = EEPROM.read(i);
  Serial.println(testdata);
  }
*/
  }
  if(memloc > 0){
    redval = EEPROM.read(memloc*3);
    greenval = EEPROM.read((memloc*3)+1);
    blueval = EEPROM.read((memloc*3)+2);
/*
       //test routine, keep commented unless diags needed
    Serial.print(F("Loaded preset: "));
    Serial.print(memloc);
    Serial.print(F(" with values: "));
    Serial.print(redval);
    Serial.print(F(" , "));
    Serial.print(blueval);
    Serial.print(F(" , "));
    Serial.println(greenval);
    Serial.print(F("At EEPROM locations: "));
    Serial.print(memloc*3);
    Serial.println(F(" , "));
    Serial.print((memloc*3)+1);
    Serial.println(F(" , "));
    Serial.print((memloc*3)+2);
    
    
    
    //test dump
    Serial.println(F("Dumping EEPROM limited to 50 bytes"));
for (int i = 0; i <= 50; i++) {
   Serial.print(i);
   Serial.print(F(": "));
  testdata = EEPROM.read(i);
  Serial.println(testdata);
  }
  */

  }

//updatelights();
 transferlights();
}

void savethepreset(byte memloc){
  savetone();
  //here, we'll save a preset to EEPROM
if(memloc == 0){
    EEPROM.update(0,redval);
    EEPROM.update(1,greenval);
    EEPROM.update(2,blueval);
    
/*
    //test routine, commment untill diags needed
    Serial.print(F("Saving preset: "));
    Serial.print(memloc);
    Serial.print(F(" with values: "));
    Serial.print(redval);
    Serial.print(F(" , "));
    Serial.print(blueval);
    Serial.print(F(" , "));
    Serial.println(greenval);
    Serial.print(F("At EEPROM locations: "));
    Serial.println(F("0, 1 and 2 because memloc is zero!"));
    //test dump
    Serial.println(F("Dumping EEPROM limited to 50 bytes"));
for (int i = 0; i <= 50; i++) {
   Serial.print(i);
   Serial.print(F(": "));
  testdata = EEPROM.read(i);
  Serial.println(testdata);
  }
*/



  }
  if(memloc > 0){
    EEPROM.update(memloc*3, redval);
    EEPROM.update((memloc*3)+1, greenval);
    EEPROM.update((memloc*3)+2, blueval);
/*
        //test routine, keep commented unless diags needed
    Serial.print(F("Saving preset: "));
    Serial.print(memloc);
    Serial.print(F(" with values: "));
    Serial.print(redval);
    Serial.print(F(" , "));
    Serial.print(blueval);
    Serial.print(F(" , "));
    Serial.println(greenval);
    Serial.print(F("At EEPROM locations: "));
    Serial.print(memloc*3);
    Serial.println(F(" , "));
    Serial.print((memloc*3)+1);
    Serial.println(F(" , "));
    Serial.print((memloc*3)+2);
    
    
    
    //test dump
    Serial.println(F("Dumping EEPROM limited to 50 bytes"));
for (int i = 0; i <= 50; i++) {
   Serial.print(i);
   Serial.print(F(": "));
  testdata = EEPROM.read(i);
  Serial.println(testdata);
  }
*/


  }


}

//let's add some sound routines for a buzzer

void selectbeep(){
  // Serial.println("selectbeep");
  tone(tonePin, 800, 5);
}

void confirmbeep(){
  // Serial.println("confirmbeep");
    tone(tonePin, 2000, 5);

}

void freqbeep(int freq){
  freq = freq + 70;
  tone(tonePin, (freq*5),10);
}

void loadtone(){
tone(tonePin,600,25);
delay(25);
tone(tonePin,700,25);
delay(25);
tone(tonePin,800,25);
delay(25);
tone(tonePin,900,25);
delay(25);
tone(tonePin,1000,25);
delay(25);
}

void savetone(){
tone(tonePin,1000,25);
delay(25);
tone(tonePin,900,25);
delay(25);
tone(tonePin,800,25);
delay(25);
tone(tonePin,700,25);
delay(25);
tone(tonePin,600,25);
delay(25);
}

void clickbeep(){
  tone(tonePin, 800, 50);
  delay(25);
  tone(tonePin,1200,20);
  delay(50);
  tone(tonePin, 1600, 20);
}


void standbybeep(){
  tone(tonePin,10000,75);
  delay(75);
  tone(tonePin,8000,75);
  delay(75);
  tone(tonePin,4000,75);
  delay(75);
  tone(tonePin,1000,75);
  delay(75);
  tone(tonePin,500,75);
  delay(75);
  tone(tonePin,250,75);
  delay(75);
  tone(tonePin,100,75);
 
}

void transferlights(){
  FastLED.setBrightness(brightval);
  for (int i = 0; i < NUM_LEDS; i++) {
    delay(2);
    tone(tonePin,400,2);
    delay(2);
    tone(tonePin,800,2);
    delay(2);
    leds[i].setRGB( redval, greenval, blueval);
    FastLED.show();

  }


}
