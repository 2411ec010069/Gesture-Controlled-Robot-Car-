#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <MPU6050.h>

MPU6050 mpu;

typedef struct {
  char command[20];
} Message;

Message data;

uint8_t receiverMAC[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};

void setup() {
  Serial.begin(115200);

  Wire.begin(21,22);

  mpu.initialize();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);
}

void loop() {

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);

  if (ay > 7000)
    strcpy(data.command,"FORWARD");
  else if (ay < -7000)
    strcpy(data.command,"BACKWARD");
  else if (ax > 7000)
    strcpy(data.command,"RIGHT");
  else if (ax < -7000)
    strcpy(data.command,"LEFT");
  else
    strcpy(data.command,"STOP");

  esp_now_send(receiverMAC,(uint8_t *)&data,sizeof(data));

  delay(100);
}
