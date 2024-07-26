#include "definitions.h"
#include <tuple>
#include <cmath>
#include <Arduino.h>
#include <vector>
#include <sstream>
#include <iostream>

int XY(int x, int y) {
  if (y % 2 == 0) {
    return y * DEF_SIZE_X + x;       // Even rows run left to right
  } else {
    return y * DEF_SIZE_Y + ((DEF_SIZE_X-1) - x); // Odd rows run right to left
  }
}

// Convert 2D coordinates to 1D index
int xyToIndex(int x, int y) {
    return y * DEF_SIZE_Y + x;
}

std::tuple<int, int> indexToXY(int index) {
  int x = index % DEF_SIZE_X;  // Calculate the x coordinate
  int y = index / DEF_SIZE_Y;  // Calculate the y coordinate
  return std::make_tuple(x, y);
}

int potValueMapper(int inputValue, int maxInVal, int maxOutValue, int minOutVal) {
  return map(inputValue, 0, maxInVal, minOutVal, maxOutValue);
}

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

int stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int number;
    iss >> number;
    return number;
}
