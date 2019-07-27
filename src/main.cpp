#include <Homie.h>
#include <FS.h>

#include <string>
#include <sstream>

#include <valve_controller.h>

#define IN1 D1
#define IN2 D2

#define IN3 D7
#define IN4 D8

#define TOGGLE_A D5
#define TOGGLE_B D6

// WIFI IP
std::string ip;

// valve controller
dc::utils::TwoWireValveController valve_controller;

auto on_event = [](const HomieEvent& event){
      switch(event.type) {
        case HomieEventType::STANDALONE_MODE:
          // Do whatever you want when standalone mode is started
          Serial.println("HomieEventType::STANDALONE_MODE");
          break;
        case HomieEventType::CONFIGURATION_MODE:
          // Do whatever you want when configuration mode is started
          Serial.println("HomieEventType::CONFIGURATION_MODE");
          break;
        case HomieEventType::NORMAL_MODE:
          // Do whatever you want when normal mode is started
          Serial.println("HomieEventType::NORMAL_MODE");
          break;
        case HomieEventType::OTA_STARTED:
          // Do whatever you want when OTA is started
          Serial.println("HomieEventType::OTA_STARTED");
          break;
        case HomieEventType::OTA_PROGRESS:
          // Do whatever you want when OTA is in progress
          Serial.println("HomieEventType::OTA_PROGRESS");
          // You can use event.sizeDone and event.sizeTotal
          break;
        case HomieEventType::OTA_FAILED:
          // Do whatever you want when OTA is failed
          Serial.println("HomieEventType::OTA_FAILED");
          break;
        case HomieEventType::OTA_SUCCESSFUL:
          // Do whatever you want when OTA is successful
          Serial.println("HomieEventType::OTA_SUCCESSFUL");
          break;
        case HomieEventType::ABOUT_TO_RESET:
          // Do whatever you want when the device is about to reset
          Serial.println("HomieEventType::ABOUT_TO_RESET");
          break;
        case HomieEventType::WIFI_CONNECTED:
          // Do whatever you want when Wi-Fi is connected in normal mode
          Serial.println("HomieEventType::WIFI_CONNECTED");
          // You can use event.ip, event.gateway, event.mask
          break;
        case HomieEventType::WIFI_DISCONNECTED:
          // Do whatever you want when Wi-Fi is disconnected in normal mode
          Serial.println("HomieEventType::WIFI_DISCONNECTED");
          // You can use event.wifiReason
          break;
        case HomieEventType::MQTT_READY:
          // Do whatever you want when MQTT is connected in normal mode
          Serial.println("HomieEventType::MQTT_READY");
          break;
        case HomieEventType::MQTT_DISCONNECTED:
          // Do whatever you want when MQTT is disconnected in normal mode
          Serial.println("HomieEventType::MQTT_DISCONNECTED");
          // You can use event.mqttReason
          break;
        case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
          // Do whatever you want when an MQTT packet with QoS > 0 is acknowledged by the broker
          //Serial.println("HomieEventType::MQTT_PACKET_ACKNOWLEDGED");
          // You can use event.packetId
          break;
        case HomieEventType::READY_TO_SLEEP:
          // After you've called `prepareToSleep()`, the event is triggered when MQTT is disconnected
          Serial.println("HomieEventType::READY_TO_SLEEP");
          break;
        }
  };


void setup() {

  Serial.begin(115200);
  Serial << endl << endl;
  
  SPIFFS.begin();

  valve_controller.begin("valve-box","1.0.0");

  valve_controller.add_valve(
        IN1,
        IN2,
        TOGGLE_A,
        "A");

  valve_controller.add_valve(
        IN3,
        IN4,
        TOGGLE_B,
        "B");

  Homie.onEvent(on_event);

  Homie.setup();

  WiFi.begin(
      HomieInternals::Interface::get().getConfig().get().wifi.ssid,
      HomieInternals::Interface::get().getConfig().get().wifi.password);  
}

void loop() {
  Homie.loop();
  ip = WiFi.localIP().toString().c_str();
  valve_controller.loop();
  delay(10);
}