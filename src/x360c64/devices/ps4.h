#ifndef X360C64_DEVICE_PS4
#define X360C64_DEVICE_PS4

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "controllercontext.h"
#include "tusb.h"

// Sony DS4 report layout detail https://www.psdevwiki.com/ps4/DS4-USB
typedef struct TU_ATTR_PACKED {
    uint8_t x, y, z, rz;  // joystick

    struct {
        uint8_t dpad : 4;      // (hat format, 0x08 is released, 0=N, 1=NE, 2=E,
                               // 3=SE, 4=S, 5=SW, 6=W, 7=NW)
        uint8_t square : 1;    // west
        uint8_t cross : 1;     // south
        uint8_t circle : 1;    // east
        uint8_t triangle : 1;  // north
    };

    struct {
        uint8_t l1 : 1;
        uint8_t r1 : 1;
        uint8_t l2 : 1;
        uint8_t r2 : 1;
        uint8_t share : 1;
        uint8_t option : 1;
        uint8_t l3 : 1;
        uint8_t r3 : 1;
    };

    struct {
        uint8_t ps : 1;       // playstation button
        uint8_t tpad : 1;     // track pad click
        uint8_t counter : 6;  // +1 each report
    };

    // comment out since not used by this example
    // uint8_t l2_trigger; // 0 released, 0xff fully pressed
    // uint8_t r2_trigger; // as above

    //  uint16_t timestamp;
    //  uint8_t  battery;
    //
    //  int16_t gyro[3];  // x, y, z;
    //  int16_t accel[3]; // x, y, z

    // there is still lots more info

} sony_ds4_report_t;

static void process_sony_ds4(uint8_t const* report, uint16_t len,
                      JoyPort_t* joyPortState) {
    uint8_t const report_id = report[0];
    report++;
    len--;

    // all buttons state is stored in ID 1
    if (report_id == 1) {
        sony_ds4_report_t deviceReport;
        memcpy(&deviceReport, report, sizeof(deviceReport));

        joyPortState->dpad = deviceReport.dpad;
        joyPortState->A = deviceReport.circle;
        joyPortState->B = deviceReport.cross;
        joyPortState->X = deviceReport.triangle;

        if (hasChanged(joyPortState->POT1_X,
                       deviceReport.x)) {
            joyPortState->POT1_X = deviceReport.x;
        }

        if (hasChanged(joyPortState->POT1_Y,
                       deviceReport.y)) {
            joyPortState->POT1_Y = deviceReport.y;
        }

        if (hasChanged(joyPortState->POT2_X,
                       deviceReport.z)) {
            joyPortState->POT2_X = deviceReport.z;
        }

        if (hasChanged(joyPortState->POT2_Y,
                       deviceReport.rz)) {
            joyPortState->POT2_Y = deviceReport.rz;
        }
    }
}

#endif