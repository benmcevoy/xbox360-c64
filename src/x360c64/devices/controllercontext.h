#ifndef X360C64_CONTROLLERCONTEXT_
#define X360C64_CONTROLLERCONTEXT_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "tusb.h"

typedef struct TU_ATTR_PACKED {
    // buttons
    struct {
        uint8_t A : 1;
        uint8_t B : 1;
        uint8_t X : 1;
        uint8_t not_used : 1;
        uint8_t dpad : 4;
    };

    // analog sticks
    uint8_t POT1_X, POT1_Y, POT2_X, POT2_Y;

    // housekeeping
    uint8_t counter;

} JoyPort_t;

static bool hasChanged(uint8_t x, uint8_t y) { return (x - y > 2) || (y - x > 2); }


#endif