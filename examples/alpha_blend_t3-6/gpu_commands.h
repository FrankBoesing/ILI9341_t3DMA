#ifndef GPU_DRAW_H_
#define GPU_DRAW_H_

#include <SPI.h>
#include <DMAChannel.h>
#include <ILI9341_t3.h>
#include <ILI9341_t3DMA.h>

class GPU {
  bool auto_refresh;
  ILI9341_t3DMA tft = ILI9341_t3DMA(9,10);
private:
  uint16_t blendColor(uint16_t bgpix, uint16_t forepix, float alpha) {
    if ( alpha < 0 ) {
      return bgpix;
    }
    if ( alpha > 1) {
      return forepix;
    }
    uint8_t red, green, blue,forered,foregreen,foreblue,finalred,finalgreen,finalblue;
    red = (bgpix & 0xf800) >> 11 << 3;
    green = (bgpix & 0x7E0) >> 5 << 2;
    blue = (bgpix & 0x1F) << 3;
    
    forered = (forepix & 0xf800) >> 11 <<3;
    foregreen = (forepix & 0x7E0) >> 5 <<2;
    foreblue = (forepix & 0x1F) <<3 ;
    
    finalred = (red * (1 - alpha) + (forered * alpha));
    finalgreen = (green * (1 - alpha) + (foregreen * alpha));
    finalblue = (blue * (1 - alpha) + (foreblue * alpha));
    //return ((finalred / 8) << 11) | ((finalgreen / 4) << 5) | (finalblue / 8);
    return (uint16_t)((finalred & 0xF8) << 8) | ((finalgreen & 0xFC) << 3) | (finalblue >> 3);
  }
public:

  GPU() {
  }

  void start_auto_refresh() {
    if (!auto_refresh) {
      auto_refresh = 1;
      tft.refresh();
    }
  }
  
  void init_screen() {
    delay(200);
    tft.begin();
    auto_refresh = 0;
  }
  void fillScreen(uint16_t color) {
    tft.dfillScreen(color);
  }
  
  void blend_screen(int16_t startx, int16_t starty, int16_t end_x, int16_t end_y, uint16_t color, float AA) {
    uint16_t bgbuffer;
    for (int16_t y = starty; y < end_y; y++) {
      for (int16_t x = startx; x < end_x; x++) {
        bgbuffer = blendColor(tft.dgetPixel(x,y),color, AA); //pixel
        tft.ddrawPixel(x,y,bgbuffer);
      }
    }
  }
};

#endif
