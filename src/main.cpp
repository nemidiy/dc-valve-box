#include <Homie.h>
#include <FS.h>

#include <string>
#include <sstream>

#include <valve_controller.h>
#include <homie_handler.h>

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

void setup() {

  Serial.begin(115200);
  Serial << endl << endl;
  
  SPIFFS.begin();

  valve_controller.begin("valve-box","1.0.0");

  Homie.onEvent(on_event);

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