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

#include "valve_controller.h"
#include <sstream>

using namespace dc::utils;

TwoWireValveController::TwoWireValveController(){
}

TwoWireValveController::~TwoWireValveController(){
}

void TwoWireValveController::begin(
        const std::string& product, const std::string& fw_version){
    Homie_setFirmware("valve-box", "1.0.0");
}

bool TwoWireValveController::add_valve(
        uint8_t pin_a,
        uint8_t pin_b,
        uint8_t pin_toggle,
        const std::string& name){

    pinMode(pin_a, OUTPUT);
    pinMode(pin_b, OUTPUT);
    pinMode(pin_toggle, INPUT);

    auto conf = new valve_config;
    conf->pin_a = pin_a;
    conf->pin_b = pin_b;
    conf->pin_toggle = pin_toggle;

    std::ostringstream os;
    os << "VALVE_" << name;
    conf->valve_file = os.str();
    Serial.print("valve conf file : ");
    Serial.println(os.str().c_str());
    valves.insert(std::make_pair(name, conf));

    HomieNode* valve_node = new HomieNode(name.c_str(), "switch");
    nodes.insert(make_pair(name, valve_node));

    auto handler = [conf, this, valve_node](
            const HomieRange& range, const String& value) -> bool{
        Serial.print("valve handler ");
        Serial.println(conf->valve_file.c_str());
        if (value != "true" && value != "false")
          return false;
        bool on = (value == "true");
        if(on){
          Serial.println("\t-> true");
          conf->pin_a_state = conf->pin_a_open_state;
          conf->pin_b_state = !conf->pin_a_open_state;
          
        }else{
          Serial.println("\t-> false");
          conf->pin_a_state = !conf->pin_a_open_state;
          conf->pin_b_state = conf->pin_a_open_state;
        }
        Serial.print("\t-> ");
        Serial.print(conf->pin_a_state);
        Serial.println(conf->pin_b_state);
        bool res = this->set_status(
            conf->valve_file.c_str(), conf->pin_a_state, conf->pin_b_state, on);
        digitalWrite(conf->pin_a, conf->pin_a_state);
        digitalWrite(conf->pin_b, conf->pin_b_state);
        valve_node->setProperty("open").send(value);
        return res;
    };

    valve_node->advertise("open").settable(handler);

    std::ostringstream fn;
    fn << "VALVE " << name;
    //check if we have status files or not.
    if(SPIFFS.exists(conf->valve_file.c_str())){
      // we have status for valve A
      get_status(conf->valve_file.c_str(), conf->pin_a_state, conf->pin_b_state, conf->open);
      if(conf->open){
        fn << " is open";
        Serial.println(fn.str().c_str());
        conf->pin_a_open_state = conf->pin_a_state;
      }else{
        fn << " is closed";
        Serial.println(fn.str().c_str());
        conf->pin_a_open_state = !conf->pin_a_state;
      }
    }else{
      Serial.print("No file present for ");
      Serial.println(name.c_str());
    }

    digitalWrite(conf->pin_a, conf->pin_a_state);
    digitalWrite(conf->pin_b, conf->pin_b_state);

    return true;
}

void TwoWireValveController::loop(){

    for (auto& kv: valves){
        auto conf = kv.second;
        uint8_t toggle_state = digitalRead(conf->pin_toggle);
        // check if the pushbutton is pressed.
        // if it is, the buttonState is HIGH
        if(millis() - conf->switch_time > 5000)
          conf->switch_time = 0;
        
        if(toggle_state == HIGH && !conf->switch_time){
          conf->switch_time = millis();
          //move it to open
          conf->pin_a_state = !conf->pin_a_state;
          conf->pin_b_state = !conf->pin_b_state;
          conf->pin_a_open_state = !conf->pin_a_open_state;
          digitalWrite(conf->pin_a, conf->pin_a_state);
          digitalWrite(conf->pin_b, conf->pin_b_state);
          set_status(conf->valve_file.c_str(), conf->pin_a_state, conf->pin_b_state, true);
        }
    }
}

bool TwoWireValveController::get_status(
        const char* fname, uint8_t& pin_a, uint8_t& pin_b, bool& status){

    status = false;
    pin_a = LOW;
    pin_b = LOW;

    auto f = SPIFFS.open(fname, "r");
    if(!f) {
      Serial.println("ERROR : file open failed");
      return false;
    }
    //read first byte
    char buff[4];
    memset(buff, 0, 4);

    size_t bytes = f.readBytes(buff, 3);
    if(bytes != 3){
      Serial.println(bytes);
      Serial.println(buff);
      Serial.println("ERROR : bad format, overrirding to 101");
      status = true;
      pin_a = LOW;
      pin_b = HIGH;
      f.close();
      set_status(fname, pin_a, pin_b, status);
      return false;
    }

    Serial.println(buff);
    if(buff[0] == '1'){
      status = true;
    }
    if(buff[1] == '1'){
      pin_a = HIGH;
    }
    if(buff[2] == '1'){
      pin_b = HIGH;
    }
    f.close();
    return true;
}

bool TwoWireValveController::set_status(
        const char* fname, uint8_t pin_a, uint8_t pin_b, bool open){
    auto f = SPIFFS.open(fname, "w");
    std::ostringstream os;
    if(!f) {
      Serial.println("ERROR : file open failed");
      return false;
    }
    if(open)
      os << "1";
    else
      os << "0";
    os << (int)pin_a << (int)pin_b;
    Serial.print("\t-> SETTING ");
    Serial.println(os.str().c_str());
    f.print(os.str().c_str());
    f.close();
    return true;
}

std::string TwoWireValveController::state_to_char(int state){
    if(state == HIGH)
        return "1";
    return "0";
}