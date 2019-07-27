#ifndef __DIFFCUMULUS_VALVE_CONTROLLER_H__
#define __DIFFCUMULUS_VALVE_CONTROLLER_H__

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
};

} // namespace utils

} // namespace dc
#endif
