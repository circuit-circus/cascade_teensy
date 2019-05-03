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
    pin 14: LED strip #2 BLUE CAT6 Wire
    pin 7:  LED strip #3 GREEN CAT6 Wire
    pin 8:  LED strip #4 BROWN CAT6 Wire
    pin 6:  LED strip #5
    pin 20: LED strip #6
    pin 21: LED strip #7
    pin 5:  LED strip #8
*/

#include <OctoWS2811.h>
const int longestStrip = 170;

DMAMEM int displayMemory[longestStrip * 6];
int drawingMemory[longestStrip * 6];

const int config = WS2811_800kHz;

OctoWS2811 leds(longestStrip, displayMemory, drawingMemory, config);

const uint8_t numSensors = 4;
uint8_t dataToSend[numSensors + 1];

long lastUpdate;
long lastSend;
uint16_t sendInterval = 50;

//Sensors
uint16_t sensorData[numSensors] = {4, 3, 2, 1};
const uint8_t touchPins[] = {22,23,18,19};

void setup() {
  delay(1000); // sanity delay
  Serial.setTimeout(1000);
  leds.begin();
  leds.show();
  dataToSend[numSensors] = '#'; //Adding a buffer character to the data that will be sent. This triggers the 
}

//This function can be changed to fit any kind and amount of sensors
void readSensors() {
  //Read cap sensors
  for (int i = 0; i < numSensors; i++){
    sensorData[i] = touchRead(touchPins[i]);  
    }
}

//Reads all incoming bytes at once
void loop() {
  int startChar = Serial.read();

  //The data transmission from the graphic computer should start with a ! character
  if (startChar == '!') {
    int serialCount = Serial.readBytes((char *)drawingMemory, sizeof(drawingMemory));

    //When all data has been received the leds are updated 
    if (serialCount == sizeof(drawingMemory)) {
      leds.show();
      lastUpdate = millis();
    }
  }

  //The Teensy sends sensors data at a fixed interval
  if (millis() - lastSend > sendInterval) {
    readSensors();
    for (int i = 0; i < numSensors; i++){
    dataToSend[i] = min(255,map(sensorData[i], 0, 1023, 0, 255));  
    }
    
    Serial.write((uint8_t*) dataToSend, sizeof(dataToSend));
    lastSend = millis();
  }

  //If the serial connection is dead, show a color so that we can tell.
  if (millis() - lastUpdate > 1000) { 
    for (int i = 0; i < leds.numPixels(); i++) {
      leds.setPixel(i, 0, 255, 0);
    }
    leds.show();
  }

}

