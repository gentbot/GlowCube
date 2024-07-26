#include <Arduino.h>
#include <string>
#include <vector>
#include <iostream>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <wificonnect.h>
#include <webmonitor.h>
#include <animations/manager.h>
#include "time.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_I2CDevice.h>
#include <secret.h>

#define ACCEL_SCL 17
#define ACCEL_SDA 18

#define MIN_ACCEL_RANGE -4
#define MAX_ACCEL_RANGE 4
#define SPEED_KNOB 7
#define NTPSERVER "pool.ntp.org"
#define DAYLIGHTOFFSET_SEC 3600
#define GMTOFFSET_SEC -8* DAYLIGHTOFFSET_SEC
#define MATRIX_PIN 12
#define SDA_PIN 40
#define SCL_PIN 39
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define LG_SCREEN_WIDTH 128
#define LG_SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SSD1306_I2C_ADDRESS 0x3C  // Commonly used I2C address for SSD1306. 0x3D for other display types
#define SSD1306_LG_I2C_ADDRESS 0x3D  // Commonly used I2C address for SSD1306. 0x3D for other display types
#define ADXL345_I2C_ADDRESS 0x53  // Commonly used I2C address for SSD1306. 0x3D for other display types

// Create the TwoWire instances
// TwoWire I2C1 = TwoWire(0);
// TwoWire I2C2 = TwoWire(1);

float mainTimer = 0.0;                // Time variable for animation
uint8_t mainHue = 0;              // Hue for color
// Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 lgscreen(LG_SCREEN_WIDTH, LG_SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C1, OLED_RESET);
// Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C1, OLED_RESET, SDA_PIN, SCL_PIN);
// Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SDA_PIN, SCL_PIN);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

// Create the ADXL345 object
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
// Create I2C device instance for ADXL345
// Adafruit_I2CDevice i2c_dev_adxl = Adafruit_I2CDevice(ADXL345_I2C_ADDRESS, &I2C2);

using namespace std;

struct DisplayContent {
  String content = "";
  bool useNewLine = true;
  int textSize = 1;
  DisplayContent(const String& pCopy, bool newLine, int newTextSize = 1) : content(pCopy), useNewLine(newLine), textSize(newTextSize){}
};

using DisplayContentList = vector<DisplayContent>;

void setTextSizeDirectly(int textSize = 1) {
  // screen.setTextSize(textSize);
  lgscreen.setTextSize(textSize);
  
}

void resetDisplay(int textSize = 1) {
  // screen.clearDisplay();
  lgscreen.clearDisplay();
  setTextSizeDirectly(textSize);
  // screen.setTextColor(SSD1306_WHITE);
  lgscreen.setTextColor(SSD1306_WHITE);
  // screen.setCursor(0, 0);
  lgscreen.setCursor(0, 0);
}

void basicSetupItems() {
  delay(1000); // Power-up safety delay
  // printf("Check this");
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial port to connect.
  }
}

void getCurrentTime() {
  // Get and print the current time periodically
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    WebSerial.println(&timeinfo, "Current time: %A, %B %d %Y %H:%M:%S");
  } else {
    WebSerial.println("Failed to obtain time");
  }
  delay(10000); // Update every 10 seconds
}

void setupLocalTime() {
  // Initialize NTP
  configTime(GMTOFFSET_SEC, DAYLIGHTOFFSET_SEC, NTPSERVER);

  // Wait for time to be set
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    WebSerial.println("Failed to obtain time");
    return;
  }

  getCurrentTime();
}

void testFontSlectionsAndDisplay() {
  // screen.clearDisplay();
  lgscreen.clearDisplay();
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color
  
  u8g2_for_adafruit_gfx.setFont(u8g2_font_siji_t_6x10);  // icon font
  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.drawGlyph(0, 10, 0x0e200);  // Power Supply
  u8g2_for_adafruit_gfx.drawGlyph(12, 10, 0x0e201);  // Charging
  u8g2_for_adafruit_gfx.drawGlyph(24, 10, 0x0e10a);  // Right Arrow
  u8g2_for_adafruit_gfx.drawGlyph(36, 10, 0x0e24b);  // full Battery

  u8g2_for_adafruit_gfx.setFont(u8g2_font_7x13_te);  // extended font
  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setCursor(0,40);                // start writing at this position
  u8g2_for_adafruit_gfx.print("<Ȧǀʘ>"); 
  // screen.display();
  lgscreen.display();
}

void setupScreen() {
  Wire.begin(SDA_PIN, SCL_PIN);
  // Initialize I2C1 with specified SDA and SCL pins
  // I2C1.begin(SDA_PIN, SCL_PIN);
  
  // if(!screen.begin(SSD1306_I2C_ADDRESS, OLED_RESET)) {
  // if(!screen.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS)) {
  //   WebSerial.println(F("SSD1306 allocation failed"));
  //   for(;;);
  // }
  
  if(!lgscreen.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS)) {
    WebSerial.println(F("LG SSD1306 allocation failed"));
    for(;;);
  }
  // u8g2_for_adafruit_gfx.begin(screen);
  u8g2_for_adafruit_gfx.begin(lgscreen);
  // u8g2_for_adafruit_gfx.setFont(u8g2_font_siji_t_6x10);
  // u8g2_for_adafruit_gfx.setFont(u8g2_font_3x3basic_tr);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_4x6_tf);
}

void setupAccelerometer() {
  // Wire.begin(ACCEL_SDA, ACCEL_SCL);
  // accel.begin(ADXL345_I2C_ADDRESS);
  // Initialize I2C2 with specified SDA and SCL pins
  // I2C2.begin(ACCEL_SDA, ACCEL_SCL);

  // // Initialize the ADXL345 sensor on I2C1
  // if (!accel.begin(0x53, &I2C2)) {
  //   Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  //   while (1);
  // }
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected... Check your wiring!");
    while (1);
  }

  // if (!i2c_dev_adxl.begin()) {
  //   Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  //   WebSerial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  //   while (1);
  // }
  // if (!accel.begin(&i2c_dev_adxl)) {
  // if (!accel.begin(ADXL345_I2C_ADDRESS)) {
  //   Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  //   WebSerial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  //   // while (1);
  // }


  // Set the range to +/- 16G for ADXL345
  accel.setRange(ADXL345_RANGE_16_G);
}

void i2cDeviceScanner() {
  byte error, address;
  int deviceCount = 0;

  WebSerial.println("Scanning...");

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      WebSerial.print("I2C device found at address 0x");
      if (address < 16) {
        WebSerial.print("0");
      }
      WebSerial.print(address);
      WebSerial.print(HEX);
      WebSerial.println(" !");
      deviceCount++;
    }    
    else if (error == 4) {
      WebSerial.print("Unknown error at address 0x");
      if (address < 16) {
        WebSerial.print("0");
      }
      WebSerial.print(address);
      WebSerial.print(HEX);
    }    
  }
  
  if (deviceCount == 0)
    Serial.println("No I2C devices found\n");

  // delay(5000); // Wait 5 seconds for the next scan
}

// output something to the screen
void displayItemsOnScreen(const String& displayItem, bool clearScreen = true, int textSize = 1) {
  if(clearScreen) {
    resetDisplay(textSize);
  }

  // screen.println(displayItem.c_str());
  // screen.display();
  lgscreen.println(displayItem.c_str());
  lgscreen.display();
}

void displayItemsOnScreen(const DisplayContentList& dispalyItems, bool clearScreen = true, int textSize = 1) {
  if(clearScreen) {
    resetDisplay(textSize);
  }

  for(const auto& displayCopy : dispalyItems) {
    setTextSizeDirectly((textSize < displayCopy.textSize) ? displayCopy.textSize: textSize);
    if(displayCopy.useNewLine) {
      // screen.println(displayCopy.content.c_str());
      lgscreen.println(displayCopy.content.c_str());
    } else {
      // screen.print(displayCopy.content.c_str());
      lgscreen.print(displayCopy.content.c_str());
    }
    // WebSerial.print(displayCopy.content.c_str());
  }

  // screen.display();
  lgscreen.display();
}

void setup() {
  pinMode(SPEED_KNOB, INPUT);
  basicSetupItems();
  initMatrixManager(20);
  setupScreen();
  resetDisplay();
  displayItemsOnScreen("looking for wifi...", true);
  init_wifi_connection(ssid, pass);
  DisplayContent ipContent(get_ip(), true);
  displayItemsOnScreen({ipContent}, true, 2);
  
  if(isWifiConnected()) {
    create_web_monitor();
    setupLocalTime();
    displayItemsOnScreen("accelerometer now connecting");
    setupAccelerometer();
    displayItemsOnScreen("accelerometer connected");
  }
}

void getAndConvertAccelData(sensors_event_t &event) {
  // sensors_event_t event;
  accel.getEvent(&event);
  int x = map(event.acceleration.x, MIN_ACCEL_RANGE, MAX_ACCEL_RANGE, 0, DEF_SIZE_X);
  int y = map(event.acceleration.y, MIN_ACCEL_RANGE, MAX_ACCEL_RANGE, 0, DEF_SIZE_Y);
  x = constrain(x, 0, DEF_SIZE_X);
  y = constrain(y, 0, DEF_SIZE_Y);
  iluminateSingleLEDXY(x, y);
  // iluminatePathLEDXY(x, y);
}

void loop() {
  int speedKnobValue = analogRead(SPEED_KNOB);
  int speedMappedValue = potValueMapper(speedKnobValue, MAX_POT_VAL, 100, 5);
  // callSelectedFunction(mainTimer, mainHue, 0, speedMappedValue, false);
  
  // testFontSlectionsAndDisplay();

  EVERY_N_MILLISECONDS(speedMappedValue*.5) {
    mainTimer += 0.1;
    mainHue += 1;
  }

  EVERY_N_MILLIS(100) {
    callSelectedFunction(mainTimer, mainHue, 0, speedMappedValue, false);
    // alphaOmegle();
    
    DisplayContent brightValue("Speed: ", false, 1);
    DisplayContent speedDisplayValue(String(speedMappedValue), true, 1);
    // displayItemsOnScreen({brightValue, speedDisplayValue}, false, 1);

    // // Read the accelerometer data
    sensors_event_t accel_event;
    accel.getEvent(&accel_event);

    // getAndConvertAccelData(accel_event);
    DisplayContent accelTitleX("X: ", false);
    DisplayContent accelValueX(String(accel_event.acceleration.x), true);
    DisplayContent accelTitleY("Y: ", false);
    DisplayContent accelValueY(String(accel_event.acceleration.y), true);
    DisplayContent accelTitleZ("Z: ", false);
    DisplayContent accelValueZ(String(accel_event.acceleration.z), true);
    DisplayContent accelTitleRoll("ROLL: ", false);
    DisplayContent accelValueRoll(String(accel_event.acceleration.roll), true);
    DisplayContent accelTitlePitch("PITCH: ", false);
    DisplayContent accelValuePitch(String(accel_event.acceleration.pitch), true);
    DisplayContent accelTitleHeading("HEADING: ", false);
    DisplayContent accelValueHeading(String(accel_event.acceleration.heading), true);
    displayItemsOnScreen({
      brightValue,
      speedDisplayValue,
      accelTitleX,
      accelValueX,
      accelTitleY,
      accelValueY,
      accelTitleZ,
      accelValueZ,
      accelTitleRoll,
      accelValueRoll,
      accelTitlePitch,
      accelValuePitch,
      accelTitleHeading,
      accelValueHeading,
    }, true, 1);
  }

  // EVERY_N_SECONDS(3) {
  //   i2cDeviceScanner();
  // }

  // EVERY_N_SECONDS(5) {
  //   arbitraryFunctionPicker();
  // }
}
