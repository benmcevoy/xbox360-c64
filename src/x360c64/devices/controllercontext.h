#ifndef X360C64_CONTROLLERCONTEXT_
#define X360C64_CONTROLLERCONTEXT_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "tusb.h"

typedef struct TU_ATTR_PACKED
{
    // buttons  XBOBX style not nintendo
    struct
    {
        // primary fire
        uint8_t A : 1;
        uint8_t B : 1;
        uint8_t X : 1;
        uint8_t Y : 1;
        // U=0, R=2, D=4, L=6
        uint8_t dpad : 4;
    };

    // analog sticks 128 is center. U=0 D=255 L=0 R=255
    uint8_t POT1_X, POT1_Y, POT2_X, POT2_Y;

    // housekeeping
    uint8_t counter;

    bool IsConnected;

} JoyPort_t;

static uint8_t _threshold = 3;
static bool hasChanged(uint8_t x, uint8_t y) { return (x - y > _threshold) || (y - x > _threshold); }

#endif