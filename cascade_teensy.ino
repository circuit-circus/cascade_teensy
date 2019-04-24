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

const int config = WS2811_800kHz;

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

uint8_t dataToSend[numSensors + 1];

boolean firstContact = false;

long lastUpdate;
long lastSend;
uint16_t sendInterval = 50;

//Touch sensors
uint16_t sensorData[numSensors] = {4, 3, 2, 1};
const uint8_t touchPins[] = {22,23,18,19};

void setup() {
  delay(1000); // sanity delay
  Serial.setTimeout(1000);
  leds.begin();
  leds.show();
  dataToSend[numSensors] = '#'; //Adding a buffer character to the data that will be sent.
}

//This function can be changed to fit any kind and amount of sensors
void readSensors() {
  //Read cap sensors
  sensorData[0] = touchRead(touchPins[0]);
}

//Reads all incoming bytes at once
void loop() {
  int startChar = Serial.read();

  if (startChar == '!') {
    int serialCount = Serial.readBytes((char *)drawingMemory, sizeof(drawingMemory));

    if (serialCount == sizeof(drawingMemory)) {
      leds.show();
      lastUpdate = millis();
    }
  }

  if (millis() - lastSend > sendInterval) {
    readSensors();
    for (int i = 0; i < numSensors; i++){
    dataToSend[i] = min(255,map(sensorData[i], 0, 10000, 0, 255));  
    }
    
    Serial.write((uint8_t*) dataToSend, sizeof(dataToSend));
    lastSend = millis();
  }

  if (millis() - lastUpdate > 1000) { //If the serial connection is dead. Reset everything.
    for (uint8_t i = 0; i < totalLedCount; i++) {
      leds.setPixel(i, 0, 255, 0);
    }
    leds.show();
  }

}

