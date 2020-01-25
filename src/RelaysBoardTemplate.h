#pragma once

#include <array>
#include <ExpansionBoard.h>
#include "RelayTypes.h"

template <uint8_t relays_count>
struct RelaysBoardTemplate {
    constexpr static const uint8_t RELAYS_COUNT{relays_count};

    using RelayActuationType = std::array<Relay::Actuation, relays_count>;

    explicit RelaysBoardTemplate(ExpansionBoard &sink);

    void setup();

    void setup(uint16_t initial_state_flags);

    uint16_t flags();

    void actuate(const RelayActuationType &action);

protected:
    ExpansionBoard &sink;

    using RelaysPinMappingType = std::array<uint8_t, RELAYS_COUNT>;
    RelaysPinMappingType RELAY_ID_TO_PIN_MAPPING;

    constexpr static const uint8_t ENERGIZE{1};
    constexpr static const uint8_t RELEASE{0};
    using RelayActuationMappingType = std::array<uint8_t, 2>;
    RelayActuationMappingType RELAY_ACTION_MAPPING;

    void energize(uint_fast8_t relay_id);

    void release(uint_fast8_t relay_id);

    void invert(uint_fast8_t relay_id);

    void leaveUnchanged(uint_fast8_t relay_id);

    void writeToHardware(uint8_t relay_id, uint8_t digital_pin_value);

    uint8_t readFromHardware(uint8_t relay_id);
};

template <uint8_t T>
RelaysBoardTemplate<T>::RelaysBoardTemplate(ExpansionBoard &sink) :
    sink(sink),
    RELAY_ID_TO_PIN_MAPPING {
            0, // first relay maps to pin 0
            1, // second relay maps to pin 1
            2,
            3,
            4,
            5,
            6,
            7 // seventh relay maps to pin 0
    },
    RELAY_ACTION_MAPPING {
            ENERGIZE, //Relay::Actuation::Release means to ENERGIZE the pin
            RELEASE //Relay::Actuation::Activate means to RELEASE the pin
    }
{
}

template <uint8_t T>
void RelaysBoardTemplate<T>::setup() {}

template <uint8_t T>
void RelaysBoardTemplate<T>::setup(uint16_t initial_state_flags) {
    sink.write(initial_state_flags);
    setup();
}

template <uint8_t T>
void RelaysBoardTemplate<T>::actuate(const RelayActuationType &action) {
    Serial.printf("RelaysBoardTemplate::actuate: [");
    String delimiter{""};

    uint8_t relay_id = 0;
    for (const auto &a : action) {

        uint8_t relay_action = Relay::uint8FromActuation(a);
        uint8_t mapped_action = RELAY_ACTION_MAPPING[relay_action];
        uint8_t p = RELAY_ID_TO_PIN_MAPPING[relay_id];
        Serial.printf("%s[%d]=%d", delimiter.c_str(), p, mapped_action);
        if (delimiter.length() == 0)
            delimiter += ", ";

        switch (a) {
            case Relay::Actuation::Activate:
                energize(relay_id);
                break;

            case Relay::Actuation::Release:
                release(relay_id);
                break;

            case Relay::Actuation::Invert:
                invert(relay_id);
                break;

            case Relay::Actuation::NoAction:
                leaveUnchanged(relay_id);
                break;
        }
        relay_id++;
    }
    Serial.printf("]\n");
}

template <uint8_t T>
void RelaysBoardTemplate<T>::energize(uint_fast8_t relay_id) {
    uint8_t relay_action = Relay::uint8FromActuation(Relay::Actuation::Activate);
    uint8_t hardware_action = RELAY_ACTION_MAPPING[relay_action];
    writeToHardware(relay_id, hardware_action);
}

template <uint8_t T>
void RelaysBoardTemplate<T>::release(uint_fast8_t relay_id) {
    uint8_t relay_action = Relay::uint8FromActuation(Relay::Actuation::Release);
    uint8_t hardware_action = RELAY_ACTION_MAPPING[relay_action];
    writeToHardware(relay_id, hardware_action);
}

template <uint8_t T>
void RelaysBoardTemplate<T>::invert(uint_fast8_t relay_id) {
    if (readFromHardware((relay_id)) != 0) {
        energize(relay_id);
    } else {
        release(relay_id);
    }
}

template <uint8_t T>
void RelaysBoardTemplate<T>::leaveUnchanged(uint_fast8_t) {
    // noop
}

template <uint8_t T>
void RelaysBoardTemplate<T>::writeToHardware(uint8_t relay_id, uint8_t digital_pin_value) {
    uint8_t hardware_pin = RELAY_ID_TO_PIN_MAPPING[relay_id];
    sink.digitalWrite(hardware_pin, digital_pin_value);
}

template <uint8_t T>
uint8_t RelaysBoardTemplate<T>::readFromHardware(uint8_t relay_id) {
    uint8_t hardware_pin = RELAY_ID_TO_PIN_MAPPING[relay_id];
    return sink.digitalRead(hardware_pin);
}

template <uint8_t T>
uint16_t RelaysBoardTemplate<T>::flags() {
    return sink.read();
}
