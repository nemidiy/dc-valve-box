/*
Copyright 2019 Nicolas Emiliani

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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

#ifdef ENABLE_VALVE_A
  valve_controller.add_valve(
        IN1,
        IN2,
        TOGGLE_A,
        "A");
#endif

#ifdef ENABLE_VALVE_B
  valve_controller.add_valve(
        IN3,
        IN4,
        TOGGLE_B,
        "B");
#endif

#ifdef ENABLE_FLOAT
  valve_controller.add_float_switch(D5, "float");
#endif

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
  delay(5);
}