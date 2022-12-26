#include <FastLED.h>
#include "Button.h"
#define NUM_LEDS 150
#define LED_PIN 5
#define BUTTON_PIN 4

Button button(BUTTON_PIN);

CRGB leds[NUM_LEDS];

int ledMode = 0;

int loopCountInMode = 0;

int ledIndex = 0;
int colorIndex = 0;
int tailLength = 24;

int numColors = 10;
int colors[16] = {CRGB::Red, CRGB::Brown, CRGB::Yellow, CRGB::Blue, CRGB::DarkBlue, CRGB::Amethyst, CRGB::Purple, CRGB::Green, CRGB::Orange, CRGB::White};

unsigned long twinkleArr[NUM_LEDS];
int twinklePhase[NUM_LEDS];
int fadeDelay[4] = {10, 15, 20, 25};

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2811, LED_PIN>(leds, NUM_LEDS);

  // Initialize the twinkle arrays
  initializeTwinkle();
  
  // Initialize the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);  
}

void loop() {
  if (button.update()) {
    ledMode++;
    loopCountInMode = 0;
    if (ledMode >= 4) {
      ledMode = 0;
    }
  }
  
  switch (ledMode) {
    case 0:
      if (loopCountInMode == 0) {
        ledModeWhite();        
      }
      break;
    case 1:
      if (loopCountInMode == 0) {
        ledModeRead();
      }
      break;
    case 2:
      if (loopCountInMode == 0) {
        initializeTwinkle();
        turnOffAllLights();
      }
      ledModeTwinkle();
      break;
    default:
      if (loopCountInMode == 0) {
        turnOffAllLights();
      }
      ledModeChase();
      break;
  }

  // Don't really care about loopCountInMode when it gets big, and we want to avoid
  // it maxing out int
  if (loopCountInMode < 100) {
    loopCountInMode++;    
  }
}

void ledModeWhite() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i%3 == 0) {
      leds[i] = CRGB::White;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
  delay(10);
}

void ledModeRead() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i>100 && i < 140) {
      leds[i] = CRGB::Red;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
  delay(10);
}

void ledModeTwinkle() {
  unsigned long now = millis();
  
  // Determine how many LEDs are currently twinkling
  int twinklingLightCount = countTwinklingLights();


  // Add another twinking light if necessary
  if (twinklingLightCount < 60) {
    int nextTwinkleIdx = getNextTwinkleIdx();
    leds[nextTwinkleIdx] = colors[random(numColors)];
    twinkleArr[nextTwinkleIdx] = now + 500;
    twinklePhase[nextTwinkleIdx] = 1;
  }

  FastLED.show();

  // Fade and/or turn off the LEDs as necessary
  for (int i = 0; i < NUM_LEDS; i++) {
    if (twinkleArr[i] > 0 && twinkleArr[i] < now) {
      if (twinklePhase[i] < 50) {
        // fade the LED
        leds[i].nscale8_video(222);
        twinklePhase[i] = twinklePhase[i] + 1;
        twinkleArr[i] = now + fadeDelay[random(3)];
      } else {
        // done fading.  Turn off the LED and reset twinkleArr & twinklePhase
        leds[i] = CRGB::Black;
        twinkleArr[i] = 0;
        twinklePhase[i] = 0;
      }
    }
  }
  
  FastLED.show();
  delay(1);
}

int countTwinklingLights() {
  int count = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (twinklePhase[i] > 0) {
      count++;
    }
  }
  return count;
}

int getNextTwinkleIdx() {
  int nextTwinkleIdx = random(NUM_LEDS);
  for(int i = nextTwinkleIdx; i < NUM_LEDS; i++) {
    if (twinkleArr[nextTwinkleIdx] == 0) {
      break;
    } else {
      nextTwinkleIdx++;
      if (nextTwinkleIdx >= NUM_LEDS) {
        nextTwinkleIdx = 0;
      }
    }
  }
  return nextTwinkleIdx;
}

void ledModeChase() {
  for (int i = 0; i < tailLength; i++) {
    int tailIndex = ledIndex - i;
    if (tailIndex < 0) {
      tailIndex = NUM_LEDS + tailIndex;
    }
    if (i > 8) {
      leds[tailIndex].nscale8_video(192);
    }
  }

  int offIndex = ledIndex - (tailLength + 1);
  if (offIndex < 0) {
    offIndex = NUM_LEDS + offIndex;
  }
  leds[offIndex] = CRGB::Black;


  leds[ledIndex] = colors[colorIndex];

  FastLED.show();

  ledIndex = ledIndex + 1;
  if (ledIndex > NUM_LEDS - 1) {
    ledIndex = 0;
    colorIndex = colorIndex + 1;
    if (colorIndex >= numColors) {
      colorIndex = 0;
    }
  }

  delay(50);
}

void initializeTwinkle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    twinkleArr[i] = 0;
    twinklePhase[i] = 0;
  }
}

void turnOffAllLights() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}
