#include "definitions.h"
#include "animations.h"
#include "manager.h"
#include <FastLED.h>
#include <webmonitor.h>

// led container
CRGB leds[LED_COUNT];
// CRGBPalette16 currentPalette;
// TBlendType currentBlending;
int currentAnimationIndex = 0;
float internalTimer = 0.0;
uint8_t internalHue = 0;
// FunctionPtr functions[] = {displaySineWave, displayPong, displayEdgeCrawlWithTrail, displayEdgeCrawl, displayWave2};
FunctionPtr functions[] = {displayEdgeCrawl, displayCubeSnakeWalk, displayWave2, displayEdgeCrawlWithTrail, displayEdgeCrawl};
int numFunctions = sizeof(functions) / sizeof(functions[0]);

void matrixTest(int repeaterCount = 2) {
  for(int i = 0; i < repeaterCount; i++){
    // Turn on the single LED
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(500);

    // Turn off the single LED
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(500);
  }
}

template <typename CustomItem>
int sizeOfList(CustomItem list) {
  return sizeof(list) / sizeof(list[0]);
}

template<typename T, size_t N>
constexpr size_t arraySize(T (&)[N]) {
  return N;
}

// Define the type of function pointers
// typedef void (*FunctionPtr)();
// FunctionPtr arbitraryFunctionPicker() {
void arbitraryFunctionPicker() {
  // Seed the random number generator
  srand(time(NULL));

  // Generate a random index between 0 and numFunctions - 1
  // int randomIndex = rand() % numFunctions;
  currentAnimationIndex = rand() % numFunctions;

  // Call the function at the random index
  // functions[currentAnimationIndex](leds, 0, color);
  // return functions[currentAnimationIndex];
}
void incrementHueAndTime() {
  internalHue += 1;
  internalTimer += 0.1;
}

void callSelectedFunction(int timeItem, uint8_t hueItem, int repeatCount, int delayAmount, bool offAfterAnimation) {
  // Convert hue to RGB color
  if(repeatCount > 0) {
    for(int i = 0; i < repeatCount; i++){
      // CRGB colorSelect = CHSV(hueItem, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
      CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
      functions[currentAnimationIndex](leds, internalTimer, colorSelect);
      incrementHueAndTime();
      delay(delayAmount);
    }
  } else {
    CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
    functions[currentAnimationIndex](leds, internalTimer, colorSelect);
    incrementHueAndTime();
    delay(delayAmount);
  }

  if(offAfterAnimation) {
    FastLED.clear();
    FastLED.show();
  }
}

std::vector<int> prevPosition;

// Function to calculate the shortest path between two points on an 8x8 grid
std::vector<int> getPath(int x0, int y0, int x1, int y1) {
  std::vector<int> path;

  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; /* error value e_xy */

  while (true) {
    path.push_back(XY_ALIAS(x0, y0));  // Add the LED index to the path

    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }

  return path;
}

void iluminateSingleLEDXY(int x, int y) {
  // FastLED.clear();

  // Fade all LEDs to create a trailing effect
  for (int i = 0; i < LED_COUNT; ++i) {
    // leds[i].fadeToBlackBy(255 / TRAIL_LENGTH);
    leds[i].fadeToBlackBy(255 / 4);
    // leds[i].fadeToBlackBy(2);
  }

  CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
  leds[XY_ALIAS(x, y)] = colorSelect;
  prevPosition.clear();
  prevPosition.push_back(x);
  prevPosition.push_back(y);
  FastLED.show();
  incrementHueAndTime();
}

void iluminatePathLEDXY(int x, int y) {
  // FastLED.clear();
  if(prevPosition.size() == 0) {
    iluminateSingleLEDXY(x,y);
    return;
  }
  // Fade all LEDs to create a trailing effect
  for (int i = 0; i < LED_COUNT; ++i) {
    leds[i].fadeToBlackBy(255 / 4);
  }

  CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
  std::vector<int> path = getPath(prevPosition[0], prevPosition[1], x, y);
  prevPosition.clear();
  prevPosition.push_back(x);
  prevPosition.push_back(y);
  for(int j = 0; j < path.size(); ++j){
    leds[j] = colorSelect;
    incrementHueAndTime();
  }
  
  FastLED.show();
}

void iluminateEachLED() {
  FastLED.clear();
  for(int i = 0; i < LED_COUNT; ++i) {
    CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
    leds[i] = colorSelect;
    incrementHueAndTime();
    FastLED.show();
    delay(5);
  }
}

void alphaOmegle() {
  FastLED.clear();
  // leds[0] = colorSelect;
  // leds[LED_COUNT-1] = colorSelect;
  // leds[((LED_COUNT-1)/2)] = colorSelect;
  // for(int i = 0; i < 5; ++i ){
  //   CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
  //   for(int j = 0; j < 8; ++j){
  //     // leds[XY_ALIAS((j*i), i)] = colorSelect;
  //     leds[j + i * 8] = colorSelect;
  //   }
  // }

  for(int grid = 0; grid < 5; ++grid) {
    CRGB colorSelect = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
    for(int x = 0; x < 8; ++x) {
      for(int y = 0; y < 1; ++y) {
        int ledIndex = (64 * grid) + x + y;
        leds[ledIndex] = colorSelect;
      }
    }
    incrementHueAndTime();
  }
  FastLED.show();

  // static uint16_t wavePos = 0;

  // for (int grid = 0; grid < NUM_GRIDS; grid++) {
  //   for (int y = 0; y < ROWS_PER_GRID; y++) {
  //     for (int x = 0; x < COLUMNS; x++) {
  //       int index = grid * (COLUMNS * ROWS_PER_GRID) + ((y % 2 == 0) ? (y * COLUMNS + x) : (y * COLUMNS + (COLUMNS - 1 - x)));
  //       uint8_t brightness = sin8(wavePos + (y + grid * ROWS_PER_GRID) * 16); // Adjust wave speed and frequency
  //       // leds[index] = CHSV(160, 255, brightness); // Color can be adjusted by changing the first parameter
  //       leds[index] = CHSV(internalHue, MAX_COLOR_VALUE, MAX_COLOR_VALUE);
  //     }
  //   }
  // }
  // wavePos += 2; // Adjust the wave speed
  // incrementHueAndTime();
  // FastLED.show();
  // delay(100);
}


// this should be last
void initMatrixManager(int repeatStartSequenceCount) {
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInMilliWatts(300);
  FastLED.clear();
  FastLED.show();

  callSelectedFunction(internalTimer, internalHue, repeatStartSequenceCount, 50, true);
  // iluminateEachLED();
}

// Nothing below this line