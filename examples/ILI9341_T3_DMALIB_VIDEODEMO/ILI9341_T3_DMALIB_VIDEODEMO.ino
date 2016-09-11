
//
//Demovideo see :
//
//https://drive.google.com/drive/folders/0Bx2Jw84lqebkelF3MHg2eWVzb0U
//
// This sketch works with std 180MHz and less, but better with 240MHZ and overclocked F_BUS, or 144MHz and overclocked F_BUS to the max.
//

#include <SD.h>
#include <SdFat.h>
#include <SPI.h>
#include <Audio.h>
#include <ILI9341_t3DMA.h>

#define SCREEN_WIDTH  ILI9341_TFTWIDTH
#define SCREEN_HEIGHT ILI9341_TFTHEIGHT

#define TFT_DC      15
#define TFT_CS      10
#define TFT_RST     4  // 255 = unused, connect to 3.3V
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12

ILI9341_t3DMA tft = ILI9341_t3DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

SdFatSdio sd;
File file;


// GUItool: begin automatically generated code
AudioPlayQueue           queue1;         //xy=183,163
AudioOutputAnalog        dac1;           //xy=376,154
AudioConnection          patchCord1(queue1, dac1);
// GUItool: end automatically generated code

const float fps = 23.98;
const int rate_us = 1e6 / fps;

uint8_t header[512];

void setup() {
  AudioMemory(30);

  tft.begin();
  tft.setRotation(3);
  tft.dfillScreen(ILI9341_BLUE);
  tft.refresh();
  delay(500);

  if (!sd.begin()) {
    sd.initErrorHalt();
  }

  if (!file.open("myvideo.bin", O_READ)) {
    sd.errorHalt("open failed");
  }

  Serial.println("Begin playing");

  //read header
  file.read(header, 512);
}


uint32_t audioPos = 0;
const uint32_t framesize = 5 * 32768;
const uint16_t * screen16e = ( uint16_t*) (screen32 + (320 + 240 * 2) / 4);

int16_t *lb;
int16_t *rb;

void loop() {
  uint32_t m;
  m = micros();
  uint32_t * p = screen32;
  uint32_t sdbufsize = 65536;
  uint32_t rd = 0;
  int bytesread;
  do {
    do {
      if (sdbufsize & framesize) {
        bytesread = file.read(p, sdbufsize);
        if (!bytesread) {
          file.seek(512);
          break;
        }
        p += sdbufsize / 4;
        rd += sdbufsize;
      } else {
        sdbufsize /= 2;
      }
    } while (rd < framesize);
  } while (!bytesread);


  //Audio begins at position 320*240*2 and is 1839 samples*2 channels
  int16_t * ap = ( int16_t*) screen32;
  ap += 320 * 240;
  int i = 0;
  int16_t l, r;
  for (i = 0; i < 1839; i++) {
    if (audioPos == 0) {
      lb = queue1.getBuffer();
    }
    *lb++ = (*ap) * 2 ;
    ap++;
    ap++;
    audioPos += 1;
    if (audioPos >= 128) {
      audioPos = 0;
      queue1.playBuffer();
    }

  }


  while (micros() - m < rate_us);

}

