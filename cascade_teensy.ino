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
#include <FastLED.h>
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  200
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
int serialCount;
boolean isReady = true;
DMAMEM int ledData[TOTAL_LED_COUNT * 3 + numStrips];


void setup() {
  delay(1000); // sanity delay
  FastLED.addLeds<CHIPSET, 2, COLOR_ORDER>(leds, TOTAL_LED_COUNT).setCorrection( TypicalLEDStrip ); //The LED Pin is currently hard coded

  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(2000000);

  for (uint8_t i = 0; i < numStrips; i++) {
    for (uint8_t j = 0; i < stripLengths[i]; i++) {
      leds[i][j] = CRGB::Black;
    }
  }
  FastLED.show();

  //establishContact();  // send a byte to establish contact until Processing responds

}

//void establishContact() {
//  while (Serial.available() <= 0) {
//    Serial.write('A');   // send a capital A
//    delay(300);
//  }
//}

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
//  }
//  else {
//    Serial.write('#');
//
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

        Serial.write('#'); //Request new led data
        FastLED.show();
        sensorData[0] = serialCount;
        serialCount = 0;
        
      }
    }
  }
  else {
    Serial.write('#');

  }
}



//Reads all incoming bytes at once
//void loop() {
//  if (Serial.available() > 0) {
//    Serial.write('A');
//    firstcontact
//    int startChar = Serial.read();
//    if (startChar == 'A') {
//      for (int i = 0; i < numSensors; i++) {
//        readSensors();
//        Serial.write(sensorData[i]);
//      }
//    }
//    if (startChar == 'S') {
//        byte ledData[TOTAL_LED_COUNT * 3 + 1];
//        int count = Serial.readBytes(ledData, TOTAL_LED_COUNT * 3 + 1 + 1); //Number of leds * 3 colors + 1 display ID + 1 termination character
//        if (count == TOTAL_LED_COUNT * 3 + 1 + 1) {
//          currentDisplay = ledData[0];
//          for (int i = 0; i < TOTAL_LED_COUNT; i++) {
//            int index = i * 3 + 1; //Index adjusted to 3 colors and 1 display ID
//            int r = ledData[index];
//            int g = ledData[index + 1];
//            int b = ledData[index + 2];
//            leds[i] = CRGB(r, g, b);
//
//            FastLED.show();
//            //Serial.write('A'); //Triggers Processings serialEvent
//          }
//        }
//      }
//
//  }
//}

//void serialSend() {
//
//  //Send sensor data back
//  if (i == TOTAL_LED_COUNT - 1 ) {
//    sensorData[0] = r;
//    sensorData[1] = g;
//    sensorData[2] = b;
//    sensorData[3] = count;
//  }
//}

//// Read information from serial connection
//void serialEvent() {
//  int inByte = Serial.read();
//
//  // Establish contact
//  if (firstContact == false) {
//    if (inByte == 'A') {
//      Serial.flush();
//      firstContact = true;
//      Serial.write('A');
//    }
//    // If contact is already established store the data the arrays
//  } else {
//    currentDisplay = ledData[0];
//    //for (int i = 0; i < TOTAL_LED_COUNT; i++)
//    int index = serialCount * 3 + 1; //Index adjusted to 3 colors and 1 display ID
//    int r = ledData[index];
//    int g = ledData[index + 1];
//    int b = ledData[index + 2];
//    leds[serialCount] = CRGB(r, g, b);
//    serialCount++;
//
//    // Send an A when all data has been received.
//    if (serialCount > TOTAL_LED_COUNT * 3 + 1) {
//      Serial.write('A');
//      serialCount = 0;
//      FastLED.show();
//    }
//
//  }
//}

