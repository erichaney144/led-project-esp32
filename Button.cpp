#include "Button.h"

Button::Button(byte pin) {
  this->pin = pin;
  lastReading = HIGH;
  init();
}

void Button::init() {
  pinMode(pin, INPUT_PULLUP);
  update();
}


// Return true if button is updated and button is released
bool Button::update() {
  bool returnValue = false;
  byte newReading = digitalRead(pin);

  if (newReading != lastReading) {
    lastDebounceTime = millis();
  }

  // Update the 'state' attribute only if debounceDelay is exceeded
  if (millis() - lastDebounceTime > debounceDelay) {
    if (newReading != state) {
      if (newReading == LOW) {
        returnValue = true;
      }
    }

    state = newReading;
  }

  lastReading = newReading;
  return returnValue;
}

byte Button::getState() {
  update();
  return state;
}
