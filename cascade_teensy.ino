/*

   ––––––––––––––––
  |C I R C         |
  |U I T ~ ~ ~ ~ ~ |
  |                |
  |                |
  |                |
  |~ ~ ~ ~ C I R C |
  |            U S |
  ––––––––––––––––
  CASCADE TEENSY
  A CIRCUIT CIRCUS PROJECT

  Available Connections on the OctoWS2811 adapter
  --------------------
    pin 2:  LED Strip #1 ORANGE CAT6 Wire
    pin 14: LED strip #2
    pin 7:  LED strip #3
    pin 8:  LED strip #4
    pin 6:  LED strip #5
    pin 20: LED strip #6
    pin 21: LED strip #7
    pin 5:  LED strip #8
*/
#include <OctoWS2811.h>
const int longestStrip = 84;

DMAMEM int displayMemory[longestStrip * 6];
int drawingMemory[longestStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(longestStrip, displayMemory, drawingMemory, config);

/*  Two dimensional array containing the physical arrangement of the LED.
    The first array contains one array for each circle. In Cascade will be 4.
    The second array contains 3 values: Data Pin, Length of Inner Strip, Length of Outer Strip.
*/

const uint8_t numStrips = 1;
const uint8_t stripLengths[] = {84};
const uint16_t totalLedCount = numStrips * longestStrip;

const uint8_t numSensors = 4;
//uint16_t sensorData[numSensors];
uint16_t sensorData[] = {4, 3, 2, 1};
boolean firstContact = false;

long lastUpdate;
long lastSend;
uint16_t sendInterval = 50;

void setup() {
  delay(1000); // sanity delay
  Serial.setTimeout(1000);
  leds.begin();
  leds.show();
}

//This function can be changed to fit any kind and amount of sensors
void readSensors() {

}

//Reads one incoming byte at a time
//void loop() {
//  readSensors();
//  Serial.write('!');
//  for (int i = 0; i < numSensors; i++) {
//    Serial.write(sensorData[i]);
//  }
//  if (Serial.available() > 0) {
//    int inByte = Serial.read();
//    if (serialCount == 0) {
//      currentDisplay = inByte;
//    } else {
//      ledData[serialCount - 1] = inByte;
//      sensorData[0]++;
//    }
//    serialCount++;
//
//    if (serialCount >= totalLedCount * 3 + 1) {
//      for (int i = 0; i < totalLedCount; i++) {
//        int index = i * 3; //Index adjusted to 3 colors and 1 display ID
//        int r = ledData[index];
//        int g = ledData[index + 1];
//        int b = ledData[index + 2];
//        leds[i] = CRGB(r, g, b);
//
//        Serial.write('#'); //Request new led data
//        FastLED.show();
//        serialCount = 0;
//        sensorData[0] = ledData[0];
//      }
//    }
//    lastUpdate = millis();
//  }
//  else {
//    Serial.write('#');
//    if (millis() - lastUpdate > 1000) { //If the serial connection is dead. Reset everything.
//      serialCount = 0;
//      for (uint8_t i = 0; i < totalLedCount; i++) {
//        leds[i] = CRGB::Black;
//      }
//      FastLED.show();
//    }
//  }
//}

//Reads all incoming bytes at once
void loop() {
  int startChar = Serial.read();

  if (startChar = '!') {
    int serialCount = Serial.readBytes((char *)drawingMemory, sizeof(drawingMemory));
    //byte ledData[totalLedCount * 3];
    //int serialCount = Serial.readBytes((char *)ledData, sizeof(ledData));

    sensorData[0] = 0;
    sensorData[1] = drawingMemory[5];
    sensorData[2] = drawingMemory[6];
    sensorData[3] = drawingMemory[7];

//    if (serialCount == sizeof(ledData)) {
//      for (int i = 0; i < totalLedCount; i++) {
//        int index = i * 3; //Index adjusted to 3 colors and 1 display ID
//        int r = ledData[index];
//        int g = ledData[index + 1];
//        int b = ledData[index + 2];
//        leds.setPixel(i, r, g, b);
//      }
//      sensorData[0] = ledData[0];
//      //Serial.write('#'); //Request new led data
//      leds.show();
//      lastUpdate = millis();
//    }

        if (serialCount == sizeof(drawingMemory)) {
          sensorData[0] = serialCount;
          //Serial.write('#'); //Request new led data
          leds.show();
          //serialCount = 0;
          lastUpdate = millis();
        }
  }

  if (millis() - lastSend > sendInterval) {
    readSensors();
    for (int i = 0; i < numSensors; i++) {
      Serial.write(sensorData[i]);
    }
    Serial.write('#');
  }

  if (millis() - lastUpdate > 1000) { //If the serial connection is dead. Reset everything.
    for (uint8_t i = 0; i < totalLedCount; i++) {
      leds.setPixel(i, 255, 0, 0);
    }
    leds.show();
  }

}

