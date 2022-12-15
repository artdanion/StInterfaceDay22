#include <esp_now.h>
#include <WiFi.h>


bool leftClick;
bool rightClick;
bool rightRel;
bool leftRel;

bool sendData;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xA0, 0x76, 0x4E, 0x43, 0xC9, 0x48};
//uint8_t broadcastAddress[] = {0xA0, 0x76, 0x4E, 0x40, 0x11, 0xDC};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  bool swLeft;
  bool swRight;
  bool swLeftRel;
  bool swRightRel;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  sendData = false;
  rightRel = true;
  leftRel = true;
}

void loop() {

  leftClick = !digitalRead(25);
  rightClick = !digitalRead(26);


  if (leftClick && leftRel)
  {
    Serial.println();
    Serial.println("-------------");
    Serial.println("Left clicked");
    myData.swLeft = true;
    myData.swLeftRel=false;
    leftRel = false;
    sendData = true;
  }

  if (!leftClick && !leftRel)
  {
    Serial.println();
    Serial.println("-------------");
    Serial.println("Left released");
    myData.swLeftRel = true;
    myData.swLeft=false;
    leftRel = true;
    sendData = true;
  }

  if (rightClick && rightRel)
  {
    Serial.println();
    Serial.println("-------------");
    Serial.println("Right clicked");
    myData.swRight = true;
    myData.swRightRel=false;
    rightRel = false;
    sendData = true;
  }

  if (!rightClick && !rightRel)
  {
    Serial.println();
    Serial.println("-------------");
    Serial.println("Right released");
    myData.swRightRel = true;
    myData.swRight=false;
    rightRel = true;
    sendData = true;
  }

  // Send message via ESP-NOW
  if (sendData)
  {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.print("Sent with success");
    }
    else {
      Serial.print("Error sending the data");
    }
    sendData = false;
  }
  delay(10);
}
