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

#ifdef ENABLE_ATLAS_FLOW
#include <Wire.h>
#include <device.h>
#include <device_manager.h>
#endif

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

#ifdef ENABLE_ATLAS_FLOW
// Device manager
dc::atlas::DeviceManager device_manager;
dc::atlas::DeviceManager* dm = &device_manager;
dc::atlas::DeviceManager* t = dc::atlas::DeviceManager::get_instance(dm);
#endif

void loopHandler() {
  valve_controller.loop();
#ifdef ENABLE_ATLAS_FLOW
  device_manager.loop();
#endif
}

void setup() {

  Serial.begin(115200);
  Serial << endl << endl;

#ifdef ENABLE_ATLAS_FLOW
  //enable I2C port.
  Wire.begin();
  //auto discovery
  device_manager.auto_discovery();
#endif

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

#ifdef ENABLE_ATLAS_FLOW
  valve_controller.add_flow_totalizer(
      device_manager.get_all_devs()[dc::atlas::Device::FLOW_SENSOR],
      "flow");
#endif

  Homie.setLoopFunction(loopHandler);

  Homie.onEvent(on_event);

  Homie.setup();

  WiFi.begin(
      HomieInternals::Interface::get().getConfig().get().wifi.ssid,
      HomieInternals::Interface::get().getConfig().get().wifi.password);
}

void loop(){
  Homie.loop();
  delay(100);
}

/*
    Wire.beginTransmission(104);
    Wire.write("R\0");
    Wire.endTransmission();
    delay(500);
    Wire.requestFrom(104, 20, 1);
    //the first byte is the response code, we read this separately.
    byte code = Wire.read();
    char buffer[20];
    if(code == 1){
      Serial.println("Success");
      byte i = 0;
      while (Wire.available()) {
        buffer[i] = Wire.read();
        if (buffer[i] == 0){
          Wire.endTransmission();
          break;
        }
        ++i;
      }
      Serial.print("Response : ");
      Serial.println(buffer);
    }else if (code == 2){
      Serial.println("Failure");
    }else if (code == 254){
      Serial.println("Not finished");
    }else if (code == 255){
      Serial.println("No data");
    }
  delay(1000);
*/