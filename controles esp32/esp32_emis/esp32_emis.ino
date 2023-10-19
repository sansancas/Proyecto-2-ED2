#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  int button1State;
  int button2State;
} MyData;

MyData myData;

const int buttonPin1 = 2; // Primer botón
const int buttonPin2 = 3; // Segundo botón

void setup() {
  WiFi.begin("FAMILIARUIZ", "RUIZ1607");
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("Error en la inicialización de ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != 0) {
    Serial.println("Error al agregar el peer");
    return;
  }

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP); // Configurar el segundo botón como entrada pull-up

  Serial.begin(115200);
}

void loop() {
  myData.button1State = !digitalRead(buttonPin1); // Leer el estado del primer botón
  myData.button2State = !digitalRead(buttonPin2); // Leer el estado del segundo botón

  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  Serial.println("Datos enviados: " + String(myData.button1State) + ", " + String(myData.button2State));

  delay(1000);
}
