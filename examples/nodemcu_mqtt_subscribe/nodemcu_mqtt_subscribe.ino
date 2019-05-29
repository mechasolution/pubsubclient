#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "사용자의 WIFI이름을 입력해주세요";
const char* password = "WIFI의 비밀번호를 입력해주세요";
const char* mqtt_server = "브로커의 IP를 입력해주세요";

WiFiClient espClient;
PubSubClient client(espClient);

String subscribe_message = "";
char topic[] = "inTopic";

void setup_wifi() { //와이파이를 연결하는 함수
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

// 구독한 토픽에서 메세지가 발행되면 데이터를 받아와 처리해주는 함수
void callback(char* topic, byte* payload, unsigned int length) {
  subscribe_message = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  for (int i = 0; i < length; i++) {
    subscribe_message += (char)payload[i]; //문자열에 전달된 메세지를 저장
  }
  Serial.println(subscribe_message);
}

void reconnect() { //브로커와 연결이 끊기면 재연결해주는 함수
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic);
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
  client.setCallback(callback); //메세지가 입력되면 처리해줄 콜백함수를 설정
  randomSeed(micros()); //reconnect() 함수에서 클라이언트 ID를 생성할 때 사용되는 랜덤시드를 설정
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  client.subscribe(topic); //Subscribe()를 사용해 토픽을 구독한다
}
