#include "SPI.h"
#include <ILI9341_t3DMA.h>

#define TFT_DC      15
#define TFT_CS      10
#define TFT_RST     4  // 255 = unused, connect to 3.3V
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12

ILI9341_t3DMA tft = ILI9341_t3DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

unsigned char vu_img_sm1_bmp[] = {
  0x42, 0x4d, 0x8a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8a, 0x00,
  0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x20, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x00, 0x00, 0x23, 0x2e, 0x00, 0x00, 0x23, 0x2e, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0xe0, 0x07,
  0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x47,
  0x52, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0x59, 0x02, 0x6a, 0x43, 0x7a,
  0xa4, 0x8a, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x9a, 0xe4, 0x9a, 0xe4, 0xa2,
  0x25, 0xab, 0x45, 0xab, 0x85, 0xb3, 0xc6, 0xbb, 0x06, 0xc4, 0x46, 0xcc,
  0x67, 0xcc, 0xa7, 0xd4, 0xc8, 0xd4, 0xe8, 0xdc, 0x08, 0xdd, 0x29, 0xdd,
  0x49, 0xdd, 0x69, 0xe5, 0x8a, 0xe5, 0xaa, 0xe5, 0xaa, 0xed, 0xcb, 0xed,
  0xcb, 0xed, 0xcb, 0xed, 0xca, 0xed, 0xca, 0xe5, 0xca, 0xed, 0xeb, 0xed,
  0xeb, 0xed, 0x0b, 0xee, 0x0b, 0xf6, 0x2b, 0xf6, 0x2c, 0xf6, 0x2c, 0xf6,
  0x2b, 0xf6, 0x0b, 0xf6, 0x0b, 0xf6, 0xca, 0xed, 0x8a, 0xed, 0x49, 0xe5,
  0x08, 0xdd, 0xc7, 0xdc, 0x87, 0xd4, 0x47, 0xcc, 0x06, 0xcc, 0xc6, 0xc3,
  0xa6, 0xbb, 0x65, 0xb3, 0x45, 0xab, 0x25, 0xab, 0x24, 0xa3, 0x04, 0xa3,
  0xe4, 0x9a, 0xc4, 0x9a, 0xa4, 0x92, 0xa4, 0x8a, 0x43, 0x72, 0xc2, 0x61,
  0x62, 0x41, 0xa2, 0x59, 0x03, 0x6a, 0x63, 0x7a, 0xa4, 0x8a, 0xc4, 0x92,
  0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x9a, 0x04, 0xa3, 0x25, 0xab, 0x65, 0xb3,
  0x65, 0xb3, 0xa5, 0xbb, 0xc5, 0xbb, 0xe6, 0xc3, 0x26, 0xc4, 0x47, 0xcc,
  0x87, 0xd4, 0xa7, 0xd4, 0xe8, 0xdc, 0x29, 0xdd, 0x49, 0xe5, 0x49, 0xe5,
  0x69, 0xe5, 0x8a, 0xe5, 0xaa, 0xed, 0xca, 0xed, 0xcb, 0xed, 0xeb, 0xed,
  0xeb, 0xed, 0xeb, 0xf5, 0xeb, 0xed, 0x0b, 0xf6, 0x0b, 0xf6, 0x0b, 0xf6,
  0x2b, 0xf6, 0x2b, 0xf6, 0x2b, 0xf6, 0x0b, 0xf6, 0xeb, 0xf5, 0xeb, 0xf5,
  0xcb, 0xed, 0xaa, 0xed, 0x6a, 0xe5, 0x29, 0xe5, 0xc8, 0xdc, 0x87, 0xd4,
  0x47, 0xcc, 0x26, 0xcc, 0x06, 0xc4, 0xc6, 0xc3, 0x85, 0xbb, 0x85, 0xb3,
  0x65, 0xab, 0x25, 0xa3, 0x05, 0xa3, 0xe4, 0x9a, 0xe4, 0x9a, 0xc4, 0x92,
  0xa4, 0x8a, 0xa4, 0x8a, 0x43, 0x7a, 0xe2, 0x61, 0x82, 0x49, 0xc2, 0x59,
  0x03, 0x6a, 0x43, 0x7a, 0xa4, 0x8a, 0xa4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xe4, 0x9a, 0x24, 0xa3, 0x45, 0xab, 0x65, 0xb3, 0x85, 0xb3, 0xc5, 0xbb,
  0xe6, 0xbb, 0xe6, 0xc3, 0x06, 0xc4, 0x26, 0xc4, 0x67, 0xcc, 0x87, 0xd4,
  0xc8, 0xdc, 0x09, 0xdd, 0x49, 0xe5, 0x8a, 0xe5, 0x8a, 0xed, 0xaa, 0xed,
  0xaa, 0xed, 0xca, 0xed, 0xeb, 0xf5, 0xeb, 0xf5, 0xeb, 0xf5, 0x0b, 0xf6,
  0x0b, 0xf6, 0x0b, 0xf6, 0x0b, 0xf6, 0x0b, 0xf6, 0xeb, 0xf5, 0xeb, 0xf5,
  0xeb, 0xf5, 0xca, 0xed, 0xaa, 0xed, 0xaa, 0xed, 0x8a, 0xed, 0x69, 0xe5,
  0x09, 0xdd, 0xc8, 0xdc, 0x87, 0xd4, 0x26, 0xcc, 0x26, 0xcc, 0xe6, 0xc3,
  0xc6, 0xbb, 0xa6, 0xbb, 0x85, 0xb3, 0x65, 0xb3, 0x45, 0xab, 0x25, 0xa3,
  0x04, 0xa3, 0xe4, 0x9a, 0xe4, 0x9a, 0xc4, 0x92, 0xa4, 0x8a, 0x84, 0x82,
  0x43, 0x72, 0xe2, 0x61, 0x82, 0x49, 0xc2, 0x59, 0x02, 0x6a, 0x43, 0x7a,
  0xa4, 0x8a, 0xa4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x9a, 0x04, 0xa3,
  0x25, 0xab, 0x64, 0xab, 0x85, 0xb3, 0xa5, 0xbb, 0xc5, 0xbb, 0xe6, 0xc3,
  0x06, 0xc4, 0x06, 0xc4, 0x27, 0xcc, 0x67, 0xcc, 0xa8, 0xd4, 0xe8, 0xdc,
  0x09, 0xdd, 0x49, 0xe5, 0x6a, 0xe5, 0x8a, 0xed, 0xaa, 0xed, 0xaa, 0xed,
  0xcb, 0xed, 0xcb, 0xed, 0xcb, 0xf5, 0xcb, 0xf5, 0xcb, 0xf5, 0xcb, 0xf5,
  0xcb, 0xf5, 0xcb, 0xf5, 0xab, 0xed, 0xaa, 0xed, 0xaa, 0xed, 0x8a, 0xed,
  0x6a, 0xed, 0x49, 0xe5, 0x09, 0xdd, 0xc8, 0xdc, 0xa8, 0xd4, 0x67, 0xd4,
  0x26, 0xcc, 0xe6, 0xc3, 0xe6, 0xc3, 0xc6, 0xbb, 0xa6, 0xbb, 0x65, 0xb3,
  0x65, 0xb3, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x05, 0x9b, 0x05, 0x9b,
  0xe4, 0x9a, 0xc4, 0x92, 0xa4, 0x8a, 0x84, 0x82, 0x43, 0x72, 0xe2, 0x61,
  0xa2, 0x51, 0xe2, 0x61, 0x03, 0x6a, 0x63, 0x7a, 0xa4, 0x8a, 0xa4, 0x92,
  0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x9a, 0xe4, 0x9a, 0x24, 0xa3, 0x25, 0xab,
  0x65, 0xab, 0x85, 0xb3, 0xa5, 0xb3, 0xa5, 0xbb, 0xc6, 0xbb, 0xe6, 0xc3,
  0x06, 0xc4, 0x27, 0xc4, 0x67, 0xcc, 0xa8, 0xd4, 0xc8, 0xdc, 0x08, 0xdd,
  0x29, 0xe5, 0x49, 0xe5, 0x49, 0xe5, 0x69, 0xed, 0x6a, 0xed, 0x6a, 0xed,
  0x8a, 0xed, 0x8a, 0xed, 0x6a, 0xed, 0x8a, 0xed, 0x8a, 0xed, 0x6a, 0xed,
  0x69, 0xed, 0x49, 0xe5, 0x29, 0xe5, 0x09, 0xe5, 0x08, 0xdd, 0xc8, 0xdc,
  0xa8, 0xd4, 0x88, 0xd4, 0x67, 0xd4, 0x47, 0xcc, 0x06, 0xc4, 0xe6, 0xc3,
  0xc6, 0xbb, 0xa6, 0xbb, 0x85, 0xb3, 0x65, 0xb3, 0x45, 0xab, 0x45, 0xab,
  0x25, 0xab, 0x25, 0xa3, 0x05, 0x9b, 0xe5, 0x9a, 0xe4, 0x92, 0xc4, 0x92,
  0xa4, 0x8a, 0x84, 0x82, 0x43, 0x7a, 0xe2, 0x61, 0xa2, 0x51, 0xe2, 0x61,
  0x03, 0x6a, 0x63, 0x7a, 0xa4, 0x8a, 0xa4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xe4, 0x92, 0xe4, 0x9a, 0x04, 0xa3, 0x05, 0xa3, 0x25, 0xa3, 0x45, 0xab,
  0x85, 0xb3, 0xa5, 0xb3, 0x85, 0xb3, 0xa5, 0xbb, 0xe6, 0xbb, 0xe6, 0xc3,
  0x26, 0xc4, 0x67, 0xcc, 0x87, 0xd4, 0xa7, 0xd4, 0xc8, 0xdc, 0xe8, 0xdc,
  0x08, 0xdd, 0x09, 0xdd, 0x09, 0xdd, 0x29, 0xe5, 0x29, 0xe5, 0x29, 0xe5,
  0x29, 0xe5, 0x29, 0xe5, 0x09, 0xe5, 0x49, 0xe5, 0x29, 0xe5, 0xe8, 0xdc,
  0xc8, 0xdc, 0xc8, 0xdc, 0xa8, 0xdc, 0xa8, 0xd4, 0x87, 0xd4, 0x67, 0xcc,
  0x47, 0xcc, 0x27, 0xc4, 0xe6, 0xc3, 0xc6, 0xbb, 0xa5, 0xbb, 0x85, 0xb3,
  0x65, 0xb3, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3, 0x05, 0x9b,
  0x05, 0x9b, 0xe4, 0x9a, 0xe4, 0x92, 0xc4, 0x92, 0xa4, 0x8a, 0x84, 0x82,
  0x43, 0x7a, 0xc2, 0x59, 0xa2, 0x51, 0xe3, 0x61, 0x23, 0x6a, 0x64, 0x7a,
  0xa4, 0x8a, 0xa4, 0x8a, 0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x9a,
  0x04, 0x9b, 0x04, 0x9b, 0x04, 0xa3, 0x44, 0xa3, 0x45, 0xab, 0x65, 0xb3,
  0x85, 0xb3, 0x85, 0xb3, 0xa5, 0xbb, 0xe6, 0xbb, 0x06, 0xc4, 0x26, 0xcc,
  0x47, 0xcc, 0x67, 0xcc, 0x67, 0xd4, 0xa8, 0xd4, 0xa8, 0xdc, 0xe8, 0xe4,
  0x64, 0x72, 0xc5, 0x82, 0xe8, 0xe4, 0xe8, 0xdc, 0xe8, 0xdc, 0x09, 0xe5,
  0xa4, 0x82, 0xa3, 0x51, 0xa4, 0x59, 0xc5, 0x8a, 0xc8, 0xdc, 0x87, 0xd4,
  0x67, 0xd4, 0x67, 0xcc, 0x47, 0xcc, 0x27, 0xcc, 0x06, 0xc4, 0xe6, 0xc3,
  0xc6, 0xbb, 0xa6, 0xbb, 0x85, 0xb3, 0x65, 0xb3, 0x45, 0xab, 0x25, 0xab,
  0x25, 0xa3, 0x25, 0xa3, 0x05, 0x9b, 0x05, 0x9b, 0xe5, 0x9a, 0xe5, 0x9a,
  0xc4, 0x92, 0xc4, 0x92, 0xa4, 0x8a, 0x84, 0x82, 0x43, 0x72, 0xc2, 0x59,
  0xa2, 0x51, 0xe3, 0x61, 0x03, 0x6a, 0x64, 0x7a, 0xa4, 0x8a, 0xa4, 0x8a,
  0xa4, 0x8a, 0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x9a, 0x04, 0x9b, 0x05, 0x9b,
  0x05, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x45, 0xab, 0x45, 0xab, 0x65, 0xb3,
  0x85, 0xb3, 0xc5, 0xbb, 0xe6, 0xbb, 0xe6, 0xc3, 0x06, 0xc4, 0x27, 0xcc,
  0x47, 0xcc, 0x47, 0xcc, 0x47, 0xcc, 0x86, 0xab, 0x81, 0x10, 0x61, 0x10,
  0x47, 0xcc, 0x87, 0xd4, 0x87, 0xd4, 0xa4, 0x82, 0x81, 0x18, 0x66, 0x9b,
  0x45, 0x9b, 0x81, 0x10, 0x84, 0x82, 0x47, 0xcc, 0x27, 0xcc, 0x27, 0xc4,
  0x06, 0xc4, 0x06, 0xc4, 0xe6, 0xc3, 0xc6, 0xbb, 0xa6, 0xbb, 0x85, 0xb3,
  0x85, 0xb3, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3, 0x04, 0x9b,
  0x05, 0x9b, 0xe5, 0x9a, 0xe5, 0x9a, 0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xa4, 0x8a, 0x84, 0x82, 0x43, 0x7a, 0xc2, 0x59, 0xa2, 0x51, 0xe2, 0x61,
  0x03, 0x6a, 0x63, 0x7a, 0x84, 0x82, 0xa4, 0x8a, 0xc4, 0x8a, 0xc4, 0x92,
  0xc4, 0x92, 0xe4, 0x92, 0x04, 0x93, 0x04, 0x9b, 0x04, 0xa3, 0x25, 0xa3,
  0x25, 0xab, 0x25, 0xab, 0x45, 0xab, 0x65, 0xb3, 0x65, 0xb3, 0xa5, 0xbb,
  0xa6, 0xbb, 0xa6, 0xb3, 0xe6, 0xbb, 0xe6, 0xc3, 0x06, 0xc4, 0x06, 0xc4,
  0x26, 0xcc, 0x43, 0x41, 0xe3, 0x59, 0x63, 0x49, 0xc3, 0x59, 0x47, 0xcc,
  0x47, 0xcc, 0x44, 0x6a, 0x02, 0x31, 0x67, 0xd4, 0x47, 0xcc, 0x22, 0x39,
  0x23, 0x6a, 0x06, 0xc4, 0xe6, 0xc3, 0xe6, 0xc3, 0xe6, 0xbb, 0xc6, 0xbb,
  0xa6, 0xbb, 0xa6, 0xb3, 0x85, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x45, 0xab,
  0x25, 0xab, 0x25, 0xa3, 0x05, 0xa3, 0x05, 0xa3, 0xe5, 0x9a, 0xe5, 0x9a,
  0xe4, 0x92, 0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xa4, 0x8a, 0x84, 0x82,
  0x43, 0x72, 0xc2, 0x59, 0xa2, 0x51, 0xe2, 0x61, 0x03, 0x6a, 0x43, 0x72,
  0x84, 0x82, 0xa4, 0x8a, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x92,
  0xe4, 0x92, 0xe4, 0x9a, 0xe4, 0xa2, 0x04, 0xa3, 0x25, 0xab, 0x25, 0xab,
  0x45, 0xab, 0x65, 0xab, 0x65, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x86, 0xb3,
  0xa6, 0xb3, 0xa6, 0xbb, 0xc6, 0xbb, 0xe6, 0xc3, 0x86, 0xb3, 0x41, 0x08,
  0x25, 0x9b, 0x25, 0x9b, 0x21, 0x08, 0x06, 0xc4, 0x06, 0xc4, 0x24, 0x6a,
  0xe2, 0x28, 0x26, 0xcc, 0x06, 0xcc, 0x22, 0x31, 0x04, 0x62, 0xc6, 0xbb,
  0xc6, 0xbb, 0xc6, 0xbb, 0xa6, 0xbb, 0xa6, 0xb3, 0x85, 0xb3, 0x85, 0xb3,
  0x65, 0xb3, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3,
  0x05, 0xa3, 0x04, 0x9b, 0xe4, 0x9a, 0xe4, 0x92, 0xe4, 0x92, 0xc4, 0x92,
  0xc4, 0x92, 0xc4, 0x92, 0xa4, 0x8a, 0x84, 0x82, 0x43, 0x7a, 0xc2, 0x59,
  0xa2, 0x51, 0xe3, 0x61, 0x03, 0x6a, 0x63, 0x7a, 0x84, 0x82, 0xa4, 0x8a,
  0xa4, 0x92, 0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x92, 0xe4, 0x92, 0x04, 0x9b,
  0x05, 0x9b, 0xe5, 0x9a, 0x04, 0xa3, 0x25, 0xa3, 0x45, 0xab, 0x45, 0xab,
  0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x66, 0xb3, 0x86, 0xb3, 0x86, 0xb3,
  0xa6, 0xbb, 0xc6, 0xbb, 0x03, 0x62, 0x42, 0x39, 0xe6, 0xc3, 0x06, 0xc4,
  0xe2, 0x28, 0x44, 0x72, 0xc6, 0xbb, 0x44, 0x72, 0x02, 0x31, 0x06, 0xc4,
  0xe6, 0xc3, 0x42, 0x39, 0x04, 0x62, 0xa6, 0xbb, 0xa6, 0xb3, 0xa6, 0xb3,
  0x85, 0xb3, 0x85, 0xb3, 0x65, 0xb3, 0x65, 0xab, 0x65, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x25, 0xab, 0x25, 0xa3, 0x05, 0xa3, 0x05, 0x9b, 0x05, 0x9b,
  0xe4, 0x9a, 0xe4, 0x9a, 0xe4, 0x92, 0xc4, 0x92, 0xa4, 0x92, 0xc4, 0x8a,
  0xa4, 0x8a, 0x84, 0x82, 0x43, 0x7a, 0xc2, 0x59, 0xa2, 0x51, 0xe3, 0x61,
  0xe3, 0x69, 0x44, 0x7a, 0x84, 0x82, 0xa4, 0x8a, 0xc4, 0x8a, 0xc4, 0x92,
  0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x9a, 0x05, 0x9b, 0x04, 0x9b, 0xe4, 0x9a,
  0x04, 0x9b, 0x05, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x45, 0xab, 0x45, 0xab,
  0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3,
  0xc6, 0xbb, 0xc6, 0xbb, 0xa6, 0xbb, 0xa6, 0xbb, 0xc6, 0xc3, 0xc6, 0xbb,
  0xa6, 0xbb, 0xc6, 0xbb, 0xe6, 0xc3, 0xa6, 0xbb, 0xa6, 0xbb, 0xc6, 0xbb,
  0xa6, 0xbb, 0x85, 0xb3, 0x85, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xa3, 0x05, 0x9b, 0x05, 0x9b, 0xe5, 0x9a, 0xe4, 0x9a, 0xe4, 0x9a,
  0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xa4, 0x8a, 0xa4, 0x8a, 0x84, 0x82,
  0x43, 0x7a, 0xc2, 0x59, 0xa2, 0x51, 0xe2, 0x61, 0x03, 0x6a, 0x43, 0x7a,
  0xa4, 0x82, 0xa4, 0x8a, 0xa4, 0x8a, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xe4, 0x9a, 0xe4, 0x9a, 0xe4, 0x9a, 0x05, 0x9b, 0x04, 0x9b, 0x05, 0xa3,
  0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x45, 0xab, 0x45, 0xab, 0x65, 0xab,
  0x65, 0xab, 0x65, 0xb3, 0x65, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3,
  0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3,
  0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x85, 0xb3, 0x65, 0xb3, 0x65, 0xb3,
  0x65, 0xab, 0x65, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x25, 0xab,
  0x45, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x05, 0xa3, 0x05, 0x9b, 0x05, 0x9b,
  0x04, 0x9b, 0xe4, 0x9a, 0xe5, 0x9a, 0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0x84, 0x82, 0x43, 0x72, 0xc2, 0x59,
  0xa2, 0x51, 0xe3, 0x61, 0x03, 0x6a, 0x64, 0x7a, 0xa4, 0x82, 0xa4, 0x8a,
  0xa4, 0x8a, 0xa4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x9a,
  0xe4, 0x9a, 0xe5, 0x9a, 0x04, 0x9b, 0x04, 0x9b, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xa3, 0x25, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x65, 0xab, 0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3,
  0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3, 0x65, 0xb3,
  0x65, 0xb3, 0x65, 0xb3, 0x65, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x05, 0xa3,
  0x05, 0xa3, 0x05, 0x9b, 0x04, 0x9b, 0xe5, 0x9a, 0xe4, 0x9a, 0xe5, 0x9a,
  0xe5, 0x92, 0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x8a, 0xa4, 0x8a,
  0xa4, 0x8a, 0x84, 0x82, 0x43, 0x72, 0xc2, 0x59, 0xa2, 0x51, 0xe3, 0x61,
  0x03, 0x6a, 0x63, 0x7a, 0xa4, 0x8a, 0xa4, 0x82, 0xa4, 0x8a, 0xa4, 0x8a,
  0xa4, 0x8a, 0xa4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x9a, 0xe5, 0x9a,
  0x05, 0x9b, 0x05, 0x9b, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xa3, 0x05, 0xa3, 0x25, 0xa3, 0x05, 0xa3, 0x05, 0x9b, 0x05, 0x9b,
  0xe5, 0x9a, 0xe5, 0x9a, 0xe5, 0x9a, 0xe5, 0x9a, 0xc4, 0x92, 0xc4, 0x92,
  0xc4, 0x92, 0xc4, 0x92, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0x64, 0x82,
  0x43, 0x72, 0xc2, 0x59, 0xa2, 0x51, 0xe3, 0x61, 0xe2, 0x61, 0x43, 0x72,
  0x84, 0x82, 0x84, 0x82, 0x23, 0x7a, 0xc3, 0x59, 0x23, 0x72, 0x64, 0x7a,
  0xc4, 0x92, 0xa4, 0x8a, 0xc4, 0x92, 0xe4, 0x9a, 0x04, 0x9b, 0x05, 0x9b,
  0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x45, 0xa3, 0x45, 0xa3,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3, 0x05, 0xa3, 0x05, 0x9b,
  0x05, 0x9b, 0x05, 0x9b, 0xe5, 0x9a, 0xe4, 0x9a, 0xe4, 0x9a, 0xe5, 0x9a,
  0xe4, 0x9a, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xa4, 0x8a,
  0xc4, 0x8a, 0xa4, 0x8a, 0x84, 0x82, 0x63, 0x7a, 0x23, 0x72, 0xc2, 0x59,
  0x82, 0x51, 0xe3, 0x61, 0xe2, 0x61, 0x43, 0x72, 0x84, 0x82, 0xa1, 0x28,
  0x61, 0x10, 0xe3, 0x59, 0xa4, 0x82, 0xa4, 0x8a, 0xc3, 0x59, 0xe4, 0x92,
  0xc4, 0x8a, 0xc4, 0x92, 0xe4, 0x92, 0x04, 0x9b, 0x05, 0x9b, 0x05, 0xa3,
  0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab,
  0x45, 0xab, 0x25, 0xa3, 0x45, 0xab, 0x45, 0xab, 0x45, 0xab, 0x45, 0xa3,
  0x25, 0xa3, 0x45, 0xab, 0x45, 0xab, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xa3, 0x25, 0xa3, 0x05, 0xa3, 0x05, 0xa3, 0x05, 0x9b, 0x05, 0x9b,
  0xe5, 0x9a, 0xe4, 0x9a, 0xe4, 0x92, 0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xc4, 0x92, 0xa4, 0x92, 0xe4, 0x92, 0xe3, 0x81, 0xa3, 0x70, 0x63, 0x79,
  0xa4, 0x82, 0x63, 0x7a, 0x23, 0x72, 0xc2, 0x59, 0xa2, 0x49, 0xe3, 0x61,
  0xe2, 0x61, 0x23, 0x72, 0xa1, 0x20, 0xe2, 0x28, 0xa3, 0x49, 0xc3, 0x59,
  0xa1, 0x20, 0x42, 0x39, 0x64, 0x7a, 0xe4, 0x92, 0xe4, 0x92, 0x82, 0x49,
  0xe4, 0x92, 0xe4, 0x92, 0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x9a, 0xe5, 0x9a,
  0xe4, 0x9a, 0x05, 0x9b, 0x25, 0xa3, 0x05, 0xa3, 0x25, 0xa3, 0x45, 0xa3,
  0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xab, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xa3, 0x25, 0xa3, 0x45, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3,
  0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x25, 0xa3, 0x05, 0xa3,
  0x05, 0x9b, 0xe4, 0x9a, 0xe5, 0x9a, 0xe5, 0x9a, 0xe4, 0x92, 0xc4, 0x92,
  0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xe4, 0x92, 0x64, 0x8a, 0x63, 0x79,
  0x43, 0x70, 0x03, 0x68, 0x03, 0x68, 0x63, 0x70, 0x23, 0x71, 0x64, 0x7a,
  0x43, 0x72, 0xc2, 0x59, 0x82, 0x49, 0xe2, 0x61, 0xe2, 0x61, 0x42, 0x41,
  0x83, 0x51, 0xc3, 0x51, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x22, 0x31,
  0x42, 0x49, 0x81, 0x20, 0x62, 0x41, 0x43, 0x72, 0x04, 0x9b, 0xc4, 0x92,
  0xc4, 0x92, 0x82, 0x51, 0xe5, 0x92, 0xe4, 0x9a, 0xe4, 0x92, 0xc4, 0x92,
  0xe4, 0x92, 0x05, 0x9b, 0x04, 0x9b, 0x25, 0xa3, 0x25, 0xa3, 0x05, 0xa3,
  0x05, 0xa3, 0x04, 0x9b, 0x04, 0x9b, 0x05, 0x9b, 0x05, 0xa3, 0x25, 0x9b,
  0x25, 0xa3, 0x25, 0xa3, 0x05, 0x9b, 0x05, 0x9b, 0x05, 0xa3, 0x05, 0xa3,
  0x05, 0x9b, 0xe4, 0x9a, 0xe5, 0x9a, 0xe5, 0x9a, 0xe4, 0x9a, 0xe4, 0x92,
  0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0xc4, 0x92, 0x04, 0x93, 0x44, 0x8a,
  0x84, 0x81, 0x62, 0x70, 0x23, 0x70, 0x43, 0x70, 0x02, 0x68, 0x03, 0x79,
  0xe3, 0x81, 0xa4, 0x82, 0x44, 0x7a, 0x23, 0x69, 0x23, 0x6a, 0xc2, 0x59,
  0x82, 0x49, 0xc2, 0x59, 0xe2, 0x61, 0x43, 0x72, 0x23, 0x6a, 0x64, 0x82,
  0x84, 0x82, 0x84, 0x82, 0xe2, 0x28, 0xa4, 0x8a, 0xa4, 0x8a, 0xe4, 0x92,
  0x64, 0x7a, 0x83, 0x51, 0x61, 0x10, 0x02, 0x31, 0xa3, 0x51, 0xa4, 0x8a,
  0xe4, 0x92, 0xe4, 0x9a, 0xe4, 0x92, 0xa3, 0x59, 0x05, 0x9b, 0xe4, 0x9a,
  0x05, 0x9b, 0xc3, 0x59, 0x05, 0x9b, 0x04, 0x9b, 0x04, 0x9b, 0x05, 0x9b,
  0x04, 0x9b, 0x04, 0x9b, 0x05, 0x9b, 0xe5, 0x9a, 0x05, 0x9b, 0xe4, 0x9a,
  0xe5, 0x9a, 0xe5, 0x92, 0xe5, 0x92, 0x23, 0x72, 0xe4, 0x92, 0xe5, 0x92,
  0xe4, 0x92, 0xe4, 0x92, 0xc4, 0x92, 0xe4, 0x92, 0xe4, 0x92, 0x84, 0x92,
  0xa3, 0x81, 0x23, 0x81, 0x62, 0x70, 0x43, 0x70, 0x62, 0x70, 0x23, 0x70,
  0xc3, 0x70, 0xa3, 0x81, 0x84, 0x8a, 0xa4, 0x8a, 0x84, 0x82, 0x84, 0x82,
  0x64, 0x82, 0x23, 0x7a, 0x83, 0x69, 0xa2, 0x51, 0x82, 0x49, 0xa2, 0x59,
  0xa2, 0x51, 0xe3, 0x61, 0x84, 0x82, 0x03, 0x62, 0x84, 0x82, 0xa3, 0x51,
  0xa4, 0x82, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0x64, 0x7a,
  0x83, 0x51, 0xe4, 0x92, 0x83, 0x51, 0x42, 0x41, 0xc1, 0x28, 0xa1, 0x20,
  0x42, 0x41, 0xe3, 0x61, 0x44, 0x7a, 0xc4, 0x92, 0x25, 0xa3, 0xe4, 0x9a,
  0xe4, 0x9a, 0xe4, 0x92, 0xe4, 0x92, 0xa4, 0x8a, 0x24, 0x6a, 0xe4, 0x92,
  0xe4, 0x92, 0xe4, 0x92, 0xe3, 0x61, 0xe4, 0x92, 0xc4, 0x92, 0xc4, 0x92,
  0xe4, 0x92, 0xc4, 0x8a, 0x04, 0x93, 0xa4, 0x92, 0x44, 0x8a, 0xe4, 0x81,
  0x63, 0x81, 0xc3, 0x78, 0x62, 0x70, 0x43, 0x70, 0x43, 0x70, 0x43, 0x70,
  0x62, 0x70, 0x03, 0x79, 0xc3, 0x70, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x43, 0x69,
  0x22, 0x61, 0xe2, 0x48, 0x82, 0x49, 0x02, 0x39, 0x41, 0x08, 0x62, 0x41,
  0x22, 0x39, 0xc3, 0x59, 0xa1, 0x20, 0xa4, 0x8a, 0x84, 0x82, 0x84, 0x82,
  0xa4, 0x82, 0xa4, 0x8a, 0xc4, 0x8a, 0x22, 0x31, 0xc4, 0x92, 0x23, 0x6a,
  0xa4, 0x82, 0xc4, 0x8a, 0x42, 0x41, 0x05, 0x9b, 0x84, 0x82, 0x82, 0x51,
  0x83, 0x49, 0x42, 0x41, 0xc1, 0x20, 0x81, 0x18, 0xe2, 0x28, 0x42, 0x39,
  0x62, 0x41, 0x83, 0x49, 0x83, 0x49, 0x83, 0x51, 0x83, 0x51, 0x83, 0x51,
  0x83, 0x51, 0x62, 0x49, 0x62, 0x41, 0x42, 0x39, 0xe2, 0x28, 0x81, 0x20,
  0x22, 0x70, 0x23, 0x70, 0x62, 0x70, 0x63, 0x70, 0x63, 0x70, 0x42, 0x70,
  0x62, 0x70, 0x23, 0x79, 0xa3, 0x81, 0x24, 0x82, 0xc4, 0x8a, 0xa4, 0x8a,
  0x64, 0x82, 0x83, 0x79, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x22, 0x69, 0xc2, 0x58, 0xe2, 0x48,
  0x62, 0x49, 0x22, 0x39, 0x22, 0x39, 0xe1, 0x20, 0x02, 0x39, 0xe3, 0x59,
  0xa1, 0x20, 0x84, 0x82, 0x03, 0x62, 0x44, 0x72, 0x62, 0x49, 0x84, 0x82,
  0x64, 0x7a, 0x64, 0x7a, 0xc4, 0x8a, 0xe3, 0x61, 0xa4, 0x8a, 0xa4, 0x82,
  0x82, 0x49, 0xa4, 0x8a, 0xa4, 0x8a, 0x03, 0x6a, 0xc4, 0x92, 0xa4, 0x8a,
  0xe4, 0x92, 0x62, 0x49, 0xe5, 0x92, 0xc4, 0x8a, 0xa4, 0x8a, 0x42, 0x41,
  0x64, 0x82, 0x44, 0x7a, 0x44, 0x7a, 0x84, 0x82, 0x62, 0x41, 0x64, 0x82,
  0x44, 0x72, 0x84, 0x82, 0xc5, 0x92, 0xe3, 0x59, 0x83, 0x68, 0xc3, 0x78,
  0x03, 0x79, 0x83, 0x81, 0x04, 0x82, 0x04, 0x82, 0x04, 0x82, 0xa4, 0x8a,
  0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x44, 0x82, 0x44, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x64, 0x7a, 0x43, 0x72, 0xe3, 0x69, 0xa2, 0x51, 0x82, 0x49, 0xc2, 0x59,
  0xa2, 0x59, 0x23, 0x72, 0x64, 0x7a, 0x03, 0x6a, 0x64, 0x7a, 0x44, 0x7a,
  0x82, 0x41, 0x61, 0x18, 0xc4, 0x92, 0xc2, 0x20, 0xc4, 0x8a, 0x62, 0x41,
  0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xe3, 0x61, 0xc5, 0x8a, 0xa4, 0x8a,
  0xc4, 0x92, 0xe3, 0x61, 0xa4, 0x8a, 0xa4, 0x8a, 0x44, 0x72, 0x03, 0x6a,
  0xa4, 0x8a, 0xa4, 0x8a, 0xc4, 0x92, 0x63, 0x49, 0xa4, 0x8a, 0xa4, 0x8a,
  0xa4, 0x8a, 0xc4, 0x92, 0x63, 0x49, 0xa4, 0x8a, 0xc4, 0x8a, 0x23, 0x6a,
  0xa4, 0x8a, 0xa4, 0x82, 0xa3, 0x51, 0xa4, 0x8a, 0xc4, 0x8a, 0xc3, 0x81,
  0xa4, 0x8a, 0xa4, 0x8a, 0xc4, 0x81, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x82,
  0xa4, 0x82, 0x84, 0x82, 0x84, 0x82, 0x03, 0x7a, 0x82, 0x68, 0x63, 0x71,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x64, 0x82, 0x44, 0x7a, 0x43, 0x72,
  0x03, 0x6a, 0xa2, 0x51, 0x62, 0x49, 0xc2, 0x59, 0xc2, 0x59, 0x03, 0x6a,
  0x64, 0x7a, 0x64, 0x7a, 0x64, 0x7a, 0xe3, 0x69, 0x82, 0x49, 0x02, 0x39,
  0x22, 0x39, 0x63, 0x49, 0xa4, 0x8a, 0x82, 0x51, 0xe3, 0x59, 0x84, 0x82,
  0x22, 0x39, 0x42, 0x39, 0x43, 0x72, 0xa4, 0x82, 0x84, 0x82, 0xa4, 0x82,
  0xa4, 0x8a, 0xa4, 0x8a, 0xc3, 0x59, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a,
  0xe4, 0x92, 0x62, 0x49, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xc4, 0x8a,
  0x63, 0x49, 0xc4, 0x8a, 0xc4, 0x8a, 0xe3, 0x61, 0xa4, 0x8a, 0xc4, 0x8a,
  0xa3, 0x51, 0xa4, 0x8a, 0xa4, 0x8a, 0x84, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a,
  0x84, 0x82, 0x04, 0x7a, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x84, 0x82, 0xe3, 0x79, 0xa3, 0x79, 0x62, 0x68, 0xa4, 0x82, 0x84, 0x82,
  0x84, 0x82, 0x64, 0x82, 0x63, 0x7a, 0x43, 0x72, 0xc3, 0x61, 0x82, 0x51,
  0x62, 0x41, 0x42, 0x41, 0x22, 0x39, 0x62, 0x41, 0x44, 0x7a, 0x64, 0x7a,
  0x64, 0x7a, 0x64, 0x7a, 0x84, 0x82, 0x84, 0x82, 0xa4, 0x8a, 0x84, 0x82,
  0xc3, 0x61, 0x02, 0x39, 0xe2, 0x28, 0x84, 0x82, 0x02, 0x31, 0x22, 0x41,
  0xe3, 0x61, 0x84, 0x82, 0xa4, 0x82, 0xa4, 0x82, 0xa4, 0x8a, 0x82, 0x49,
  0x83, 0x49, 0x64, 0x7a, 0xa4, 0x8a, 0xa4, 0x82, 0x84, 0x82, 0x84, 0x82,
  0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x82, 0xa4, 0x8a,
  0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a, 0xa4, 0x82, 0x82, 0x49, 0xe3, 0x61,
  0xa4, 0x8a, 0x84, 0x82, 0xa4, 0x82, 0x84, 0x82, 0x64, 0x82, 0x63, 0x79,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0xa4, 0x82,
  0x64, 0x82, 0x84, 0x82, 0x64, 0x82, 0x64, 0x82, 0x64, 0x82, 0x64, 0x82,
  0x44, 0x7a, 0xc3, 0x69, 0xe2, 0x58, 0x62, 0x49, 0x42, 0x41, 0xa2, 0x51,
  0xa2, 0x59, 0xe3, 0x69, 0x43, 0x72, 0x43, 0x72, 0x64, 0x7a, 0x64, 0x7a,
  0x64, 0x7a, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0xa4, 0x82, 0xc4, 0x8a, 0x22, 0x39, 0xa3, 0x51, 0x24, 0x72, 0xa4, 0x82,
  0xa4, 0x82, 0xa4, 0x82, 0xa4, 0x82, 0x03, 0x6a, 0x22, 0x39, 0xa3, 0x51,
  0xa4, 0x8a, 0x44, 0x72, 0x21, 0x08, 0x42, 0x41, 0x84, 0x82, 0xa4, 0x8a,
  0xa4, 0x8a, 0xa4, 0x8a, 0xe2, 0x28, 0xc4, 0x8a, 0xa4, 0x8a, 0xa4, 0x8a,
  0xa4, 0x8a, 0x42, 0x41, 0x44, 0x7a, 0xe2, 0x28, 0x84, 0x82, 0xa4, 0x82,
  0xa4, 0x82, 0x84, 0x82, 0x04, 0x82, 0xc3, 0x79, 0x84, 0x82, 0x84, 0x82,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x64, 0x7a, 0x43, 0x72, 0x83, 0x69,
  0xc2, 0x50, 0x42, 0x49, 0x42, 0x41, 0xa2, 0x51, 0xc2, 0x59, 0x03, 0x6a,
  0x43, 0x72, 0x43, 0x72, 0x43, 0x72, 0x44, 0x7a, 0x64, 0x7a, 0x64, 0x7a,
  0x64, 0x7a, 0x64, 0x7a, 0x64, 0x7a, 0x64, 0x7a, 0x84, 0x82, 0x64, 0x82,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0xa4, 0x8a, 0x22, 0x39, 0x02, 0x39, 0xe3, 0x61, 0x84, 0x82, 0x44, 0x72,
  0x82, 0x49, 0x41, 0x10, 0xa4, 0x82, 0xa4, 0x82, 0xa4, 0x82, 0xa4, 0x8a,
  0x61, 0x18, 0xa4, 0x8a, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x24, 0x6a,
  0x02, 0x31, 0xe2, 0x30, 0xa4, 0x8a, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82, 0x84, 0x82,
  0x84, 0x82, 0x84, 0x82, 0x64, 0x7a, 0x84, 0x82, 0x84, 0x82, 0x64, 0x7a,
  0x64, 0x7a, 0x43, 0x7a, 0x44, 0x72, 0x03, 0x6a, 0x82, 0x59, 0x62, 0x49,
  0x22, 0x41, 0x82, 0x49, 0xc2, 0x59, 0xc2, 0x61, 0xe3, 0x61, 0x03, 0x62,
  0xe3, 0x69, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x23, 0x72,
  0x23, 0x72, 0x03, 0x6a, 0x03, 0x6a, 0x23, 0x72, 0xe3, 0x69, 0xe3, 0x69,
  0x03, 0x6a, 0x03, 0x6a, 0x23, 0x72, 0x03, 0x6a, 0x03, 0x6a, 0x23, 0x72,
  0x23, 0x6a, 0x23, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x23, 0x72, 0x03, 0x72,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0xe3, 0x69, 0xe3, 0x61, 0xe2, 0x61,
  0xc2, 0x61, 0xa2, 0x59, 0x62, 0x49, 0x21, 0x39, 0xe2, 0x30, 0x82, 0x49,
  0xa2, 0x51, 0xa2, 0x59, 0xc2, 0x59, 0xc2, 0x61, 0xe2, 0x61, 0xc2, 0x61,
  0xe3, 0x61, 0xe3, 0x61, 0xe2, 0x61, 0xe3, 0x61, 0xe2, 0x61, 0xe3, 0x61,
  0xe2, 0x61, 0xe3, 0x61, 0xe2, 0x69, 0xe2, 0x69, 0x02, 0x6a, 0xe2, 0x61,
  0xe3, 0x69, 0x03, 0x6a, 0x02, 0x6a, 0xe3, 0x69, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a, 0x03, 0x6a,
  0xe3, 0x69, 0xe3, 0x69, 0xe2, 0x69, 0xe3, 0x69, 0xe3, 0x69, 0xe3, 0x61,
  0xe3, 0x69, 0xe2, 0x69, 0xe2, 0x61, 0xc3, 0x61, 0xe3, 0x61, 0xe3, 0x61,
  0xe3, 0x61, 0xc3, 0x61, 0xc3, 0x61, 0xc2, 0x59, 0xa2, 0x59, 0x82, 0x51,
  0x41, 0x41, 0x01, 0x39, 0xe1, 0x28, 0x62, 0x49, 0x82, 0x49, 0x82, 0x51,
  0xa2, 0x51, 0xa2, 0x51, 0xa2, 0x59, 0xa2, 0x59, 0xc2, 0x59, 0xa2, 0x59,
  0xa2, 0x59, 0xc2, 0x59, 0xc2, 0x59, 0xc2, 0x59, 0xc2, 0x59, 0xc2, 0x61,
  0xc2, 0x59, 0xc2, 0x61, 0xc2, 0x61, 0xe2, 0x61, 0xc2, 0x59, 0xc2, 0x59,
  0xe2, 0x61, 0xc2, 0x61, 0xe2, 0x61, 0xc2, 0x61, 0xe2, 0x61, 0xe2, 0x61,
  0xe3, 0x61, 0xe2, 0x61, 0xc2, 0x61, 0xe3, 0x61, 0xe3, 0x61, 0xe3, 0x61,
  0xe2, 0x61, 0xe3, 0x61, 0xe2, 0x61, 0xe3, 0x61, 0xe3, 0x61, 0xe2, 0x61,
  0xe3, 0x61, 0xc2, 0x61, 0xe2, 0x61, 0xe2, 0x61, 0xe2, 0x61, 0xc2, 0x61,
  0xc2, 0x61, 0xc2, 0x61, 0xc2, 0x61, 0xc2, 0x61, 0xc2, 0x61, 0xc2, 0x61,
  0xc2, 0x61, 0xc2, 0x61, 0xc2, 0x59, 0xc2, 0x59, 0xa2, 0x59, 0xa2, 0x59,
  0xa2, 0x51, 0x82, 0x51, 0x82, 0x51, 0x62, 0x49, 0x22, 0x39, 0xe1, 0x30,
  0xe1, 0x28, 0x02, 0x31, 0x01, 0x31, 0x01, 0x39, 0x21, 0x39, 0x21, 0x39,
  0x21, 0x39, 0x21, 0x39, 0x22, 0x39, 0x21, 0x39, 0x21, 0x39, 0x42, 0x39,
  0x42, 0x41, 0x42, 0x41, 0x41, 0x41, 0x42, 0x39, 0x42, 0x39, 0x42, 0x41,
  0x41, 0x39, 0x21, 0x39, 0x21, 0x39, 0x42, 0x39, 0x42, 0x41, 0x42, 0x41,
  0x41, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41,
  0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41,
  0x42, 0x41, 0x41, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41,
  0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41,
  0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x62, 0x41, 0x42, 0x41,
  0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41, 0x42, 0x41,
  0x41, 0x41, 0x21, 0x39, 0x01, 0x31, 0xe1, 0x30, 0xc1, 0x28
};
unsigned int vu_img_sm1_bmp_len = 4234;

void setup() {
  tft.begin();

  //START DMA MODE
  tft.refresh();
    
  tft.dfillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello...");
  
}
int t = 0;

void loop() {

  // calculate sine wave
  short signed int v[320];
  for (int i=1; i<320; i++){
    v[i] = (int8_t)(sin((i+t)/20.0) * 100.0);
  }
  
  //tft.stopRefresh();
  uint16_t *p = (uint16_t*)vu_img_sm1_bmp + 69;
  tft.dwriteRectScale(0,0, 64, 32, 240, 320, p);

  for (int i=1; i<320; i++){
    drawLine(120 + v[i-1], i-1, 120 + v[i], i, ILI9341_BLACK);
  }
  //tft.refresh();
  t++;
}

void drawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int x = x1-x0;
    int y = y1-y0;
    int dx = abs(x), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                                                /* error value e_xy             */
    for (;;){                                                           /* loop                         */
        tft.ddrawPixel(x0,y0,color);
        e2 = 2*err;
        if (e2 >= dy) {                                                 /* e_xy+e_x > 0                 */
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {                                                 /* e_xy+e_y < 0                 */
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}
