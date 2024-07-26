#ifndef MANAGER_H
#define MANAGER_H

#define BIG_MATRIX false
#include "definitions.h"
#include <FastLED.h>

typedef std::function<void(CRGB leds[LED_COUNT], float t, CRGB color)> FunctionPtr;

void initMatrixManager(int repeatStartSequenceCount = 1);
void arbitraryFunctionPicker();
void iluminateSingleLEDXY(int x, int y);
void iluminatePathLEDXY(int x, int y);
void iluminateEachLED();
void alphaOmegle();
// void callSelectedFunction(int timeItem, uint8_t hueItem);
void callSelectedFunction(int timeItem, uint8_t hueItem, int repeatCount = 0, int delayAmount = 50, bool offAfterAnimation = true);
#endif //MANAGER_H