#include "SPI.h"
#include <ILI9341_t3DMA.h>

//THESE ARE ALT PINS! for use with audio shield
#define TFT_DC      20
#define TFT_CS      21
#define TFT_RST    255  
#define TFT_MOSI     7
#define TFT_SCLK    14
#define TFT_MISO    12

ILI9341_t3DMA tft = ILI9341_t3DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

void setup() {
  tft.begin();
  Serial.begin(9600);
  while (!Serial) ;
  tft.refresh();
}

int x=0;
void loop(void) {
  x++;
  testText();
  delay(10);
}

unsigned long testText() {
 // tft.dfillScreen(ILI9341_BLACK);
 //drawRotText( unsigned char* Text, bool CompressBlankSpace)
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK); 
  tft.setCursor(100, x);
  tft.ddrawRotText("abcdef ghijklm nopqrstuv wxyz1234 567890",1);
 
  tft.setTextColor(ILI9341_WHITE,ILI9341_RED); 
  tft.setCursor(140, x);
  tft.ddrawRotText("abcdef ghijklm nopqrstuv wxyz1234 567890");
}
