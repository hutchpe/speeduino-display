#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TouchScreen.h"
#include "SymbolMono18pt7b.h"
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// These are the four touchscreen analog pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be any digital pin
#define XP 9   // can be any digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define MINPRESSURE 10
#define MAXPRESSURE 1000

//This is sensor threshold values
#define OIL_PRESSURE_MIN 15
#define OIL_PRESSURE_MAX 120
#define OIL_TEMP_MIN 70
#define OIL_TEMP_MAX 110
#define CLT_TEMP_MIN 70
#define CLT_TEMP_MAX 95

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

uint8_t rotation = 3;
byte liveData[75];
int arrayCounter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(rotation);
  tft.fillScreen(ILI9341_BLACK);
  displaySymbol();

  // read diagnostics (optional but can help debug problems)
  //uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  //Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  //x = tft.readcommand8(ILI9341_RDMADCTL);
  //Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  //x = tft.readcommand8(ILI9341_RDPIXFMT);
  //Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  //x = tft.readcommand8(ILI9341_RDIMGFMT);
  //Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  //x = tft.readcommand8(ILI9341_RDSELFDIAG);
  //Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
  
}


void loop() {
  // put your main code here, to run repeatedly:

  clearData();
  getData();
  displayData(liveData[41], liveData[43], liveData[8], liveData[7], liveData[10], liveData[5]);
  delay(1000);
}
void displaySymbol() {
  drawSymbol(0,60, OIL_PRESSURE_SYMBOL,ILI9341_RED,0,3);
  drawSymbol(0,120, OIL_TEMP_SYMBOL,ILI9341_ORANGE,0,3);
  drawSymbol(165,60, COOLANT_TEMP_SYMBOL,ILI9341_RED,0,3);
  drawSymbol(0,185, BATTERY_SYMBOL,ILI9341_RED,0,3);
  drawSymbol(160,180, BOOST_SYMBOL,ILI9341_BLUE,0,3);
  drawSymbol(130,105, DEG_CELCIUS_SYMBOL,ILI9341_WHITE,0,1);//Oil Temp C
  drawSymbol(290,45, DEG_CELCIUS_SYMBOL,ILI9341_WHITE,0,1);//Coolant Temp C
  drawSymbol(290,105, DEG_CELCIUS_SYMBOL,ILI9341_WHITE,0,1);//IAT C

  //Oil Pressure PSI unit
  tft.setCursor(130, 45);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("PSI");

  //IAT Symbol
  tft.setCursor(170,105);
  tft.setFont(&FreeSans18pt7b);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("IAT");

  //Battery voltage unit
  tft.setCursor(140,165);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("V");
}
void displayData(byte OIL_PRS, byte OIL_TMP, byte CLT_TMP, byte IAT, byte BAT_VOLT, byte MAP) {
  //Check if values exceed thresholds
  checkThreshold(OIL_PRS, OIL_TMP, CLT_TMP);
  
  //Display Oil Pressure Data
  tft.setCursor(70, 45);
  tft.setFont(&FreeSans18pt7b);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(OIL_PRS);
  
  //Display Oil Temp Data
  tft.setCursor(70, 105);
  tft.setFont(&FreeSans18pt7b);
  tft.print(OIL_TMP);

  //Display Battery Data
  tft.setCursor(70, 165);
  tft.setFont(&FreeSans18pt7b);
  tft.print((float)BAT_VOLT/10,1);
  
  //Display Coolant Temp Data
  tft.setCursor(230,45);
  tft.setFont(&FreeSans18pt7b);
  tft.print(CLT_TMP);
  
  //Display IAT Data
  tft.setCursor(230,105);
  tft.setFont(&FreeSans18pt7b);
  tft.print(IAT);

  //Display Boost Data
  tft.setCursor(225, 165);
  tft.setFont(&FreeSans18pt7b);
  //If vacuum, display in unit inHg
  if (MAP<100) {
    tft.print(MAP*0.2961-29.61,0);
    tft.setCursor(280,165);
    tft.setFont(&FreeSans9pt7b);
    tft.print("inHg");
  }
  //If positive pressure, display in unit PSI
  else if (MAP>=100){
    tft.print(MAP*0.145-14.5,1);
    tft.setCursor(290,165);
    tft.setFont(&FreeSans9pt7b);
    tft.print("PSI");
  }
}

//Selectively clear data from screen for refresh
void clearData() {
  tft.fillRect(70, 15, 60, 40, ILI9341_BLACK); //Clear oil pressure
  tft.fillRect(70, 75, 60, 40, ILI9341_BLACK); //Clear oil temp
  tft.fillRect(70, 135, 68, 40, ILI9341_BLACK); //Clear battery volt
  tft.fillRect(230, 15, 60, 40, ILI9341_BLACK); //Clear coolant temp
  tft.fillRect(230, 75, 60, 40, ILI9341_BLACK); //Clear IAT
  tft.fillRect(225, 135, 100, 40, ILI9341_BLACK); //Clear boost
}

//Display warning if any sensor values have exceeded thresholds
void checkThreshold(byte OIL_PRS, byte OIL_TMP, byte CLT_TMP) {
  if ((int)OIL_PRS < OIL_PRESSURE_MIN || (int)OIL_PRS > OIL_PRESSURE_MAX){
     tft.fillRect(70, 15, 60, 40, ILI9341_RED);
  }
  if ((int)OIL_TMP < OIL_TEMP_MIN || (int)OIL_TMP > OIL_TEMP_MAX){
     tft.fillRect(70, 75, 60, 40, ILI9341_RED);
  }
  if ((int)CLT_TMP < CLT_TEMP_MIN || (int)CLT_TMP > CLT_TEMP_MAX){
     tft.fillRect(230, 15, 60, 40, ILI9341_RED);
  } 
}

//Set font to custom font when drawing custom symbols
void drawSymbol(uint16_t x, uint16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t Size){
    if( (c>=32) && (c<=126) ){ //If it's 33-126 then use standard mono 18 font
        tft.setFont(&FreeSans18pt7b);
    } else {
      tft.setFont(&SymbolMono18pt7b);//Otherwise use special symbol font
      if (c>126) {      //Remap anything above 126 to be in the range 32 and upwards
        c-=(127-32);
      }
    }
    tft.drawChar(x,y,c,color,bg,Size);
  }

//Request data from Speeduino and save it
 void getData(){
  Serial.write(0x6e); //Send 'n' to Speeduino to request live data
  while(!Serial.available()); //Wait for the serial port to send data

  while (arrayCounter < 75) {
    if (Serial.available() > 0) {
      liveData[arrayCounter] = Serial.read();
      if (arrayCounter == 0 && liveData[arrayCounter] != 0x6e) {
        return;
      }
      arrayCounter++;
    }
  }
  
  arrayCounter=0;
 }
