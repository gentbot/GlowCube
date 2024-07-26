#include <FastLED.h>
#include "definitions.h"

void displayWave2(CRGB leds[LED_COUNT], float t, CRGB color);
void displayEdgeCrawl(CRGB leds[LED_COUNT], float t, CRGB color);
void displayEdgeCrawlWithTrail(CRGB leds[LED_COUNT], float t, CRGB color);
void displaySpiral(CRGB leds[LED_COUNT], float t, CRGB color);
void displayPong(CRGB leds[LED_COUNT], float t, CRGB color);
void displaySineWave(CRGB leds[LED_COUNT], float t, CRGB color);
void displayCubeSnakeWalk(CRGB leds[LED_COUNT], float t, CRGB color);
