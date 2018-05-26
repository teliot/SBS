#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <MQTTClient.h>
#include "sbs.h"

char ssid[] = "networkName";
char pass[] = "networkPass";

char mqttAddress[] = "test.mosquitto.org";
String mqttTopicBase = "/test/kundarsa/battery/" + WiFi.macAddress() + "/";
String thisVersion = "2018/05/25-19:15";

MQTTClient client;
SBS battery = SBS(0x0b);
unsigned long shortMillis = 0;
unsigned long longMillis = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("and were up!");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  client.begin(mqttAddress, *new WiFiClient());
  client.setWill((mqttTopicBase + "version").c_str(), "OFFLINE", true, 0);
  client.onMessage(messageReceived);
  connect();
  longMillis = millis() + 5000;
}

void loop() {
  if (!client.connected())
    connect();
  client.loop();
  delay(10);

  if (millis() - shortMillis >= 60000) {
    shortMillis = millis();
    client.publish(battery.commands[10].slaveFunction, (String)battery.sbsReadInt(battery.commands[10].code));
    client.publish(battery.commands[13].slaveFunction, (String)battery.sbsReadInt(battery.commands[13].code));
    publishTemp();
  }

  if (millis() - longMillis >= 300000) {
    longMillis = millis();
    dumpAll();
  }
}

void connect() {
  while (WiFi.status() != WL_CONNECTED)
    delay(1000);

  while (!client.connect("weatherStation"));
    delay(1000);

  client.publish(mqttTopicBase + "status", "ONLINE", true, 0);
  client.publish(mqttTopicBase + "version", thisVersion);
  client.subscribe(mqttTopicBase + "update");
}

void update(String file) {
  client.publish(mqttTopicBase + "update", "Updating to file: " + file);
  t_httpUpdate_return ret = ESPhttpUpdate.update(file);
}

void dumpAll() {
  for(int i = 1; i < 38; i++) {
    if(battery.commands[i].bytes == 4) //broken
      client.publish(mqttTopicBase + "rawData/" + battery.commands[i].slaveFunction, (String)battery.sbsReadByte(battery.commands[i].code));
    else if(battery.commands[i].bytes == 2) //works
      client.publish(mqttTopicBase + "rawData/" + battery.commands[i].slaveFunction, (String)battery.sbsReadInt(battery.commands[i].code));
    else if(battery.commands[i].bytes == 3) { //broken
      int n = battery.sbsReadStringSize(battery.commands[i].code);
      if(n>0) {
        char text[++n];
        battery.sbsReadString(text, n-2);
        text[n-1] = '\n';
        client.publish(mqttTopicBase + "rawData/" + battery.commands[i].slaveFunction, (String)text);
      }
      client.publish(mqttTopicBase + "rawData/" + battery.commands[i].slaveFunction + "Size", (String)n);
    }
  }
}

void publishTemp() {
  client.publish(mqttTopicBase + "Temperature", (String)battery.getFahrenheit());
}

void messageReceived(String &topic, String &payload) {
    if (topic == mqttTopicBase + "update") {
      if (payload.substring(0,20)=="http://192.168.1.30/") //trivial but good idea to keep the ending / to help prevent people from doing something similar to http://192.168.1.30.mydomain.tld/exploited.bin
        update(payload);
      if (payload=="dumpAll")
        dumpAll();
      if (payload=="getTemp")
        publishTemp();
    }
  }
