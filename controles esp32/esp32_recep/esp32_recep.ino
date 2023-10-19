#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  int button1State;
  int button2State;
} MyData;

MyData myData;

const int ledPin1 = 23; // Primer LED
const int ledPin2 = 22; // Segundo LED

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&myData, data, sizeof(myData));

  // Controlar el primer LED en función del estado del primer botón
  if (myData.button1State == 1) {
    digitalWrite(ledPin1, HIGH); // Encender el primer LED cuando se recibe un 1
    Serial.println("Primer botón presionado (1)");
  } else {
    digitalWrite(ledPin1, LOW); // Apagar el primer LED cuando se recibe un 0
    Serial.println("Primer botón liberado (0)");
  }

  // Controlar el segundo LED en función del estado del segundo botón
  if (myData.button2State == 1) {
    digitalWrite(ledPin2, HIGH); // Encender el segundo LED cuando se recibe un 1
    Serial.println("Segundo botón presionado (1)");
  } else {
    digitalWrite(ledPin2, LOW); // Apagar el segundo LED cuando se recibe un 0
    Serial.println("Segundo botón liberado (0)");
  }
}

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

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT); // Configurar el segundo LED como salida

  Serial.begin(115200);
}

void loop() {
  // No es necesario hacer nada en el bucle principal
}
 
