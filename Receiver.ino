#include <WiFi.h>
#include <esp_now.h>

#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

#define TRIG 5
#define ECHO 18

#define PIR 19
#define BUZZER 23

typedef struct {
  char command[20];
} Message;

Message incomingData;

void stopCar() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}

void forward() {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void backward() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void left() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void right() {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

long getDistance() {

  digitalWrite(TRIG,LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG,LOW);

  long duration = pulseIn(ECHO,HIGH);

  long distance = duration * 0.034 / 2;

  return distance;
}

void onReceive(const esp_now_recv_info_t *info,
               const uint8_t *incomingDataBytes,
               int len) {

  memcpy(&incomingData,
         incomingDataBytes,
         sizeof(incomingData));
}

void setup() {

  Serial.begin(115200);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);

  pinMode(PIR,INPUT);

  pinMode(BUZZER,OUTPUT);

  WiFi.mode(WIFI_STA);

  esp_now_init();

  esp_now_register_recv_cb(onReceive);
}

void loop() {

  long distance = getDistance();

  int human = digitalRead(PIR);

  if (distance < 20 || human == HIGH) {

    stopCar();

    digitalWrite(BUZZER,HIGH);

    delay(500);

    digitalWrite(BUZZER,LOW);
  }

  else {

    digitalWrite(BUZZER,LOW);

    if (strcmp(incomingData.command,"FORWARD") == 0)
      forward();

    else if (strcmp(incomingData.command,"BACKWARD") == 0)
      backward();

    else if (strcmp(incomingData.command,"LEFT") == 0)
      left();

    else if (strcmp(incomingData.command,"RIGHT") == 0)
      right();

    else
      stopCar();
  }

  delay(50);
}
