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

#ifndef __DC_VALVE_CONTROLLER_H__
#define __DC_VALVE_CONTROLLER_H__

#include <FS.h>
#include <Homie.h>
#include <map>
#include <string>

namespace dc {

namespace utils {

struct TwoWireValveController {

    TwoWireValveController();

    virtual ~TwoWireValveController();

    void begin(const std::string& product, const std::string& fw_version);

    bool add_valve(
        uint8_t pin_a,
        uint8_t pin_b,
        uint8_t pin_toggle,
        const std::string& name);

    void loop();

protected :

    bool get_status(const char* fname, uint8_t& pin_a, uint8_t& pin_b, bool& status);

    bool set_status(const char* fname, uint8_t pin_a, uint8_t pin_b, bool open);

    static std::string state_to_char(int state);

    typedef struct {
        uint8_t pin_a; //id of pin a
        uint8_t pin_b; //id of pin b
        uint8_t pin_toggle; //id of toggle pin
        uint8_t pin_a_state; // state of pin a
        uint8_t pin_b_state; // state of pin b
        uint8_t pin_a_open_state; //value of pin a when valve is open
        std::string valve_file; // valve filename to store state
        bool open; // if open then true
        unsigned long int switch_time; // time when toggle button was pressed
    } valve_config;

    std::map<std::string, HomieNode*> nodes;
    std::map<std::string, valve_config*> valves;

    unsigned long int last_sent = 0;
    unsigned long int INTERVAL = 10;
};

} // namespace utils

} // namespace dc
#endif
