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
 
#include "ILI9341_t3DMA.h"

#define SPICLOCK 90e6

DMAMEM uint16_t screen[ILI9341_TFTHEIGHT+22][ILI9341_TFTWIDTH];
uint16_t * screen16 = (uint16_t*)&screen[0][0];
uint32_t * screen32 = (uint32_t*)&screen[0][0];
const uint32_t * screen32e = (uint32_t*)&screen[0][0] + sizeof(screen) / 4;

DMAChannel dmatx;
volatile uint8_t rstop = 0;


void dmaInterrupt(void) {
  ///  digitalWriteFast(1,!digitalReadFast(1));
  rstop = 1;
  dmatx.clearInterrupt();

}

void ILI9341_t3DMA::begin(void) {
  ILI9341_t3::begin();
  const uint32_t bytesPerLine = ILI9341_TFTWIDTH * 2;
  const uint32_t maxLines = (65536 / bytesPerLine);
  uint32_t i = 0, sum = 0, lines;
  do {

	//Source:
	lines = min(maxLines, ILI9341_TFTHEIGHT - sum);
	uint32_t len = lines * bytesPerLine;
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
	dmasettings[i].replaceSettingsOnCompletion(dmasettings[i + 1]);
	sum += lines;
  } while (++i < SCREEN_DMA_NUM_SETTINGS);
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].replaceSettingsOnCompletion(dmasettings[0]);

  dmatx.begin(false);
  dmatx.triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX );
  dmatx = dmasettings[0];

  dmatx.attachInterrupt(dmaInterrupt);
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].interruptAtCompletion();

  dfillScreen(0);
};

void ILI9341_t3DMA::start(void) {
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
  setAddr(0, 0, _width, _height);
  writecommand_cont(ILI9341_RAMWR);
}

void ILI9341_t3DMA::fill(void) {

  for (int y = 0; y < ILI9341_TFTHEIGHT; y++)
	for (int x = 0; x < ILI9341_TFTWIDTH; x++)
	  writedata16_cont(screen[y][x]);

};

void ILI9341_t3DMA::refresh(void) {
  if (!started) {
	start();
	fill();//TODO : Why is fill() needed ?
	started = 1;

  }
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].TCD->CSR &= ~DMA_TCD_CSR_DREQ; //disable "disableOnCompletion"
  SPI0_RSER |= SPI_RSER_TFFF_DIRS |	 SPI_RSER_TFFF_RE;	 // Set DMA Interrupt Request Select and Enable register
  SPI0_MCR &= ~SPI_MCR_HALT;  //Start transfers.
  dmatx.enable();
  rstop = 0;
  autorefresh = 1;
}

void ILI9341_t3DMA::stopRefresh(void) {
  dmasettings[SCREEN_DMA_NUM_SETTINGS - 1].disableOnCompletion();
  //wait();  
  SPI.endTransaction();
  autorefresh = 0;
}

void ILI9341_t3DMA::refreshOnce(void) {
  if (!autorefresh) {
	refresh();
	stopRefresh();
  }
}

void ILI9341_t3DMA::wait(void) {
  while (!rstop) {
	asm volatile("wfi");
  };
}

void ILI9341_t3DMA::dfillScreen(uint16_t color) {

  uint32_t col32 = (color << 16) | color;
  uint32_t * p = screen32;

  do {
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
	*p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32; *p++ = col32;
  } while (p < screen32e);

}

void ILI9341_t3DMA::ddrawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
  screen[y][x] = color;
  //screen16[y*_height+x*_width*2] = color;
}

void ILI9341_t3DMA::ddrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height)) return;
  if ((y + h - 1) >= _height) h = _height - y;
  while (h-- > 0) {
	screen[y + h][x] = color;
  }
}

void ILI9341_t3DMA::ddrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((x+w-1) >= _width)  w = _width-x;
	while (w-- > 0)	 {
		screen[y][x + w] = color;
		//screen16[y*_height+x*_width+w] = color;
	}
}

// fill a rectangle
void ILI9341_t3DMA::dfillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	// rudimentary clipping (drawChar w/big text requires this)
	if((x >= _width) || (y >= _height)) return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

	for(int i=h-1; i>=0; i--) {
		for(int j=w-1; j>=0;j--) {
			screen[y + i][x + j] = color;
		}
	}
};

void ILI9341_t3DMA::dwriteRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors) {
//TODO : memcopy??
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
void ILI9341_t3DMA::ddrawCircle(int16_t x0, int16_t y0, int16_t r,
	uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  ddrawPixel(x0	 , y0+r, color);
  ddrawPixel(x0	 , y0-r, color);
  ddrawPixel(x0+r, y0  , color);
  ddrawPixel(x0-r, y0  , color);

  while (x<y) {
	if (f >= 0) {
	  y--;
	  ddF_y += 2;
	  f += ddF_y;
	}
	x++;
	ddF_x += 2;
	f += ddF_x;

	ddrawPixel(x0 + x, y0 + y, color);
	ddrawPixel(x0 - x, y0 + y, color);
	ddrawPixel(x0 + x, y0 - y, color);
	ddrawPixel(x0 - x, y0 - y, color);
	ddrawPixel(x0 + y, y0 + x, color);
	ddrawPixel(x0 - y, y0 + x, color);
	ddrawPixel(x0 + y, y0 - x, color);
	ddrawPixel(x0 - y, y0 - x, color);
  }
}

void ILI9341_t3DMA::ddrawCircleHelper( int16_t x0, int16_t y0,
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
	  ddrawPixel(x0 + x, y0 + y, color);
	  ddrawPixel(x0 + y, y0 + x, color);
	}
	if (cornername & 0x2) {
	  ddrawPixel(x0 + x, y0 - y, color);
	  ddrawPixel(x0 + y, y0 - x, color);
	}
	if (cornername & 0x8) {
	  ddrawPixel(x0 - y, y0 + x, color);
	  ddrawPixel(x0 - x, y0 + y, color);
	}
	if (cornername & 0x1) {
	  ddrawPixel(x0 - y, y0 - x, color);
	  ddrawPixel(x0 - x, y0 - y, color);
	}
  }
}

void ILI9341_t3DMA::dfillCircle(int16_t x0, int16_t y0, int16_t r,
				  uint16_t color) {
  ddrawFastVLine(x0, y0-r, 2*r+1, color);
  dfillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void ILI9341_t3DMA::dfillCircleHelper(int16_t x0, int16_t y0, int16_t r,
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
	  ddrawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
	  ddrawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
	}
	if (cornername & 0x2) {
	  ddrawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
	  ddrawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
	}
  }
}


// Bresenham's algorithm - thx wikpedia
void ILI9341_t3DMA::ddrawLine(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1, uint16_t color)
{
	if (y0 == y1) {
		if (x1 > x0) {
			ddrawFastHLine(x0, y0, x1 - x0 + 1, color);
		} else if (x1 < x0) {
			ddrawFastHLine(x1, y0, x0 - x1 + 1, color);
		} else {
			ddrawPixel(x0, y0, color);
		}
		return;
	} else if (x0 == x1) {
		if (y1 > y0) {
			ddrawFastVLine(x0, y0, y1 - y0 + 1, color);
		} else {
			ddrawFastVLine(x0, y1, y0 - y1 + 1, color);
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
					ddrawFastHLine(y0, xbegin, len + 1, color);
				} else {
					ddrawPixel(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			ddrawFastVLine(y0, xbegin, x0 - xbegin, color);
		}

	} else {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					ddrawFastHLine(xbegin, y0, len + 1, color);
				} else {
					ddrawPixel(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			ddrawFastHLine(xbegin, y0, x0 - xbegin, color);
		}
	}
}

// Draw a rectangle
void ILI9341_t3DMA::ddrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	ddrawFastHLine(x, y, w, color);
	ddrawFastHLine(x, y+h-1, w, color);
	ddrawFastVLine(x, y, h, color);
	ddrawFastVLine(x+w-1, y, h, color);
}

// Draw a rounded rectangle
void ILI9341_t3DMA::ddrawRoundRect(int16_t x, int16_t y, int16_t w,
  int16_t h, int16_t r, uint16_t color) {
  // smarter version
  ddrawFastHLine(x+r  , y	 , w-2*r, color); // Top
  ddrawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  ddrawFastVLine(x	  , y+r	 , h-2*r, color); // Left
  ddrawFastVLine(x+w-1, y+r	 , h-2*r, color); // Right
  // draw four corners
  ddrawCircleHelper(x+r	   , y+r	, r, 1, color);
  ddrawCircleHelper(x+w-r-1, y+r	, r, 2, color);
  ddrawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  ddrawCircleHelper(x+r	   , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void ILI9341_t3DMA::dfillRoundRect(int16_t x, int16_t y, int16_t w,
				 int16_t h, int16_t r, uint16_t color) {
  // smarter version
  dfillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  dfillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  dfillCircleHelper(x+r	   , y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
void ILI9341_t3DMA::ddrawTriangle(int16_t x0, int16_t y0,
				int16_t x1, int16_t y1,
				int16_t x2, int16_t y2, uint16_t color) {
  ddrawLine(x0, y0, x1, y1, color);
  ddrawLine(x1, y1, x2, y2, color);
  ddrawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void ILI9341_t3DMA::dfillTriangle ( int16_t x0, int16_t y0,
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
	ddrawFastHLine(a, y0, b-a+1, color);
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
	ddrawFastHLine(a, y, b-a+1, color);
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
	ddrawFastHLine(a, y, b-a+1, color);
  }
}

void ILI9341_t3DMA::ddrawBitmap(int16_t x, int16_t y,
				  const uint8_t *bitmap, int16_t w, int16_t h,
				  uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
	for(i=0; i<w; i++ ) {
	  if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
			ddrawPixel(x+i, y+j, color);
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
			ddrawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
			cursor_x += textsize*6;
			if (wrap && (cursor_x > (_width - textsize*6))) {
				cursor_y += textsize*8;
				cursor_x = 0;
			}
		}
	}
	return 1;
}

void ILI9341_t3DMA::ddrawRotText(const char* c, bool compress) {
  while (*c != '\0') {
  	if (*c != ' ') ddrawRotChar(*c++, compress);
  	else  {
  		
	   cursor_y -= 3; 
	   if (textcolor != textbgcolor) { 
	   		dfillRect(max(0,cursor_x-7), max(0,cursor_y+1), 8, 3, textbgcolor);
	   }
	   c++;
	   }
  }
}

void ILI9341_t3DMA::ddrawRotChar(unsigned char c, bool compress) {
  uint_fast8_t j = 0;
  uint_fast16_t c5 = c * 5;
  uint16_t *color;
  uint16_t xAdjusted =  cursor_y;
  bool xFlag = false;
  if (textcolor != textbgcolor) {
    while (j < 5) {
      uint_fast8_t mask = 0x01, i = 8;
      xFlag = false;
      while (i--) {
        if (glcdfont[c5 + j] & mask) { 
        ddrawPixel( cursor_x - i,  cursor_y, textcolor);
          xFlag = true;
        }
        else {
          ddrawPixel( cursor_x - i,  cursor_y, textbgcolor);
        }
        mask = mask << 1;
      }
      if (xFlag || !compress) {
      	 cursor_y--;
	  }
      mask = mask << 1;
      j++;
    }
    
    ddrawFastHLine(max(0,cursor_x-7), max(0,cursor_y), 8, textbgcolor);
    cursor_y--;
  }
  else {
    while (j < 5) {
      uint_fast8_t mask = 0x01, i = 8;
      xFlag = false;
      while (i--) {
        if (glcdfont[c5 + j] & mask) {
          ddrawPixel( cursor_x - i,  cursor_y, textcolor);
          xFlag = true;
        }
        mask = mask << 1;
      } 
      if (xFlag || !compress) {
      	 cursor_y--;
	  }
      j++;
    }
    cursor_y--;
  }
   
}

// Draw a character
void ILI9341_t3DMA::ddrawChar(int16_t x, int16_t y, unsigned char c,
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
						ddrawFastHLine(x + xoff, y + yoff, 5, fgcolor);
						break;
					} else if (line == 0x1E) {
						ddrawFastHLine(x + xoff, y + yoff, 4, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						ddrawFastHLine(x + xoff, y + yoff, 3, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						ddrawFastHLine(x + xoff, y + yoff, 2, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						ddrawPixel(x + xoff, y + yoff, fgcolor);
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
						dfillRect(x + xoff * size, y + yoff * size,
							5 * size, size, fgcolor);
						break;
					} else if (line == 0x1E) {
						dfillRect(x + xoff * size, y + yoff * size,
							4 * size, size, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						dfillRect(x + xoff * size, y + yoff * size,
							3 * size, size, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						dfillRect(x + xoff * size, y + yoff * size,
							2 * size, size, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						dfillRect(x + xoff * size, y + yoff * size,
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
					ddrawFastHLine(ox, oy+yr, size, color);
				}
				ddrawFastHLine(ox, oy+yr, size, color);
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


void ILI9341_t3DMA::ddrawFontChar(unsigned int c)
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
				ddrawFontBits(bits, xsize, origin_x + x, y, 1);
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
				ddrawFontBits(bits, xsize, origin_x + x, y, n);
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

void ILI9341_t3DMA::ddrawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat)
{
#if 0
	// TODO: replace this *slow* code with something fast...
	//Serial.printf("	   %d bits at %d,%d: %X\n", numbits, x, y, bits);
	if (bits == 0) return;
	do {
		uint32_t x1 = x;
		uint32_t n = numbits;
		do {
			n--;
			if (bits & (1 << n)) {
				ddrawPixel(x1, y, textcolor);
				//Serial.printf("		 pixel at %d,%d\n", x1, y);
			}
			x1++;
		} while (n > 0);
		y++;
		repeat--;
	} while (repeat);
#endif
#if 1
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
