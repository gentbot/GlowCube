#include <WiFi.h>

void init_wifi_connection(const char* ssid, const char* password, std::function<void(bool)> callback = nullptr);
String get_ip();
bool isWifiConnected();