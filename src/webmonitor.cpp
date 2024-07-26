#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <webmonitor.h>

// Create an instance of the server
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

template<typename CustomType>
void WSSerialPrint(CustomType message) {
  // ws.client() ->text(message);
  ws.textAll(message);
}
template<>
void WSSerialPrint<char const*>(char const* const& message) {
  // ws.client() ->text(message);
  ws.textAll(message);
}

template<typename CustomType>
void WSSerialPrintln(CustomType message) {
  // Example WebSocket implementation with newline
  // ws.client() ->text(message + "\n");
  String msgWithNewline = String(message) + "\n";
  ws.textAll(msgWithNewline.c_str());
}

template<>
void WSSerialPrintln<char const*>(char const* const& message) {
  // Example WebSocket implementation with newline
  // ws.client() ->text(message + "\n");
  String msgWithNewline = String(message) + "\n";
  ws.textAll(msgWithNewline.c_str());
}

SerialClass WebSerial;

void printAllClients() {
  // Get the list of clients
  const auto& clients = ws.getClients();

  // Iterate over each client using range-based for loop
  for (const auto& client : clients) {
    if (client->status() == WS_CONNECTED) {
      // Print information about the connected client
      WebSerial.print("Client ID: ");
      WebSerial.print(client->id());
      WebSerial.print(", Remote IP: ");
      WebSerial.print(client->remoteIP());
      WebSerial.print(", Remote Port: ");
      WebSerial.println(client->remotePort());
    }
  }
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    WebSerial.println(" WebSocket client connected");
    client->text("Hello from ESP32 WebSocket server");
    // printAllClients();
  } else if (type == WS_EVT_DISCONNECT) {
    WebSerial.println("WebSocket client disconnected");
  } else if (type == WS_EVT_DATA) {
    WebSerial.print("Data received");
    for (size_t i = 0; i < len; i++) {
      WebSerial.print((const char*) data[i]);
    }
    client->text("Round trip. message received");
  }
}

void create_web_monitor(std::function<void(bool)> callback) {
  if(callback) {
    callback(false);
    delay(50);
    callback(true);
    delay(50);
    callback(false);
    delay(50);
    callback(true);
    delay(50);
    callback(false);
    delay(50);
  }
  // WebSocket event handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serve the HTML file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <style>
    * {
      margin: 0;
      padding: 0;
      background-color: black;
      color: white;
    }

    #tx {
      width: 90vw;
      max-width: 90vw;
      min-height: 20vh; /* Set a specific height or use auto to expand */
      max-height: 80vh;
      overflow-y: scroll;
      padding: 1rem; /* Optional: Adjust padding as needed */
      font-size: 1rem; /* Optional: Adjust font size as needed */
      border: 1px solid #ccc; /* Optional: Add border for styling */
      resize: vertical; /* Optional: Allow vertical resizing */
    }

    #grid-container {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;
      width: 100%;
      height: 100%;
    }

    .grid-cell {
      text-align: center;
      border: 1px solid white;
      cursor: pointer;
    }

    .grid-cell:hover {
      background-color: blue;
    }
  </style>
</head>
<body>
  <h1>ESP32 WebSocket Server</h1>
  <input type="text" id="message" placeholder="Message">
  <button onclick="sendMessage()">Send</button>
  <div id="content_area">
  <textarea id="tx" rows="50" ></textarea>
  </div>

  <div id="grid-container"></div>
  <script>
    const gateway = `ws://${window.location.hostname}/ws`;
    const content = document.getElementById('content_area');
    const tx = document.getElementById('tx');
    const container = document.getElementById('grid-container');
    let websocket;
    window.addEventListener('load', onLoad);

    function onLoad(event) {
      initWebSocket();
      container.innerHTML = createGrid(64);
      // container.addEventListener('mouseover', (e) => {
      //   // console.log("CELL Hover event ", e.target.dataset.cell);
      //   websocket.send(`position, ${e.target.dataset.cell}`);
      // })
    }

    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }

    function onOpen(event) {
      console.log('Connection opened');
    }

    function onClose(event) {
      console.log('Connection closed');
      setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
      console.log('Message received: ' + event.data);
      addText(event.data);
    }

    function sendMessage() {
      var message = document.getElementById('message').value;
      console.group("socket message")
      console.log(`Sending the message: ${message}`)
      console.groupEnd()
      websocket.send(message);
    }

    function addText(inputText) {
      if(!inputText) return;
      tx.value += `${inputText}\n`;
    }

    function createGrid(count) {
        let gridStructure = "";
        for(let i = 0; i < count; i++) {
          gridStructure += `<div class="grid-cell" data-cell=${i} >${i+1}</div>`
        }
    
      return gridStructure;
    }
  </script>
</body>
</html>
    )rawliteral");
  });

  // Start the server
  server.begin();
}

void test_connection_and_messaging() {
  // Broadcast a message every 10 seconds
  static unsigned long lastBroadcast = 0;
  if (millis() - lastBroadcast > 10000 && WiFi.status() == WL_CONNECTED) {
    lastBroadcast = millis();
    ws.textAll("Broadcast message from ESP32");
  }
}
