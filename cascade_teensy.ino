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
#define FASTLED_ALLOW_INTERRUPTS 0 //If the amount of LEDs is above approx 75 the LEDs start flickering when interrupts are on.
#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>

#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  255
//#define FRAMES_PER_SECOND 60
#define TOTAL_LED_COUNT 84 //You have to manually add the total LED count here

/*  Two dimensional array containing the physical arrangement of the LED.
    The first array contains one array for each circle. In Cascade will be 4.
    The second array contains 3 values: Data Pin, Length of Inner Strip, Length of Outer Strip.
*/

const uint8_t numStrips = 1;
const uint8_t stripLengths[] = {TOTAL_LED_COUNT};

CRGB leds[TOTAL_LED_COUNT];

uint8_t currentDisplay; // Used to keep track of which circle we are receiving data for
const uint8_t numSensors = 4;
//uint16_t sensorData[numSensors];
uint16_t sensorData[] = {4, 3, 2, 1};
boolean firstContact = false;
byte serialCount;
boolean isReady = true;
DMAMEM int ledData[TOTAL_LED_COUNT * 3 + numStrips];

long lastUpdate;

void setup() {
  delay(1000); // sanity delay
  LEDS.addLeds<CHIPSET, 5, COLOR_ORDER>(leds, TOTAL_LED_COUNT).setCorrection( TypicalLEDStrip ); //The LED Pin is currently hard coded
  //LEDS.addLeds<WS2812SERIAL,5,GRB>(leds,TOTAL_LED_COUNT);
  LEDS.setBrightness( BRIGHTNESS );
  Serial.begin(2000000);

  for (uint8_t i = 0; i < TOTAL_LED_COUNT; i++) {
    leds[i] = CRGB::Black;
  }
  LEDS.show();
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
//    if (serialCount >= TOTAL_LED_COUNT * 3 + 1) {
//      for (int i = 0; i < TOTAL_LED_COUNT; i++) {
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
//      for (uint8_t i = 0; i < TOTAL_LED_COUNT; i++) {
//        leds[i] = CRGB::Black;
//      }
//      FastLED.show();
//    }
//  }
//}

//Reads all incoming bytes at once
void loop() {
  readSensors();
  Serial.write('!');
  for (int i = 0; i < numSensors; i++) {
    Serial.write(sensorData[i]);
  }
  if (Serial.available() > 0) {
    serialCount = Serial.readBytes((char *)ledData, TOTAL_LED_COUNT * 3 + numStrips); //Number of leds * 3 colors + 1 display ID
    if (serialCount >= TOTAL_LED_COUNT * 3 + 1) {
      for (int i = 0; i < TOTAL_LED_COUNT; i++) {
        int index = i * 3; //Index adjusted to 3 colors and 1 display ID
        int r = ledData[index];
        int g = ledData[index + 1];
        int b = ledData[index + 2];
        leds[i] = CRGB(r, g, b);
      }
      Serial.write('#'); //Request new led data
      LEDS.show();
      sensorData[0] = serialCount;
      serialCount = 0;
      lastUpdate = millis();
    }
  }
  else {
    delay(500);
    Serial.write('&');
    if (millis() - lastUpdate > 1000) { //If the serial connection is dead. Reset everything.
      serialCount = 0;
      for (uint8_t i = 0; i < TOTAL_LED_COUNT; i++) {
        leds[i] = CRGB::Black;
      }
      LEDS.show();
    }
  }
}

