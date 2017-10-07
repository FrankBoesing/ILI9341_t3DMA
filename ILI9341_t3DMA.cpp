/* ILI9341_t3DMA library code is placed under the MIT license
   Copyright (c) 2016 Frank Bösing

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/


//-DF_BUS=120000000

#include "ILI9341_t3DMA.h"

#define SPICLOCK 60e6

#define qBlink() {GPIOC_PTOR=32;} // This toggles the Teensy 3.2 Builtin LED pin 13

#define WIDTH  ILI9341_TFTWIDTH
#define HEIGHT ILI9341_TFTHEIGHT

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04


DMAMEM uint16_t screen[HEIGHT][WIDTH];
#ifdef ILI_DMA
DMASetting dmasettings[SCREEN_DMA_NUM_SETTINGS];
DMAChannel dmatx;
volatile uint8_t rstop = 0;
volatile uint8_t ntransfer = 0;
#endif

uint16_t * screen16 = (uint16_t*)&screen[0][0];
uint32_t * screen32 = (uint32_t*)&screen[0][0];
const uint32_t * screen32e = (uint32_t*)&screen[0][0] + sizeof(screen) / 4;

extern "C" const unsigned char glcdfont[];

static const uint8_t init_commands[] = {
	4, 0xEF, 0x03, 0x80, 0x02,
	4, 0xCF, 0x00, 0XC1, 0X30,
	5, 0xED, 0x64, 0x03, 0X12, 0X81,
	4, 0xE8, 0x85, 0x00, 0x78,
	6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
	2, 0xF7, 0x20,
	3, 0xEA, 0x00, 0x00,
	2, ILI9341_PWCTR1, 0x23, // Power control
	2, ILI9341_PWCTR2, 0x10, // Power control
	3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
	2, ILI9341_VMCTR2, 0x86, // VCM control2
	2, ILI9341_MADCTL, 0x48, // Memory Access Control
	2, ILI9341_PIXFMT, 0x55,
	3, ILI9341_FRMCTR1, 0x00, 0x18,
	4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
	2, 0xF2, 0x00, // Gamma Function Disable
	2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
	16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
		0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
	16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
		0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
	3, 0xb1, 0x00, 0x10, // FrameRate Control 119Hz
	
	2, ILI9341_MADCTL, MADCTL_MV | MADCTL_BGR,
	0
};

	void waitFifoNotFull(void) {
		uint32_t sr;
		uint32_t tmp __attribute__((unused));
		do {
			sr = KINETISK_SPI0.SR;
			if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
		} while ((sr & (15 << 12)) > (3 << 12));
	}
	
#ifdef ILI_DMA
void dmaInterrupt(void) {
	 
  dmatx.clearInterrupt();
#ifdef ILI_DMASCATTER_GATHER
  //digitalWriteFast(13,!digitalRead(13));
  qBlink();
  rstop = 1;
#else
  dmatx.disable();
  ntransfer++;
  if (ntransfer >= SCREEN_DMA_NUM_SETTINGS) {	  
    ntransfer = 0;
    rstop = 1;
    //qBlink();
  }
  dmatx = dmasettings[ntransfer];
  dmatx.enable();
#endif
}
#endif

ILI9341_t3DMA::ILI9341_t3DMA(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t mosi, uint8_t sclk, uint8_t miso)
{
	_cs   = cs;
	_dc   = dc;
	_rst  = rst;
	_mosi = mosi;
	_sclk = sclk;
	_miso = miso;
	_width    = WIDTH;
	_height   = HEIGHT;
	rotation  = 0;
	cursor_y  = cursor_x    = 0;
	textsize  = 1;
	textcolor = textbgcolor = 0xFFFF;
	wrap      = true;
	font      = NULL;
}

void ILI9341_t3DMA::begin(void) {

  pinMode(13, OUTPUT);
  
    pinMode(_dc, OUTPUT);
    pinMode(_cs, OUTPUT);
	digitalWriteFast(_cs, 1);
	digitalWriteFast(_dc, 1);
	
    SPI.setMOSI(_mosi);
    SPI.setMISO(_miso);
    SPI.setSCK(_sclk);
	SPI.begin();

	if (_rst < 255) {	// toggle RST low to reset
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(5);
		digitalWrite(_rst, LOW);
		delay(20);
		digitalWrite(_rst, HIGH);
		delay(150);
	}
	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	const uint8_t *addr = init_commands;
	
	digitalWriteFast(_cs, 0);
 	   
	while (1) {
		uint8_t count = *addr++;
		if (count-- == 0) break;
	    
		digitalWriteFast(_dc, 0);
		SPI.transfer(*addr++);		
		
		//writecommand_cont(*addr++);
		while (count-- > 0) {
			digitalWriteFast(_dc, 1);
			SPI.transfer(*addr++);
			//writedata8_cont(*addr++);
		}		
	}
	digitalWriteFast(_dc, 0);
	SPI.transfer(ILI9341_SLPOUT);
	//writecommand_last(ILI9341_SLPOUT);    // Exit Sleep
	digitalWriteFast(_dc, 1);
	digitalWriteFast(_cs, 1);
	SPI.endTransaction();

	delay(120); 		
	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	digitalWriteFast(_dc, 0);
	digitalWriteFast(_cs, 0);
	SPI.transfer(ILI9341_DISPON);
	//writecommand_last(ILI9341_DISPON);    // Display on
	digitalWriteFast(_dc, 1);
	digitalWriteFast(_cs, 1);	
	SPI.endTransaction();
	
#ifdef ILI_DMA
  const uint32_t bytesPerLine = ILI9341_TFTWIDTH * 2;
  const uint32_t maxLines = (65536 / bytesPerLine);
  uint32_t i = 0, sum = 0, lines;
  do {

    //Source:
    lines = min(maxLines, ILI9341_TFTHEIGHT - sum);
    int32_t len = lines * bytesPerLine;
    dmasettings[i].TCD->CSR = 0;
    dmasettings[i].TCD->SADDR = &screen[sum][0];
    dmasettings[i].TCD->SOFF = 2;
    dmasettings[i].TCD->ATTR_SRC = 1;
    dmasettings[i].TCD->NBYTES = 2;
    dmasettings[i].TCD->SLAST = -len;
    dmasettings[i].TCD->BITER = len / 2;
    dmasettings[i].TCD->CITER = len / 2;

    //Destination:
    dmasettings[i].TCD->DADDR = &SPI0_PUSHR;
    dmasettings[i].TCD->DOFF = 0;
    dmasettings[i].TCD->ATTR_DST = 1;
    dmasettings[i].TCD->DLASTSGA = 0;

#ifdef ILI_DMASCATTER_GATHER
    dmasettings[i].replaceSettingsOnCompletion(dmasettings[i + 1]);
#else
    dmasettings[i].interruptAtCompletion();
    dmasettings[i].disableOnCompletion();

#endif
    sum += lines;
  } while (++i < SCREEN_DMA_NUM_SETTINGS);

#ifdef ILI_DMASCATTER_GATHER
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].interruptAtCompletion();
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].replaceSettingsOnCompletion(dmasettings[0]);
#else
  dmatx.attachInterrupt(dmaInterrupt);
#endif

  dmatx.begin(false);
  dmatx.triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX );
  dmatx = dmasettings[0];

  //NVIC_SET_PRIORITY(IRQ_UART0_STATUS, 16);
 
#endif //ILI_DMA


  fillScreen(ILI9341_BLACK);
};

void ILI9341_t3DMA::start(void) {
#ifdef ILI_DMA
  if (started) return;
#endif
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT);

  //SPI.usingInterrupt(IRQ_DMA_CH1);  

  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
  digitalWriteFast(_cs, 0);
  digitalWriteFast(_dc, 0);

  SPI.transfer(ILI9341_CASET);
  digitalWriteFast(_dc, 1);
  SPI.transfer16(0);
  //SPI.transfer16(_width);  
  SPI.transfer16(319);

  SPI.transfer(ILI9341_PASET);
  digitalWriteFast(_dc, 1);
  SPI.transfer16(0);
  //SPI.transfer16(_height);
  SPI.transfer16(239);
  
  digitalWriteFast(_dc, 0);
  SPI.transfer(ILI9341_RAMWR);

  digitalWriteFast(_dc, 1);
  digitalWriteFast(_cs,0);

#ifdef ILI_DMA
  SPI0_RSER |= SPI_RSER_TFFF_DIRS | SPI_RSER_TFFF_RE;  // Set ILI_DMA Interrupt Request Select and Enable register
  SPI0_MCR &= ~SPI_MCR_HALT;  //Start transfers.
  SPI0_CTAR0 = SPI0_CTAR1;
  (*(volatile uint16_t *)((int)&SPI0_PUSHR + 2)) = (SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT) >> 16; //Enable 16 Bit Transfers + Continue-Bit

  started = 1;
#endif
}


void ILI9341_t3DMA::refresh(void) {
//	Serial.printf("Channel:%d %d", dmatx.channel, IRQ_DMA_CH0 + dmatx.channel);
#ifdef ILI_DMA
  start();
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].TCD->CSR &= ~DMA_TCD_CSR_DREQ; //disable "disableOnCompletion"
  dmatx.enable();

  ntransfer = 0;
  dmatx = dmasettings[0];

  rstop = 0;
  autorefresh = 1;
#endif
}

void ILI9341_t3DMA::stopRefresh(void) {
#ifdef ILI_DMA
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].disableOnCompletion();  
  wait();
  dmatx.disable();
#endif
  digitalWriteFast(_dc, 1);
  digitalWriteFast(_cs, 1);
#ifdef ILI_DMA
  
  autorefresh = 0;
  started = 0;
#endif
  SPI.endTransaction();
  //SPI.notUsingInterrupt(IRQ_DMA_CH1); 

}

void ILI9341_t3DMA::refreshOnce(void) {
#ifdef ILI_DMA
  if (!autorefresh) {
    refresh();
    stopRefresh();
  }
#else
  start();

  for (int i=0; i<ILI9341_TFTHEIGHT * ILI9341_TFTWIDTH; i++) {	  
	  KINETISK_SPI0.PUSHR = screen16[i] | SPI_PUSHR_CTAS(1);
	  waitFifoNotFull();
  }
  stopRefresh();
#endif
}

void ILI9341_t3DMA::wait(void) {
#ifdef ILI_DMA
  while (!rstop) {
   // asm volatile("wfi");
  };
#endif
}



void ILI9341_t3DMA::setRotation(uint8_t m)
{
/*	
#ifdef ILI_DMA
	int aautorefresh = autorefresh;
	if (autorefresh) {
		stopRefresh();
	}
#else 
	start();		
#endif	    
*/
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
  digitalWriteFast(_cs, 0);


	rotation = m % 4; // can't be higher than 3	
	
	digitalWriteFast(_dc, 0);
	SPI.transfer(ILI9341_MADCTL);
	digitalWriteFast(_dc, 1);
	//writecommand_cont(ILI9341_MADCTL);
	switch (rotation) {
	case 0:
		SPI.transfer(MADCTL_MX | MADCTL_BGR);
//		writedata8_last(MADCTL_MX | MADCTL_BGR);
		_width  = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case 1:
		SPI.transfer(MADCTL_MV | MADCTL_BGR);
//		writedata8_last(MADCTL_MV | MADCTL_BGR);
		_width  = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	case 2:
		SPI.transfer(MADCTL_MY | MADCTL_BGR);
//		writedata8_last(MADCTL_MY | MADCTL_BGR);
		_width  = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case 3:
		SPI.transfer(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
//		writedata8_last(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		_width  = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	}
	digitalWriteFast(_cs, 1);	
	SPI.endTransaction();
	//start();
/*	
#ifdef ILI_DMA	
	if (aautorefresh) {
		refresh();
	}
#else
    stopRefresh();
#endif
*/
	cursor_x = 0;
	cursor_y = 0;

}

void ILI9341_t3DMA::setScroll(uint16_t offset)
{
/*
	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	writecommand_cont(ILI9341_VSCRSADD);
	writedata16_last(offset);
	SPI.endTransaction();
*/	
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
/*******************************************************************************************************************/

void ILI9341_t3DMA::fillScreen(uint16_t color) {

  uint32_t col32 = (color << 16) | color;
  uint32_t * p = screen32;

  do {
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
  } while (p < screen32e);

}

void ILI9341_t3DMA::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
#ifdef ENABLE_SCREEN_ROTATE
  screen16[y*_width +x] = color;
#else
  screen[y][x] = color;
#endif
}

void ILI9341_t3DMA::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height)) return;
  if ((y + h - 1) >= _height) h = _height - y;
#ifdef ENABLE_SCREEN_ROTATE
  // Lets start off by simply addressing memory
  int screen_index = y*_width + x;
  for (;h>0; h--) {
  	screen16[screen_index] = color;
  	screen_index += _width;
  }
#else
  while (h-- > 0) {
	screen[y + h][x] = color;
  }
#endif

}

void ILI9341_t3DMA::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((x+w-1) >= _width)  w = _width-x;
#ifdef ENABLE_SCREEN_ROTATE
    int screen_index = y*_width + x;
#endif
	while (w-- > 0)	 {
#ifdef ENABLE_SCREEN_ROTATE
		screen16[screen_index++] = color;
#else
		screen[y][x + w] = color;
#endif
	}
}

// fill a rectangle
void ILI9341_t3DMA::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	// rudimentary clipping (drawChar w/big text requires this)
	if((x >= _width) || (y >= _height)) return;
	if(x < 0) {	w += x; x = 0; 	}
	if(y < 0) {	h += y; y = 0; 	}
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

#ifdef ENABLE_SCREEN_ROTATE
    int screen_index_row = y*_width + x;
    for (;h>0; h--) {
    	int screen_index = screen_index_row;
    	for (int i = 0 ;i < w; i++) {
    		screen16[screen_index++] = color;
    }
    screen_index_row += _width;
  }

#else
	for(int i=h-1; i>=0; i--) {
		for(int j=w-1; j>=0;j--) {
			screen[y + i][x + j] = color;
		}
	}
#endif
};

void ILI9341_t3DMA::writeRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors) {
    for (int i=0; i<w; i++)
        for (int j=0; j<h; j++) {
            screen[y+j][x+i] = pcolors[(j * w) + i];
			//screen16[ (240-(y+j)) + (x+i)*240 ] = pcolors[(j * w) + i];
        }
};

void ILI9341_t3DMA::writeRectScale(int16_t x, int16_t y, int16_t orig_w, int16_t orig_h, int16_t dest_w, int16_t dest_h, const uint16_t *pcolors) {
    for (int i=0; i<dest_w; i++)
        for (int j=0; j<dest_h; j++) {
            uint16_t x2 = (orig_w * i / dest_w );
            uint16_t y2 = (orig_h * j / dest_h );
            screen[y+j][x+i] = pcolors[(y2 * orig_w) + x2];
        }
};

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).	It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.	 All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

//#include "glcdfont.c"
extern "C" const unsigned char glcdfont[];

// Draw a circle outline
void ILI9341_t3DMA::drawCircle(int16_t x0, int16_t y0, int16_t r,
	uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0	 , y0+r, color);
  drawPixel(x0	 , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
	if (f >= 0) {
	  y--;
	  ddF_y += 2;
	  f += ddF_y;
	}
	x++;
	ddF_x += 2;
	f += ddF_x;

	drawPixel(x0 + x, y0 + y, color);
	drawPixel(x0 - x, y0 + y, color);
	drawPixel(x0 + x, y0 - y, color);
	drawPixel(x0 - x, y0 - y, color);
	drawPixel(x0 + y, y0 + x, color);
	drawPixel(x0 - y, y0 + x, color);
	drawPixel(x0 + y, y0 - x, color);
	drawPixel(x0 - y, y0 - x, color);
  }
}

void ILI9341_t3DMA::drawCircleHelper( int16_t x0, int16_t y0,
			   int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f		= 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x		= 0;
  int16_t y		= r;

  while (x<y) {
	if (f >= 0) {
	  y--;
	  ddF_y += 2;
	  f		+= ddF_y;
	}
	x++;
	ddF_x += 2;
	f	  += ddF_x;
	if (cornername & 0x4) {
	  drawPixel(x0 + x, y0 + y, color);
	  drawPixel(x0 + y, y0 + x, color);
	}
	if (cornername & 0x2) {
	  drawPixel(x0 + x, y0 - y, color);
	  drawPixel(x0 + y, y0 - x, color);
	}
	if (cornername & 0x8) {
	  drawPixel(x0 - y, y0 + x, color);
	  drawPixel(x0 - x, y0 + y, color);
	}
	if (cornername & 0x1) {
	  drawPixel(x0 - y, y0 - x, color);
	  drawPixel(x0 - x, y0 - y, color);
	}
  }
}

void ILI9341_t3DMA::fillCircle(int16_t x0, int16_t y0, int16_t r,
				  uint16_t color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void ILI9341_t3DMA::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
	uint8_t cornername, int16_t delta, uint16_t color) {

  int16_t f		= 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x		= 0;
  int16_t y		= r;

  while (x<y) {
	if (f >= 0) {
	  y--;
	  ddF_y += 2;
	  f		+= ddF_y;
	}
	x++;
	ddF_x += 2;
	f	  += ddF_x;

	if (cornername & 0x1) {
	  drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
	  drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
	}
	if (cornername & 0x2) {
	  drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
	  drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
	}
  }
}


// Bresenham's algorithm - thx wikpedia
void ILI9341_t3DMA::drawLine(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1, uint16_t color)
{
	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine(x0, y0, x1 - x0 + 1, color);
		} else if (x1 < x0) {
			drawFastHLine(x1, y0, x0 - x1 + 1, color);
		} else {
			drawPixel(x0, y0, color);
		}
		return;
	} else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine(x0, y0, y1 - y0 + 1, color);
		} else {
			drawFastVLine(x0, y1, y0 - y1 + 1, color);
		}
		return;
	}

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	int16_t xbegin = x0;
	if (steep) {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastHLine(y0, xbegin, len + 1, color);
				} else {
					drawPixel(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			drawFastVLine(y0, xbegin, x0 - xbegin, color);
		}

	} else {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastHLine(xbegin, y0, len + 1, color);
				} else {
					drawPixel(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			drawFastHLine(xbegin, y0, x0 - xbegin, color);
		}
	}
}

// Draw a rectangle
void ILI9341_t3DMA::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	drawFastHLine(x, y, w, color);
	drawFastHLine(x, y+h-1, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x+w-1, y, h, color);
}

// Draw a rounded rectangle
void ILI9341_t3DMA::drawRoundRect(int16_t x, int16_t y, int16_t w,
  int16_t h, int16_t r, uint16_t color) {
  // smarter version
  drawFastHLine(x+r  , y	 , w-2*r, color); // Top
  drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x	  , y+r	 , h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r	 , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r	   , y+r	, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r	, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r	   , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void ILI9341_t3DMA::fillRoundRect(int16_t x, int16_t y, int16_t w,
				 int16_t h, int16_t r, uint16_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r	   , y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
void ILI9341_t3DMA::drawTriangle(int16_t x0, int16_t y0,
				int16_t x1, int16_t y1,
				int16_t x2, int16_t y2, uint16_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void ILI9341_t3DMA::fillTriangle ( int16_t x0, int16_t y0,
				  int16_t x1, int16_t y1,
				  int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
	swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
	swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
	swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
	a = b = x0;
	if(x1 < a)		a = x1;
	else if(x1 > b) b = x1;
	if(x2 < a)		a = x2;
	else if(x2 > b) b = x2;
	drawFastHLine(a, y0, b-a+1, color);
	return;
  }

  int16_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa	 = 0,
	sb	 = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;	// Include y1 scanline
  else		   last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
	a	= x0 + sa / dy01;
	b	= x0 + sb / dy02;
	sa += dx01;
	sb += dx02;
	if(a > b) swap(a,b);
	drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
	a	= x1 + sa / dy12;
	b	= x0 + sb / dy02;
	sa += dx12;
	sb += dx02;
	if(a > b) swap(a,b);
	drawFastHLine(a, y, b-a+1, color);
  }
}

void ILI9341_t3DMA::drawBitmap(int16_t x, int16_t y,
				  const uint8_t *bitmap, int16_t w, int16_t h,
				  uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
	for(i=0; i<w; i++ ) {
	  if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
			drawPixel(x+i, y+j, color);
	  }
	}
  }
}


size_t ILI9341_t3DMA::write(uint8_t c)
{
	if (font) {
		if (c == '\n') {
			//cursor_y += ??
			cursor_x = 0;
		} else {
			drawFontChar(c);
		}
	} else {
		if (c == '\n') {
			cursor_y += textsize*8;
			cursor_x  = 0;
		} else if (c == '\r') {
			// skip em
		} else {
			drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
			cursor_x += textsize*6;
			if (wrap && (cursor_x > (_width - textsize*6))) {
				cursor_y += textsize*8;
				cursor_x = 0;
			}
		}
	}
	return 1;
}

// Draw a character
void ILI9341_t3DMA::drawChar(int16_t x, int16_t y, unsigned char c,
				uint16_t fgcolor, uint16_t bgcolor, uint8_t size)
{
	if((x >= _width)			|| // Clip right
	   (y >= _height)			|| // Clip bottom
	   ((x + 6 * size - 1) < 0) || // Clip left	 TODO: is this correct?
	   ((y + 8 * size - 1) < 0))   // Clip top	 TODO: is this correct?
		return;

	if (fgcolor == bgcolor) {
		// This transparent approach is only about 20% faster
		if (size == 1) {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (glcdfont[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						drawFastHLine(x + xoff, y + yoff, 5, fgcolor);
						break;
					} else if (line == 0x1E) {
						drawFastHLine(x + xoff, y + yoff, 4, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						drawFastHLine(x + xoff, y + yoff, 3, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						drawFastHLine(x + xoff, y + yoff, 2, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						drawPixel(x + xoff, y + yoff, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		} else {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (glcdfont[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						fillRect(x + xoff * size, y + yoff * size,
							5 * size, size, fgcolor);
						break;
					} else if (line == 0x1E) {
						fillRect(x + xoff * size, y + yoff * size,
							4 * size, size, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						fillRect(x + xoff * size, y + yoff * size,
							3 * size, size, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						fillRect(x + xoff * size, y + yoff * size,
							2 * size, size, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						fillRect(x + xoff * size, y + yoff * size,
							size, size, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		}
	} else {
		// This solid background approach is about 5 time faster
		//setAddr(x, y, x + 6 * size - 1, y + 8 * size - 1); //TODO
		//writecommand_cont(ILI9341_RAMWR); //TODO
		uint32_t yr;
		uint32_t mask = 0x01;
		uint16_t color;
		uint32_t ox = x;
		uint32_t oy = y;
		for (y=0; y < 8; y++) {
			for (yr=0; yr < size; yr++) {
				for (x=0; x < 5; x++) {
					if (glcdfont[c * 5 + x] & mask) {
						color = fgcolor;
					} else {
						color = bgcolor;
					}
					drawFastHLine(ox, oy+yr, size, color);
				}
				drawFastHLine(ox, oy+yr, size, color);
			}
			mask = mask << 1;
		}
	}
}

static uint32_t fetchbit(const uint8_t *p, uint32_t index)
{
	if (p[index >> 3] & (1 << (7 - (index & 7)))) return 1;
	return 0;
}

static uint32_t fetchbits_unsigned(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = 0;
	do {
		uint8_t b = p[index >> 3];
		uint32_t avail = 8 - (index & 7);
		if (avail <= required) {
			val <<= avail;
			val |= b & ((1 << avail) - 1);
			index += avail;
			required -= avail;
		} else {
			b >>= avail - required;
			val <<= required;
			val |= b & ((1 << required) - 1);
			break;
		}
	} while (required);
	return val;
}

static uint32_t fetchbits_signed(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = fetchbits_unsigned(p, index, required);
	if (val & (1 << (required - 1))) {
		return (int32_t)val - (1 << required);
	}
	return (int32_t)val;
}


void ILI9341_t3DMA::drawFontChar(unsigned int c)
{
	uint32_t bitoffset;
	const uint8_t *data;

	//Serial.printf("drawFontChar %d\n", c);

	if (c >= font->index1_first && c <= font->index1_last) {
		bitoffset = c - font->index1_first;
		bitoffset *= font->bits_index;
	} else if (c >= font->index2_first && c <= font->index2_last) {
		bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
		bitoffset *= font->bits_index;
	} else if (font->unicode) {
		return; // TODO: implement sparse unicode
	} else {
		return;
	}
	//Serial.printf("  index =	%d\n", fetchbits_unsigned(font->index, bitoffset, font->bits_index));
	data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);

	uint32_t encoding = fetchbits_unsigned(data, 0, 3);
	if (encoding != 0) return;
	uint32_t width = fetchbits_unsigned(data, 3, font->bits_width);
	bitoffset = font->bits_width + 3;
	uint32_t height = fetchbits_unsigned(data, bitoffset, font->bits_height);
	bitoffset += font->bits_height;
	//Serial.printf("  size =	%d,%d\n", width, height);

	int32_t xoffset = fetchbits_signed(data, bitoffset, font->bits_xoffset);
	bitoffset += font->bits_xoffset;
	int32_t yoffset = fetchbits_signed(data, bitoffset, font->bits_yoffset);
	bitoffset += font->bits_yoffset;
	//Serial.printf("  offset = %d,%d\n", xoffset, yoffset);

	uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
	bitoffset += font->bits_delta;
	//Serial.printf("  delta =	%d\n", delta);

	//Serial.printf("  cursor = %d,%d\n", cursor_x, cursor_y);

	// horizontally, we draw every pixel, or none at all
	if (cursor_x < 0) cursor_x = 0;
	int32_t origin_x = cursor_x + xoffset;
	if (origin_x < 0) {
		cursor_x -= xoffset;
		origin_x = 0;
	}
	if (origin_x + (int)width > _width) {
		if (!wrap) return;
		origin_x = 0;
		if (xoffset >= 0) {
			cursor_x = 0;
		} else {
			cursor_x = -xoffset;
		}
		cursor_y += font->line_space;
	}
	if (cursor_y >= _height) return;
	cursor_x += delta;

	// vertically, the top and/or bottom can be clipped
	int32_t origin_y = cursor_y + font->cap_height - height - yoffset;
	//Serial.printf("  origin = %d,%d\n", origin_x, origin_y);

	// TODO: compute top skip and number of lines
	int32_t linecount = height;
	//uint32_t loopcount = 0;
	uint32_t y = origin_y;
	while (linecount) {
		//Serial.printf("	 linecount = %d\n", linecount);
		uint32_t b = fetchbit(data, bitoffset++);
		if (b == 0) {
			//Serial.println("	  single line");
			uint32_t x = 0;
			do {
				uint32_t xsize = width - x;
				if (xsize > 32) xsize = 32;
				uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
				drawFontBits(bits, xsize, origin_x + x, y, 1);
				bitoffset += xsize;
				x += xsize;
			} while (x < width);
			y++;
			linecount--;
		} else {
			uint32_t n = fetchbits_unsigned(data, bitoffset, 3) + 2;
			bitoffset += 3;
			uint32_t x = 0;
			do {
				uint32_t xsize = width - x;
				if (xsize > 32) xsize = 32;
				//Serial.printf("	 multi line %d\n", n);
				uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
				drawFontBits(bits, xsize, origin_x + x, y, n);
				bitoffset += xsize;
				x += xsize;
			} while (x < width);
			y += n;
			linecount -= n;
		}
		//if (++loopcount > 100) {
			//Serial.println("	   abort draw loop");
			//break;
		//}
	}
}

void ILI9341_t3DMA::drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat)
{
#if 1
	// TODO: replace this *slow* code with something fast...
	//Serial.printf("	   %d bits at %d,%d: %X\n", numbits, x, y, bits);
	if (bits == 0) return;
	do {
		uint32_t x1 = x;
		uint32_t n = numbits;
		do {
			n--;
			if (bits & (1 << n)) {
				drawPixel(x1, y, textcolor);
				//Serial.printf("		 pixel at %d,%d\n", x1, y);
			}
			x1++;
		} while (n > 0);
		y++;
		repeat--;
	} while (repeat);
#endif
#if 0
	if (bits == 0) return;
	int w = 0;
	do {
		uint32_t x1 = x;
		uint32_t n = numbits;

		writecommand_cont(ILI9341_PASET); // Row addr set
		writedata16_cont(y);   // YSTART
		writedata16_cont(y);   // YEND

		do {
			n--;
			if (bits & (1 << n)) {
				w++;
			}
			else if (w > 0) {
				// "drawFastHLine(x1 - w, y, w, textcolor)"
				writecommand_cont(ILI9341_CASET); // Column addr set
				writedata16_cont(x1 - w);	// XSTART
				writedata16_cont(x1);	// XEND
				writecommand_cont(ILI9341_RAMWR);
				while (w-- > 1) { // draw line
					writedata16_cont(textcolor);
				}
				writedata16_last(textcolor);
			}

			x1++;
		} while (n > 0);

		if (w > 0) {

				writecommand_cont(ILI9341_CASET); // Column addr set
				writedata16_cont(x1 - w);	// XSTART
				writedata16_cont(x1);	// XEND
				writecommand_cont(ILI9341_RAMWR);
				while (w-- > 1) { //draw line
					writedata16_cont(textcolor);
				}
				writedata16_last(textcolor);
		}

		y++;
		repeat--;
	} while (repeat);
#endif
}

void ILI9341_t3DMA::drawRotText(const char* c, bool compress) {
  while (*c != '\0') {
  	if (*c != ' ') drawRotChar(*c++, compress);
  	else  {
	   cursor_y -= 3;
	   if (textcolor != textbgcolor) fillRect(max(0,cursor_x-7), max(0,cursor_y+1), 8, 3, textbgcolor);
	   c++;
	}
  }
}

void ILI9341_t3DMA::drawRotChar(unsigned char c, bool compress) {
  uint_fast8_t j = 0;
  uint_fast16_t c5 = c * 5;
  bool xFlag = false;
  if (textcolor != textbgcolor) {
    while (j < 5) {
      uint_fast8_t mask = 0x01, i = 8;
      xFlag = false;
      while (i--) {
        if (glcdfont[c5 + j] & mask) {
        	drawPixel( cursor_x - i,  cursor_y, textcolor);
          xFlag = true;
        }
        else drawPixel( cursor_x - i,  cursor_y, textbgcolor);
        mask = mask << 1;
      }
      if (xFlag || !compress) cursor_y--;
      mask = mask << 1;
      j++;
    }
    drawFastHLine(max(0,cursor_x-7), max(0,cursor_y), 8, textbgcolor);
    cursor_y--;
  }
  else {
    while (j < 5) {
      uint_fast8_t mask = 0x01, i = 8;
      xFlag = false;
      while (i--) {
        if (glcdfont[c5 + j] & mask) {
          drawPixel( cursor_x - i,  cursor_y, textcolor);
          xFlag = true;
        }
        mask = mask << 1;
      }
      if (xFlag || !compress) cursor_y--;
      j++;
    }
    cursor_y--;
  }
}

void ILI9341_t3DMA::setCursor(int16_t x, int16_t y) {
	if (x < 0) x = 0;
	else if (x >= _width) x = _width - 1;
	cursor_x = x;
	if (y < 0) y = 0;
	else if (y >= _height) y = _height - 1;
	cursor_y = y;
}

void ILI9341_t3DMA::getCursor(int16_t *x, int16_t *y) {
  *x = cursor_x;
  *y = cursor_y;
}

void ILI9341_t3DMA::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

uint8_t ILI9341_t3DMA::getTextSize() {
	return textsize;
}

void ILI9341_t3DMA::setTextColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void ILI9341_t3DMA::setTextColor(uint16_t c, uint16_t b) {
  textcolor   = c;
  textbgcolor = b;
}

void ILI9341_t3DMA::setTextWrap(boolean w) {
  wrap = w;
}

boolean ILI9341_t3DMA::getTextWrap()
{
	return wrap;
}