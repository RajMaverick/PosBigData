#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //5.7.0

DHT sensor (A1, DHT11);
const char* ssid     = "IpadRaj";
const char* password = "casa1981";
float t,h;

unsigned long lastTime = 0;

WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(2, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.print("Conectando WIFI: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectada");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  MQTT.setServer("192.168.8.101", 1883);
  MQTT.setCallback(callback);

}

void callback(char* topic, byte* payload, unsigned int length){
  DynamicJsonBuffer jsonBuffer;
  String json = (char*)payload;
  JsonObject& rootRead = jsonBuffer.parseObject(json);
  long s = rootRead["status"];
  long i = rootRead["id"];
  Serial.println(json);
  if(s == 1){
    h = sensor.readHumidity();
    Serial.print (h);
    if (h>50) {
    digitalWrite(2, HIGH);
    Serial.println("Umidade acima de 50%");
    }else {
    digitalWrite(2, LOW);
    Serial.println("Umidade abaixo de 50%");   
    }
  }else{
      t = sensor.readTemperature();
      Serial.print (t);
      if (t>50) {
      digitalWrite(3, HIGH);
      Serial.println("Temperatura acima de 37 graus");
      }else {
      digitalWrite(3, LOW);
      Serial.println("Temperatura abaixo de 37 graus");   
    }
  }
}

void reconnect() {
  while (!MQTT.connected()) {
    if (MQTT.connect("MQTT-Gustavo")) {
      MQTT.subscribe("aula/bigdata");
    } else {
      Serial.print(".");
      delay(3000);
    }
  }
}


void loop() {
  reconnect();
  MQTT.loop();
  unsigned long now = millis();
  if((lastTime + 5000) < now){
    lastTime = now;
    
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["temp"] = t;
    root["humid"] = h;
    root["id"] = "Gustavo";
    String msg;
    root.printTo(msg);
    Serial.println(msg);
    MQTT.publish("temp/real", msg.c_str());
    
  }

}
