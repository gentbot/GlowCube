#include <FastLED.h>
#include "definitions.h"
#include <webmonitor.h>

// Function to calculate the intensity of the wave at position (x, y) at time t
float waveIntensity(int x, int y, float t) {
  float distance = sqrt((x - DEF_SIZE_X / 2) * (x - DEF_SIZE_X / 2) + (y - DEF_SIZE_Y / 2) * (y - DEF_SIZE_Y / 2));
  return (sin(distance - t) + 1.0) / 2.0; // Normalize to range [0, 1]
}

// Function to display the ripple wave effect on the LED matrix with color change
void displayWave2(CRGB leds[LED_COUNT], float t, CRGB color) {
  for (int y = 0; y < DEF_SIZE_Y; ++y) {
    for (int x = 0; x < DEF_SIZE_X; ++x) {
      int index = XY_ALIAS(x, y);
      float intensity = waveIntensity(x, y, t);
      leds[index] = color;
      leds[index].nscale8(intensity * MAX_COLOR_VALUE); // Scale color by intensity
    }
  }
  FastLED.show();
}

// Function to calculate the intensity of the colliding waves pattern
float collidingWavesPattern(int x, int y, float t) {
  float distance1 = sqrt((x - DEF_SIZE_X / 4.0) * (x - DEF_SIZE_X / 4.0) + (y - DEF_SIZE_Y / 4.0) * (y - DEF_SIZE_Y / 4.0));
  float distance2 = sqrt((x - 3 * DEF_SIZE_X / 4.0) * (x - 3 * DEF_SIZE_X / 4.0) + (y - 3 * DEF_SIZE_Y / 4.0) * (y - 3 * DEF_SIZE_Y / 4.0));
  float wave1 = (sin(distance1 - t) + 1.0) / 2.0;
  float wave2 = (sin(distance2 - t) + 1.0) / 2.0;
  return (wave1 + wave2) / 2.0; // Combine the two waves and normalize to range [0, 1]
}

// Function to display the colliding waves pattern on the LED matrix
void displayCollidingWaves(CRGB leds[LED_COUNT], float t, CRGB color) {
    for (int i = 0; i < LED_COUNT; ++i) {
        auto [x, y] = indexToXY(i);
        float intensity = collidingWavesPattern(x, y, t);
        // leds[i] = CHSV(internalHue, MAX_COLOR_VALUE, intensity * MAX_COLOR_VALUE); // Color based on intensity
        leds[i] = color;
    }
    FastLED.show();
}

void displayCollidingWaves2(CRGB leds[LED_COUNT], float t, CRGB color) {
  for (int i = 0; i < LED_COUNT; ++i) {
    auto [x, y] = indexToXY(i);
    float intensity = collidingWavesPattern(x, y, t);
    // leds[i] = CHSV(hue, MAX_COLOR_VALUE, intensity * MAX_COLOR_VALUE); // Color based on intensity
    leds[i] = color;
    leds[i].nscale8(intensity * MAX_COLOR_VALUE); // Scale color by intensity
  }
  FastLED.show();
}

// Function to animate an edge-crawling pixel
void displayEdgeCrawl(CRGB leds[LED_COUNT], float t, CRGB color) {
  // Calculate the position on the edge
  // int totalEdges = 32; // Total positions on the edge (8 + 8 + 8 + 8)
  int totalEdges = 96; // Total positions on the edge (8 + 40 + 8 + 40)
  int position = (int)(t * totalEdges) % totalEdges;

  // Clear all LEDs
  FastLED.clear();

  int x, y;
  if (position < 8) {            // Top edge (left to right)
      x = position;
      y = 0;
  // } else if (position < 16) {    // Right edge (top to bottom)
  } else if (position < 48) {    // Right edge (top to bottom)
      x = 7;
      y = position - 8;
  // } else if (position < 24) {    // Bottom edge (right to left)
  } else if (position < 56) {    // Bottom edge (right to left)
      // x = 23 - position;
      x = 55 - position;
      // y = 7;
      y = 39;
  } else {                       // Left edge (bottom to top)
      x = 0;
      // y = 31 - position;
      y = 95 - position;
  }

  // Set the color of the edge LED
  int index = xyToIndex(x, y);
  leds[index] = color;

  // Update the LEDs
  FastLED.show();
}

// Function to animate an edge-crawling pixel with a trail
void displayEdgeCrawlWithTrail(CRGB leds[LED_COUNT], float t, CRGB color) {
    // Calculate the position on the edge
    int totalEdges = 32; // Total positions on the edge (8 + 8 + 8 + 8)
    int position = (int)(t * totalEdges) % totalEdges;

    // Clear all LEDs
    FastLED.clear();

    // Set the color of the edge LED and its trail
    for (int i = 0; i < TRAIL_LENGTH; ++i) {
      int trailPosition = (position - i + totalEdges) % totalEdges;

      int x, y;
      if (trailPosition < 8) {            // Top edge (left to right)
          x = trailPosition;
          y = 0;
      } else if (trailPosition < 16) {    // Right edge (top to bottom)
          x = 7;
          y = trailPosition - 8;
      } else if (trailPosition < 24) {    // Bottom edge (right to left)
          x = 23 - trailPosition;
          y = 7;
      } else {                            // Left edge (bottom to top)
          x = 0;
          y = 31 - trailPosition;
      }

      // Set the color of the trail LED with decreasing intensity
      int index = xyToIndex(x, y);
      leds[index] = color;
      leds[index].fadeToBlackBy((255 / TRAIL_LENGTH) * i);
    }

    // Update the LEDs
    FastLED.show();
}

// Function to generate a spiral pattern
void generateSpiralOrder(int order[LED_COUNT]) {
    int x = 0, y = 0;
    int dx = 1, dy = 0;
    int segmentLength = 8;

    int index = 0;
    for (int i = 0; i < segmentLength * segmentLength; i++) {
        order[index++] = xyToIndex(x, y);
        if (--segmentLength == 0) {
            segmentLength = (dy == 0) ? 8 : 8 - (index / 8);
            std::swap(dx, dy);
            dy = -dy;
        }
        x += dx;
        y += dy;
    }
}

// Function to animate a spiral pattern
void displaySpiral(CRGB leds[LED_COUNT], float t, CRGB color) {
    static int order[LED_COUNT];
    static bool orderGenerated = false;

    if (!orderGenerated) {
        generateSpiralOrder(order);
        orderGenerated = true;
    }

    int position = (int)(t * LED_COUNT) % LED_COUNT;

    // Clear all LEDs
    FastLED.clear();

    // Set the color of the LEDs up to the current position
    for (int i = 0; i <= position; ++i) {
        leds[order[i]] = color;
    }

    // Update the LEDs
    FastLED.show();
}

// POng style animation
int ballX = 3, ballY = 3;
int ballDirX = 1, ballDirY = 1;

int paddleLeftY = 3;
int paddleRightY = 3;
int paddleHeight = 3;

// Function to display the Pong game
void generatePong(CRGB leds[LED_COUNT]) {
    // Clear all LEDs
    FastLED.clear();

    // Draw left paddle
    for (int i = 0; i < paddleHeight; ++i) {
        leds[xyToIndex(0, paddleLeftY + i)] = CRGB::Blue;
    }

    // Draw right paddle
    for (int i = 0; i < paddleHeight; ++i) {
        leds[xyToIndex(7, paddleRightY + i)] = CRGB::Blue;
    }

    // Draw the ball
    leds[xyToIndex(ballX, ballY)] = CRGB::Red;

    // Update the LEDs
    FastLED.show();
}

// Function to update the ball position
void updateBall() {
    // Move the ball
    ballX += ballDirX;
    ballY += ballDirY;

    // Bounce off the top or bottom
    if (ballY == 0 || ballY == 7) {
        ballDirY = -ballDirY;
    }

    // Bounce off the left paddle
    if (ballX == 0 && ballY >= paddleLeftY && ballY < paddleLeftY + paddleHeight) {
        ballDirX = -ballDirX;
    }

    // Bounce off the right paddle
    if (ballX == 7 && ballY >= paddleRightY && ballY < paddleRightY + paddleHeight) {
        ballDirX = -ballDirX;
    }

    // Reset ball if it goes past the paddles
    if (ballX < 0 || ballX > 7) {
        ballX = 3;
        ballY = 3;
        ballDirX = -ballDirX;
    }
}

// Function to update paddle positions (simple AI)
void updatePaddles() {
    // Move left paddle to follow the ball
    if (ballDirX == -1) {
        if (ballY < paddleLeftY) {
            paddleLeftY--;
        } else if (ballY > paddleLeftY + paddleHeight - 1) {
            paddleLeftY++;
        }
    }

    // Move right paddle to follow the ball
    if (ballDirX == 1) {
        if (ballY < paddleRightY) {
            paddleRightY--;
        } else if (ballY > paddleRightY + paddleHeight - 1) {
            paddleRightY++;
        }
    }

    // Keep paddles within bounds
    if (paddleLeftY < 0) paddleLeftY = 0;
    if (paddleLeftY > 7 - paddleHeight) paddleLeftY = 7 - paddleHeight;
    if (paddleRightY < 0) paddleRightY = 0;
    if (paddleRightY > 7 - paddleHeight) paddleRightY = 7 - paddleHeight;
}

void displayPong(CRGB leds[LED_COUNT], float t, CRGB color) {
  generatePong(leds);
  updateBall();
  updatePaddles();
}


// sine wave
const float frequency = 0.2; // Frequency of the sine wave
const float amplitude = 3.0; // Amplitude of the sine wave
const float speed = 0.1; // Speed of the sine wave animation

// Function to display the sine wave
void generateSineWave(CRGB leds[LED_COUNT], float phase) {
    // Clear all LEDs
    FastLED.clear();
    
    // Draw the sine wave
    for (int x = 0; x < 8; ++x) {
        float y = amplitude * sin(frequency * x + phase) + 3.5; // 3.5 to center the wave vertically
        int ledIndex = xyToIndex(x, round(y));
        leds[ledIndex] = CRGB::Blue;
    }
    
    // Update the LEDs
    FastLED.show();
}

void displaySineWave(CRGB leds[LED_COUNT], float t, CRGB color) {
  static float phase = 0;
  generateSineWave(leds, phase);
  phase += speed;
}

void displayCubeSnakeWalk(CRGB leds[LED_COUNT], float t, CRGB color) {
  FastLED.clear();
  // walk down two diagonal leds across all faces of the cube
  for (int grid = 0; grid < NUM_GRID_FACES; ++grid) {
    for (int x = 3; x < 4; ++x) {
      for (int y = 0; y < 40; ++y) {
        int ledIndex = (64 * grid) + x + y;
        if(x%2 == 0) {
          leds[ledIndex] = color;
        } else {
          leds[ledIndex] = color;
        }
      }
      FastLED.show();
    }
  }
  
}
