/* ILI9341_t3DMA library code is placed under the MIT license
 * Copyright (c) 2016 Frank Bösing
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#ifndef _ILI9341_t3DMAH_
#define _ILI9341_t3DMAH_

#include <SPI.h>
#include <DMAChannel.h>
#include <ILI9341_t3.h>


#define SCREEN_DMA_NUM_SETTINGS (((uint32_t)((2 * ILI9341_TFTHEIGHT * ILI9341_TFTWIDTH) / 65536UL))+1)
extern uint32_t * screen32 ;

class ILI9341_t3DMA: public ILI9341_t3
{
  public:
	ILI9341_t3DMA(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, uint8_t _MOSI = 11, uint8_t _SCLK = 13, uint8_t _MISO = 12): ILI9341_t3(_CS, _DC, _RST, _MOSI, _SCLK, _MISO) {}
	void begin(void);

	void refresh(void);	//starts continously refreshing the screen
	void stopRefresh(void);	 //stops continously refreshing the screen
	void refreshOnce(void); //one single screen refresh
	void wait(void); //waits until current refresh is done

	void dfillScreen(uint16_t color); //fills buffer with color

	void ddrawPixel(int16_t x, int16_t y, uint16_t color);
	void ddrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void ddrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void dfillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color); // fill a rectangle
	void dwriteRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);//TODO
	
	void ddrawRotChar(unsigned char c, bool compress);
	void ddrawRotText(const  char* c, bool compress=0);
	
	// from Adafruit_GFX.h
	void ddrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void ddrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
	void dfillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void dfillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
	void ddrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void dfillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void ddrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void dfillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void ddrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
	void ddrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);//TODO
	void ddrawFontChar(unsigned int c);
	void ddrawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat);
	virtual size_t write(uint8_t); //overrides write from ILI9341_t3DMA

	void ddrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void ddrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	//void drawFontChar(unsigned int c);;//TODO

	void start(void);
	void fill(void);
 protected:

	//void drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat);


 private:
	DMASetting dmasettings[SCREEN_DMA_NUM_SETTINGS];
	uint8_t started = 0;
	uint8_t autorefresh = 0;

};


#endif
