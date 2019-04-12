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

#include <FastLED.h>
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60
#define TOTAL_LED_COUNT 21 //You have to manually add the total LED count here

/*  Two dimensional array containing the physical arrangement of the LED.
    The first array contains one array for each circle. In Cascade will be 4.
    The second array contains 3 values: Data Pin, Length of Inner Strip, Length of Outer Strip.
*/

const uint8_t numStrips = 1;
const uint8_t stripLengths[] = {21};

CRGB leds[TOTAL_LED_COUNT];

uint8_t currentDisplay; // Used to keep track of which circle we are receiving data for
const uint8_t numSensors = 4;
//uint16_t sensorData[numSensors];
uint16_t sensorData[] = {4, 3, 2, 1};


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

  establishContact();  // send a byte to establish contact until Processing responds

}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.write('A');   // send a capital A
    delay(300);
  }
}

//This function can be changed to fit any kind and amount of sensors
void readSensors() {

}

void loop() {
  if (Serial.available() > 0) {
    for (int i = 0; i < numSensors; i++) {
      readSensors();
      Serial.write(sensorData[i]);
    }
    
    // get incoming byte:
    int startChar = Serial.read();
    if (startChar == 'S') {
      byte ledData[TOTAL_LED_COUNT * 3 + 1];
      int count = Serial.readBytesUntil('!', ledData, TOTAL_LED_COUNT * 3 + 1 + 1); //Number of leds * 3 colors + 1 display ID + 1 termination character
      if (count == TOTAL_LED_COUNT * 3 + 1 + 1) {
        currentDisplay = ledData[0];
        for (int i = 0; i < TOTAL_LED_COUNT; i++) {
          int index = i * 3 + 1; //Index adjusted to 3 colors and 1 display ID
          int r = ledData[index];
          int g = ledData[index + 1];
          int b = ledData[index + 2];
          leds[i] = CRGB(r, g, b);

          //Send sensor data back
          if (i == TOTAL_LED_COUNT - 1 ) {
            sensorData[0] = r;
            sensorData[1] = g;
            sensorData[2] = b;
            sensorData[3] = count;
          }
          FastLED.show();


 
         //Serial.write('A'); //Triggers Processings serialEvent
        }
      }
    }

  }
}

