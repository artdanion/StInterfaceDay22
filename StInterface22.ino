// the number of the LED pin
const int swLeft = D10;
const int swRight = D9;

#include <esp_now.h>
#include "WiFi.h"

// setting PWM properties
const int freq = 8000;
const int swLeftChannel = 0;
const int swRightChannel = 1;
const int resolution = 8;

bool leftClick = false;
bool rightClick = false;
bool leftRel = false;
bool rightRel = false;

typedef struct struct_message {
  bool swLeft;
  bool swRight;
  bool swLeftRel;
  bool swRightRel;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.print("Bytes received: ");
  //Serial.println(len);

  if (myData.swLeft)
  {
    leftClick = true;
    leftRel = false;
  }

  if (myData.swLeftRel)
  {
    leftRel = true;
    leftClick = false;
  }

  if (myData.swRight)
  {
    rightClick = true;
    rightRel = false;
  }

  if (myData.swRightRel)
  {
    rightRel = true;
    rightClick = false;
  }

}

void setup() {

  ledcSetup(swLeftChannel, freq, resolution);
  ledcSetup(swRightChannel, freq, resolution);

  ledcAttachPin(swLeft, swLeftChannel);
  ledcAttachPin(swRight, swRightChannel);

  ledcWrite(swLeftChannel, 200);
  ledcWrite(swRightChannel, 200);

  leftRel = true;
  rightRel = true;

  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {

  if (leftClick) {
    ledcWrite(swLeftChannel, 0);
    //Serial.println("Left Clicked");
  }
  if (leftRel) {
    ledcWrite(swLeftChannel, 200);
    //Serial.println("Left released");
  }

  if (rightClick) {
    ledcWrite(swRightChannel, 0);
    //Serial.println("Right Clicked");
  }
  if (rightRel) {
    ledcWrite(swRightChannel, 200);
    //Serial.println("Right released");
  }

  //Serial.println(WiFi.macAddress());
  //A0:76:4E:40:11:DC
}
