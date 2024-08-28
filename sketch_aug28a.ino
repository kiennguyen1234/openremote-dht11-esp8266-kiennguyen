#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi 
const char* ssid = "Test";  // Thay thế bằng SSID của bạn
const char* password = "<x029T43";  // Thay thế bằng mật khẩu WiFi của bạn

// MQTT Broker
const char* mqtt_server = "192.168.137.1";  // Địa chỉ IP của broker
unsigned int mqtt_port = 8883;  // Cổng SSL
const char* username = "master:mqtt_1";  // Tên người dùng MQTT
const char* mqttpass = "Zfsng1zHvGMJtR2FKMaILqlRJ0o54v7D";  // Mật khẩu MQTT
const char* ClientID = "mqtt-explorer-454b4a48";  // ID client MQTT

// DHT Sensor
#define DHTPIN D2  // Chân kết nối với DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFiClientSecure để kết nối SSL
WiFiClientSecure askClient;
PubSubClient client(askClient);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Vô hiệu hóa xác minh chứng chỉ
  askClient.setInsecure();

  // Cấu hình MQTT server
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Kết nối với MQTT
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Đọc dữ liệu từ cảm biến DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Chuyển đổi dữ liệu thành chuỗi để publish lên MQTT
  String temp = String(t);
  String hum = String(h);

  // Gửi dữ liệu nhiệt độ lên topic nhiệt độ
  client.publish("master/mqtt-explorer-454b4a48/writeattributevalue/temperature/6PvSfaoDJCxszD0JGuK8FL", temp.c_str());
  Serial.print("Temperature published: ");
  Serial.println(temp.c_str());

  // Gửi dữ liệu độ ẩm lên topic độ ẩm
  client.publish("master/mqtt-explorer-454b4a48/writeattributevalue/humidity/6PvSfaoDJCxszD0JGuK8FL", hum.c_str());
  Serial.print("Humidity published: ");
  Serial.println(hum.c_str());

  delay(10000);  // Gửi mỗi 10 giây
}

// Hàm callback xử lý tin nhắn đến từ MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
}

// Hàm reconnect để kết nối lại MQTT nếu bị ngắt kết nối
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(ClientID, username, mqttpass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
