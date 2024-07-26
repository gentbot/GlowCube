#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define LED_TYPE    WS2812B   // Type of LED
#define COLOR_ORDER GRB       // Color order for the NeoPixel
#define MATRIX_PIN 12
#define NUM_LEDS 256
// #_d_efine NUM_LEDS_SMALL 64
// #_d_efine NUM_LEDS_SMALL 128
#define NUM_LEDS_SMALL 320
#define BRIGHTNESS 5
#define MAX_COLOR_VALUE 255
#define MAX_POT_VAL 4095
#define NUM_GRID_FACES 5

#if BIG_MATRIX == true
  #define LED_COUNT NUM_LEDS
  #define XY_ALIAS XY
  #define DEF_SIZE 16
  #define DEF_SIZE_X 16
  #define DEF_SIZE_Y 16
#else
  #define LED_COUNT NUM_LEDS_SMALL
  #define XY_ALIAS xyToIndex
  #define DEF_SIZE 8
  #define DEF_SIZE_X 40
  #define DEF_SIZE_Y 8
#endif

#define TRAIL_LENGTH 12
#include <tuple>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>
// #include <string>

struct WebSerialOutput {
  char* name;
  int value;
  // DisplayContent(const String& pCopy, bool newLine, int newTextSize = 1) : content(pCopy), useNewLine(newLine), textSize(newTextSize){}
};

const int SIZE = DEF_SIZE;

int XY(int x, int y);
// Convert 2D coordinates to 1D index
int xyToIndex(int x, int y);
std::tuple<int, int> indexToXY(int index);
int potValueMapper(int inputValue, int maxInVal, int maxOutValue = MAX_COLOR_VALUE, int minOutVal = 0);

std::vector<std::string> splitString(const std::string &str, char delimiter);

int stringToInt(const std::string& str);
#endif //DEFINITIONS_H