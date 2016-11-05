/*  Convert video for SD card playing, with
      Teensy 3.6 & ILI9341_t3DMA library

    Original : Copyright (c) 2014 Paul Stoffregen, PJRC.COM, LLC

    Updated for use with ILI9341_t3DMA: (c) 2016 Frank Bösing
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

// To configure this program, edit the following:
//
//  1: Change myMovie to open a video file of your choice    ;-)
//     Also change the output file name.
//
//  2: Edit ledWidth, ledHeight, ledLayout for your LEDs
//
//  3: Edit framerate.  This configures the speed VideoSDcard
//     will play your video data.  It's also critical for merging
//     audio to be played by Teensy 3.1's digital to analog output.
//

// To create the Audiofile, install ffmpeg and use the following commandline:
// ffmpeg -i "BigBuckBunny_512kb.mp4" -f s16le -ar 44100 -acodec pcm_s16le output.raw

import processing.video.*;
import processing.serial.*;
import java.io.*;

//int ledWidth =  60;          // size of LED panel
//int ledHeight = 32;
int ledWidth =  320;          // size of LED panel
int ledHeight = 240;
boolean ledLayout = true;    // layout of rows, true = even is left->right

double framerate = 23.98;    // You MUST set this to the movie's frame rate
                             // Processing does not seem to have a way to detect it.

float gamma = 1.8;
PImage ledImage;
long elapsed_picoseconds=0L;
long elapsed_microseconds=0L;
long picoseconds_per_frame = (long)(1e12 / framerate + 0.5);
boolean fileopen=true;
int audioidx=0;
FileOutputStream myFile;

//Movie myMovie = new Movie(this, "/Users/paul/myvideo.mov");
Movie myMovie = new Movie(this, "/Users/Frank/Documents/Processing/ILI9341movie2sdcard_pde/BigBuckBunny_512kb.mp4"); 

byte[] audiobytes;
byte[] fileHeader =  new byte[512];

void setup() {
  
  try {
    audiobytes = loadBytes("/Users/Frank/Documents/Processing/ILI9341movie2sdcard_pde/BigBuckBunny_512kb.raw");
  } catch (Exception e) {
    exit();
  }
  
  try {
    myFile = new FileOutputStream("/Users/Frank/Documents/Processing/ILI9341movie2sdcard_pde/myvideo.bin");
    
  } catch (Exception e) {
    exit();
  }
  
  //double dfps = 1000000.0 / framerate;
  //int ifps = round(float(dfps));
  
  fileHeader[0] = 0;
  try {
    myFile.write(fileHeader);
  } catch (Exception e) {    
    exit();
  }
  
  ledImage = createImage(ledWidth, ledHeight, RGB);
  size(720, 560);  // create the window
  myMovie.play();  // start the movie :-)
}
 
// movieEvent runs for each new frame of movie data
void movieEvent(Movie m) {
  // read the movie's next frame
  m.read();

  elapsed_picoseconds += picoseconds_per_frame;
  int usec = (int)((elapsed_picoseconds / 1000000L) - elapsed_microseconds);
  elapsed_microseconds += (long)usec;
  println("usec = " + usec);
  
  
  // copy the movie's image to the LED image
  ledImage.copy(m, 0, 0, m.width, m.height, 0, 0, ledWidth, ledHeight);
  // convert the LED image to raw data
  //byte[] ledData =  new byte[(ledWidth * ledHeight * 3) + 5];
  //byte[] ledData =  new byte[ledWidth * ledHeight * 2];
  byte[] ledData =  new byte[5*32768];
  image2data(ledImage, ledData, ledLayout);
/*  
  ledData[0] = '*';  // first Teensy is the frame sync master
  
  ledData[1] = (byte)(ledWidth * ledHeight);
  ledData[2] = (byte)((ledWidth * ledHeight) >> 8);
  ledData[3] = (byte)(usec);   // request the frame sync pulse
  ledData[4] = (byte)(usec >> 8); // at 75% of the frame time
*/  
  // send the raw data to the LEDs  :-)
  //ledSerial[i].write(ledData); 
  try {
    myFile.write(ledData);
  } catch (Exception e) {    
    exit();
  }
}

// image2data converts an image to OctoWS2811's raw data format.
// The number of vertical pixels in the image must be a multiple
// of 8.  The data array must be the proper size for the image.
void image2data(PImage image, byte[] data, boolean layout) {
  //int offset = 5;
  int offset = 0;
  int x, y, xbegin, xend, xinc, mask;
  //int linesPerPin = image.height / 8;
  //int pixel[] = new int[8];
  int pixel;
  
  for (y = 0; y < image.height; y++) {
    xbegin = 0;
    xend = image.width;
  
    for (x = xbegin; x != xend; x += 1) {
      
      pixel = image.pixels[x + y  * image.width];
      pixel = colorWiring(pixel);
      
      data[offset++] =byte(pixel & 0xff);
      data[offset++] =byte((pixel >> 8) & 0xff);
    
    }
    
  } 
  
  //add audio data
  //(41701*44117/1000000) * 2 * 2
  int a;
  for (a = 0; a < 1839; a++) {
    //left channel       
    data[offset++] = audiobytes[audioidx++];
    //print(audiobytes[audioidx]);
    data[offset++] = audiobytes[audioidx++];
    //print(audiobytes[audioidx]);
    //right channel
    data[offset++] = audiobytes[audioidx++];
    data[offset++] = audiobytes[audioidx++];
  }
  
}

// translate the 24 bit color from RGB to the actual
// order used by the LED wiring.  GRB is the most common.
int colorWiring(int c) {
  int red =   (c & 0xFF0000) >> 16;
  int green = (c & 0x00FF00) >> 8;
  int blue =  (c & 0x0000FF);
/*  
  red = gammatable[red];
  green = gammatable[green];
  blue = gammatable[blue];
  return (green << 16) | (red << 8) | (blue); // GRB - most common wiring
*/
  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); 
}

// draw runs every time the screen is redrawn - show the movie...
void draw() {
  if (myMovie.time() < myMovie.duration()) {
    image(myMovie, 0, 80);
    image(ledImage, 240 - ledWidth / 2, 10);
  } else {
    if (fileopen) {
      println("movie stop, closing output file");
      try {
        myFile.close();
      } catch (Exception e) {    
        exit();
      }
      fileopen = false;
    }
  }
}

// respond to mouse clicks as pause/play
boolean isPlaying = true;
void mousePressed() {
  if (isPlaying) {
    myMovie.pause();
    isPlaying = false;
  } else {
    myMovie.play();
    isPlaying = true;
  }
}