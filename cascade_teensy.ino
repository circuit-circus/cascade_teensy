/*  OctoWS2811 VideoDisplay.ino - Video on LEDs, from a PC, Mac, Raspberry Pi
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

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

  Update: The movie2serial program which transmit data has moved to "extras"
  https://github.com/PaulStoffregen/OctoWS2811/tree/master/extras

  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 to 220 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4:  Do not use
    pin 3:  Do not use as PWM.  Normal use is ok.
    pin 12: Frame Sync

    When using more than 1 Teensy to display a video image, connect
    the Frame Sync signal between every board.  All boards will
    synchronize their WS2811 update using this signal.

    Beware of image distortion from long LED strip lengths.  During
    the WS2811 update, the LEDs update in sequence, not all at the
    same instant!  The first pixel updates after 30 microseconds,
    the second pixel after 60 us, and so on.  A strip of 120 LEDs
    updates in 3.6 ms, which is 10.8% of a 30 Hz video frame time.
    Doubling the strip length to 240 LEDs increases the lag to 21.6%
    of a video frame.  For best results, use shorter length strips.
    Multiple boards linked by the frame sync signal provides superior
    video timing accuracy.

    A Multi-TT USB hub should be used if 2 or more Teensy boards
    are connected.  The Multi-TT feature allows proper USB bandwidth
    allocation.  Single-TT hubs, or direct connection to multiple
    ports on the same motherboard, may give poor performance.
*/

#include <OctoWS2811.h>

/*  Two dimensional array containing the physical arrangement of the LED.
    The first array contains one array for each circle. In Cascade will be 4.
    The second array contains 3 values: Data Pin, Length of Inner Strip, Length of Outer Strip.
*/
const int ledsConfig[1][3]
{
  {2, 4, 17}
};

const int numTotalLeds = 21; //Number of LEDS in total. This is used to allocate memory

DMAMEM int displayMemory[numTotalLeds * 6];
int drawingMemory[numTotalLeds * 6];

const int config = WS2811_800kHz; // color config is on the PC side

OctoWS2811 leds(numTotalLeds, displayMemory, drawingMemory, config);

int currentCircle; // Used to keep track of which circle we are receiving data for
const int numSensors = 4;
int touchData[numSensors];

void setup() {
  Serial.begin(2000000);
  establishContact();  // send a byte to establish contact until Processing responds
  leds.begin();
  leds.show();

}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.write('A');   // send a capital A
    delay(300);
  }
}

void readSensors() {
  //Read Touch sensors
}

void loop() {
  if (Serial.available() > 0) {
    // get incoming byte:
    int startChar = Serial.read();
    if (startChar == 'A') {
      int count = 0;
      count = Serial.readBytes((char *)drawingMemory, sizeof(drawingMemory));
      if (count == sizeof(drawingMemory)) {
        leds.show(); //When all color data has been received
        readSensors();
        for (int i = 0; i < numSensors; i++){
        Serial.write(touchData[i]);
        }
      }
    } else if (startChar >= 0) {
      // discard unknown characters
    }
  }
}

