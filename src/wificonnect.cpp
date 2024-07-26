#include <Wifi.h>

void init_wifi_connection(const char* ssid, const char* password, std::function<void(bool)> callback = nullptr) {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String get_ip() {
  if(isWifiConnected()) {
    return WiFi.localIP().toString();
  } else {
    return "No wifi connection found";
  }
}