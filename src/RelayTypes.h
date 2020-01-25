#pragma once

#include <inttypes.h>

struct Relay {
    enum class Actuation : uint8_t {
        Release, Activate, Invert, NoAction
    };

    static uint8_t uint8FromActuation(Actuation ra) {
        return static_cast<std::underlying_type<Actuation>::type>(ra);
    }

    static Actuation actuationFromUint8(uint8_t ra) {
        return static_cast<Actuation>(ra);
    }
};
