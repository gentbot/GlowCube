#ifndef WEBMONITOR_H
#define WEBMONITOR_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <cstdarg> // Include cstdarg for variadic arguments

#define PREFIXER "[SERIAL OUTPUT]::"
template<typename CustomType>
void WSSerialPrint(const CustomType& message);
template<>
void WSSerialPrint<char const*>(char const* const& message);

template<typename CustomType>
void WSSerialPrintln(const CustomType& message);
template<>
void WSSerialPrintln<char const*>(char const* const& message);

// Define a custom Serial class with print and println methods
class SerialClass {
private:
  String messageBuffer;
  // const char* prefixer(const char* message, bool newLine = false) {
  template<typename CustomType>
  const char* prefixer(const CustomType& message, bool newLine = false) {
    messageBuffer = String(PREFIXER) + message;
    if(newLine) {
      messageBuffer += "\n";
    }
    return messageBuffer.c_str();
  }

public:
  template<typename CustomType>
  void print(const CustomType& message) {  
    WSSerialPrint(prefixer(message));
  }

  // void println(const char* message) {
  template<typename CustomType>
  void println(const CustomType& message) {
    WSSerialPrintln(prefixer(message, true));
  }
  
  void println(const struct tm* timeinfo, const char* format) {
    char buffer[64]; // Adjust buffer size as needed
    strftime(buffer, sizeof(buffer), format, timeinfo);
    WSSerialPrintln(prefixer(buffer, true));
  }

  template<typename... Args>
  void println(const char* format, Args... args) {
    char buffer[256]; // Adjust buffer size as needed
    snprintf(buffer, sizeof(buffer), format, args...);
    WSSerialPrintln(prefixer(buffer, true));
  }
};

// Create an instance of the custom Serial class
extern SerialClass WebSerial;
// extern SerialClass SerialOverride;

// Define macros to override Serial with our custom implementation
// #__d-efine Serial SerialOverride

// void create_web_monitor(char* ssid, char* password);
void create_web_monitor(std::function<void(bool)> callback = nullptr);
void test_connection_and_messaging();

#endif // WEBMONITOR_H