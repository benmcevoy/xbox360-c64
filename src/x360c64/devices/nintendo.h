#ifndef X360C64_DEVICE_NINTENDO
#define X360C64_DEVICE_NINTENDO

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "controllercontext.h"
#include "tusb.h"

typedef struct TU_ATTR_PACKED
{
    // this is "packed".
    // e.g. d-pad is 3 bits of a byte

    // b0: buttons =  A:1, B:2, x:4, rt: 128, lt: 64
    // b1: buttons: +:8 -:4 zr:2 zl:1, left analog stick push: 32  right analog
    // stick push: 64
    struct
    {
        uint8_t A : 1;
        uint8_t B : 1;
        uint8_t X : 1;
        uint8_t Y : 1;
        uint8_t ignore0 : 2;
        uint8_t RT : 1;
        uint8_t LT : 1;
    };

    struct
    {
        uint8_t ZL : 1;
        uint8_t ZR : 1;
        uint8_t minus : 1;
        uint8_t plus : 1;
        uint8_t ignore1 : 1;
        uint8_t LA : 1;
        uint8_t RA : 1;
        uint8_t ignore2 : 1;
    };

    // byte 2 is d-pad
    struct
    {
        uint8_t dpad : 4;    // (hat format, 0x08 is released, 0=N, 1=NE, 2=E,
                             // 3=SE, 4=S, 5=SW, 6=W, 7=NW)
        uint8_t ignore3 : 4; // not used
    };

    // analog sticks
    // bytes 3,4,5,6
    // 3: left analog left-right axis:  0..255 128 is centered
    // 4: left analog up-down axis:  0..255 128 is centered
    // 5: right analog left-right axis:  0..255 128 is centered
    // 6: right analog up-down axis:  0..255 128 is centered

    uint8_t left_analog_left_right, left_analog_up_down,
        right_analog_left_right, right_analog_up_down;

} nintendo_report_t;

static inline bool is_nintendo_pro(uint16_t vid, uint16_t pid) {
  return ((vid == 0x057e && pid == 0x2009)  // nin switch controller/pro
  );
}

static inline bool is_nintendo(uint16_t vid, uint16_t pid) {
  // my kmart/anko controller reports itself as many things
  return (is_nintendo_pro(vid, pid) ||
          (vid == 0x2563 && pid == 0x0575)  // Shenzhen Wired Controller
          ||
          (vid == 0x20BC && pid == 0x5500)  // Guangzhou P frostbite controller
          || (vid == 0x0F0D && pid == 0x00C1)  // Horipad
          || (vid == 0x0F0D && pid == 0x0092)  // Horpad pokken
          || (vid == 0x2563 && pid == 0x0575)  // ZD-V+ Wired Gaming Controller
  );
}

static void process_nintendo_pro(uint8_t const *report, uint16_t len,
                                 JoyPort_t *joyPortState)
{
    nintendo_report_t deviceReport;
    memcpy(&deviceReport, report, sizeof(deviceReport));

    joyPortState->dpad = deviceReport.dpad;
    joyPortState->A = deviceReport.B;
    joyPortState->B = deviceReport.A;
    // TODO: my Y button does not seem to work? maybe my contorller is broken
    joyPortState->X = deviceReport.Y;
    joyPortState->Y = deviceReport.X;

    if (hasChanged(joyPortState->POT1_X,
                   deviceReport.left_analog_left_right))
    {
        joyPortState->POT1_X = deviceReport.left_analog_left_right;
    }

    if (hasChanged(joyPortState->POT1_Y, deviceReport.left_analog_up_down))
    {
        joyPortState->POT1_Y = deviceReport.left_analog_up_down;
    }

    if (hasChanged(joyPortState->POT2_X,
                   deviceReport.right_analog_left_right))
    {
        joyPortState->POT2_X = deviceReport.right_analog_left_right;
    }

    if (hasChanged(joyPortState->POT2_Y, deviceReport.right_analog_up_down))
    {
        joyPortState->POT2_Y = deviceReport.right_analog_up_down;
    }
}

#endif