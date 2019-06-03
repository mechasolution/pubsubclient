#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

//네트워크 설정
const char* ssid = "사용자의 WIFI이름을 입력해주세요";
const char* password = "WIFI의 비밀번호를 입력해주세요";
const char* mqtt_server = "브로커의 IP를 입력해주세요";

long lastMsg = 0;

char topic[] = "outTopic";

void setup_wifi() { // 와이파이를 연결하는 함수
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() { //브로커와 연결이 끊기면 재연결해주는 함수
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish(topic, "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  randomSeed(micros()); //reconnect() 함수에서 클라이언트 ID를 생성할 때 사용되는 랜덤시드를 설정

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    client.publish(topic, "Hello MQTT"); // 토픽에 메세지 버퍼를 발행해주는 함수
  }
}
