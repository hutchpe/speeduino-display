/*
 * Program to test custom fonts for Adafruit GFX library.
 * See https://learn.adafruit.com/creating-custom-symbol-font-for-adafruit-gfx-library/
 * for details.
 */
#include <Wire.h>

//Uncomment only one of the following lines to select your display.
#define USE_ILI9341   //2.4 inch 320x240 TFT Feather Wing
//#define USE_HX8357    //3.5 inch 480x320 TFT Feather Wing
//#define USE_HALLOWING //Adafruit Hallowing M0 Express
//#define USE_PYGAMER   //Adafruit PyGamer M4 Express
#include "board_select.h"

/*
 * We will attempt to create a symbol font to complement the 18 point FreeMono font.
 * Any character in the range 32-127 will use the FreeMono font. Anything outside 
 * that range will be one of our custom symbols.
 */
#include <Fonts/FreeMono18pt7b.h>

/*
 * This is our custom symbol font. It can be stored as a separate tab of this sketch or
 * in Arduino/libraries/My_Fonts folder which you would create. We do not recommend putting it in
 * Arduino/libraries/Fonts in case you ever need to delete and reinstall the GFX Library.
 */
#include "SymbolMono18pt7b.h"

uint8_t  Magnifier;
uint16_t First_Glyph;

/*
 * Use this function instead of display.drawChar to draw the symbol or to use
 * the default font if it's not in the symbol range.
 */
void drawSymbol(uint16_t x, uint16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t Size){
  if( (c>=32) && (c<=126) ){ //If it's 33-126 then use standard mono 18 font
      display.setFont(&FreeMono18pt7b);
  } else {
    display.setFont(&SymbolMono18pt7b);//Otherwise use special symbol font
    if (c>126) {      //Remap anything above 126 to be in the range 32 and upwards
      c-=(127-32);
    }
  }
  display.drawChar(x,y,c,color,bg,Size);
}

/*
 * Display characters on the screen starting with First_Glyph until you hit
 * last character of the font or run out of space on the screen.
 */
void Show(void)  {
  /*
   * Although our characters are at most 16 pixels wide, we're going to space them
   * an arbitrary horizontal amount DELTA_C for readability. Similarly the vertical 
   * spacing is set at DELTA_R for readability. The BASE_R is a vertical adjustment 
   * that is the distance from the top of the cell to the baseline of the character.
   */
  #define DELTA_C 21
  #define DELTA_R 30
  #define BASE_R 22
  uint8_t Max_C = DWIDTH / (DELTA_C * Magnifier);
  uint8_t Max_R = DHEIGHT / (DELTA_R * Magnifier);
  uint8_t NumChar = Max_C * Max_R;
  uint16_t Last_Glyph = min(First_Glyph+NumChar-1,SymbolMono18pt7b.last+128-32);
  Serial.print("Displaying "); Serial.print(NumChar,DEC);
  Serial.print(" glyphs in "); Serial.print(Max_R,DEC);
  Serial.print(" rows by "); Serial.print(Max_C,DEC);
  Serial.println(" columns.");
  Serial.print("Magnifier:"); Serial.println(Magnifier,DEC);
  Serial.print("Displaying characters:"), 
  Serial.print(First_Glyph,DEC); 
  Serial.print(" through ");
  Serial.println(Last_Glyph,DEC);
  Serial.println();
  display.fillScreen(COLOR_BLACK);  //Clear the screen
  if(Magnifier>1){    //If magnified, draw a grid for reference
    for (uint16_t C=0; C<DWIDTH; C=C+(DELTA_C*Magnifier)) {
      display.drawFastVLine(C,0,DHEIGHT,COLOR_YELLOW); 
    }
    for (uint16_t R=0; R<DHEIGHT; R=R+(DELTA_R*Magnifier)) {
      display.drawFastHLine(0,R,DWIDTH,COLOR_YELLOW); 
    }
  }
  uint8_t I=0;
  for (uint8_t Glyph = First_Glyph; Glyph <= Last_Glyph; Glyph++) {
    drawSymbol((I % Max_C) * DELTA_C * Magnifier,
        (BASE_R*Magnifier) + (I / Max_C) * DELTA_R * Magnifier,
        Glyph,COLOR_WHITE,COLOR_BLACK,Magnifier);
    I++;
  }
}

void setup() {  
  Serial.begin(9600);while(! Serial) {}; delay (1000);
  Serial.println("Type a positive number to adjust the first character displayed.");
  Serial.println("Type a negative number to change the magnifier. ie -2 multiplied by two.");
  Initialize_Display();
  delay (1000);
  Magnifier=1;
  First_Glyph=0;
  Show ();
}

void loop() {
  int16_t val;
  if(Serial.available()) {
    val= Serial.parseInt();
    if (val<0) {
      Magnifier=-val;
    } else {
      First_Glyph=val;
    }
    while(Serial.available())  {Serial.read();};
    Show ();
  }
}
