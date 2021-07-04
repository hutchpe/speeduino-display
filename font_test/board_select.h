/*
 * This file contains definitions for all of the supported graphic boards.
 */


#ifdef USE_ILI9341
  /*
   * Definitions for the 2.4 inch 320x240 TFT FeatherWing
   * https://www.adafruit.com/product/3315
   */
  //Assumes Feather M0 or M4 or other compatible
  #define STMPE_CS 6
  #define TFT_CS   9
  #define TFT_DC   10
  #define SD_CS    5

  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
  #define DWIDTH 320
  #define DHEIGHT 240
  #define COLOR_BLACK  ILI9341_BLACK
  #define COLOR_YELLOW ILI9341_YELLOW
  #define COLOR_WHITE  ILI9341_WHITE
  void Initialize_Display(void){ 
    display.begin();
    display.setRotation(1);
  };
#endif

#ifdef USE_HX8357
  /*
   * Definitions for the 3.5 inch 480x220 TFT FeatherWing
   * https://www.adafruit.com/product/3651
   */
  //Assumes Feather M0 or M4 or other compatible
  #define STMPE_CS 6
  #define TFT_CS   9
  #define TFT_DC   10
  #define SD_CS    5

  #include <Adafruit_GFX.h>
  #include <Adafruit_HX8357.h>
  Adafruit_HX8357 display = Adafruit_HX8357(TFT_CS, TFT_DC);
  #define DWIDTH 480
  #define DHEIGHT 320
  #define COLOR_BLACK  HX8357_BLACK
  #define COLOR_YELLOW HX8357_YELLOW
  #define COLOR_WHITE  HX8357_WHITE
  void Initialize_Display(void){
    display.begin();
    display.setRotation(1);
  };
#endif

#ifdef USE_HALLOWING
  /*
   * Definitions for the Hallowing M0 Express
   * https://www.adafruit.com/product/3900
   */
  //Hardwired for hallowing
  #define TFT_CS     39
  #define TFT_RST    37
  #define TFT_DC     38
  #define TFT_BACKLIGHT 7

  #include <Adafruit_GFX.h>
  #include <Adafruit_ST7735.h>
  Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
  #define DWIDTH 128
  #define DHEIGHT 128
  #define COLOR_BLACK  ST7735_BLACK
  #define COLOR_YELLOW ST7735_YELLOW
  #define COLOR_WHITE  ST7735_WHITE
  void Initialize_Display(void){ 
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
    display.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
    display.setRotation(2);
  };
#endif

#ifdef USE_PYGAMER
  /*
   * Definitions for the PyGamer M4 Express
   * https://www.adafruit.com/product/4242
   */
  #include <Adafruit_Arcada.h>
  Adafruit_Arcada display;
  #define DWIDTH 160
  #define DHEIGHT 128
  #define COLOR_BLACK  ST77XX_BLACK
  #define COLOR_YELLOW ST77XX_YELLOW
  #define COLOR_WHITE  ST77XX_WHITE
  void Initialize_Display(void){ 
    display.begin(); 
    display.displayBegin();
    display.setBacklight(255);
  };
#endif
