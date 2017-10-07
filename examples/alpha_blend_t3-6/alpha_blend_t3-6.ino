/*
* Small alpha blend example
* By Ramon Schepers
* What does this example do:
* Fades the screen between black and white using a fullscreen alpha blend.
 */
#include <SPI.h>
#include <DMAChannel.h>
#include <ILI9341_t3.h>
#include <ILI9341_t3DMA.h>
#include "gpu_commands.h"

GPU gpu = GPU(); //create the gpu instance

void setup() {
  gpu.init_screen();
  Serial.begin(38400);
  gpu.fillScreen(0x0000);
  //make a dutch flag :)
  gpu.fillRect(0,0,100,240,0xf800);
  gpu.fillRect(100,0,120,240,0xffff);
  gpu.fillRect(220,0,100,240,0x007f);
  //start the auto refresh
  gpu.start_auto_refresh();
}

void loop() {
  //the main code should make the screen slowly fade from the flag to white with a fullscreen alpha blend.
  uint32_t start = micros();
  gpu.blend_screen(0,0,320,240,0xffff,0.1);
  uint32_t end = micros();
  //show render speed!
  Serial.println("a screenblend took: ");
  Serial.println(end-start);
  Serial.println("microseconds");
  delay(100);
}
