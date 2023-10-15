#ifndef X360C64_DEVICE_NINTENDO
#define X360C64_DEVICE_NINTENDO

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "tusb.h"

typedef struct TU_ATTR_PACKED {
    // this is "packed".
    // e.g. d-pad is 3 bits of a byte

    // b0: buttons =  A:1, B:2, x:4, rt: 128, lt: 64
    // b1: buttons: +:8 -:4 zr:2 zl:1, left analog stick push: 32  right analog
    // stick push: 64
    struct {
        uint8_t A : 1;
        uint8_t B : 1;
        uint8_t X : 1;
        uint8_t ignore0 : 3;
        uint8_t RT : 1;
        uint8_t LT : 1;
    };
    struct {
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
    struct {
        uint8_t dpad : 4;     // (hat format, 0x08 is released, 0=N, 1=NE, 2=E,
                              // 3=SE, 4=S, 5=SW, 6=W, 7=NW)
        uint8_t ignore3 : 4;  // not used
    };

    // analog sticks
    // bytes 3,4,5,6
    // 3: left analog left-right axis:  0..255 128 is centered
    // 4: left analog up-down axis:  0..255 128 is centered
    // 5: right analog left-right axis:  0..255 128 is centered
    // 6: right analog up-down axis:  0..255 128 is centered

    uint8_t left_analog_left_right, left_analog_up_down,
        right_analog_left_right, right_analog_up_down;

    // housekeeping
    uint8_t counter;

} DeviceReport_t;


void process_nintendo_pro(uint8_t const* report, uint16_t len) {
    const char* dpad_str[] = {"N",  "NE", "E",  "SE",  "S",
                              "SW", "W",  "NW", "none"};

    // previous report used to compare for changes
    static DeviceReport_t prev_report = {0};

    uint8_t const report_id = report[0];

    DeviceReport_t nin_report;
    memcpy(&nin_report, report, sizeof(nin_report));

    // counter is +1, assign to make it easier to compare 2 report
    prev_report.counter = nin_report.counter;

    // TODO: do I care about diff? good for analog
    // only print if changes since it is polled ~ 5ms
    // Since count+1 after each report and  x, y, z, rz fluctuate within 1
    // or 2 We need more than memcmp to check if report is different enough
    // if (diff_report(&prev_report, &nin_report)) {

    printf("(la_lr, la_up, ra_lr, ra_ud) = (%u, %u, %u, %u)",
           nin_report.left_analog_left_right, nin_report.left_analog_up_down,
           nin_report.right_analog_left_right, nin_report.right_analog_up_down);
    printf("DPad = %s ", dpad_str[nin_report.dpad]);

    if (nin_report.A) printf("A ");
    if (nin_report.B) printf("B ");
    if (nin_report.X) printf("X ");
    // if (nin_report.triangle) printf("Triangle ");

    // if (nin_report.l1) printf("L1 ");
    // if (nin_report.r1) printf("R1 ");
    // if (nin_report.l2) printf("L2 ");
    // if (nin_report.r2) printf("R2 ");

    // if (nin_report.share) printf("Share ");
    // if (nin_report.option) printf("Option ");
    // if (nin_report.l3) printf("L3 ");
    // if (nin_report.r3) printf("R3 ");

    // if (nin_report.ps) printf("PS ");
    // if (nin_report.tpad) printf("TPad ");

    printf("\r");
    fflush(stdout);

    prev_report = nin_report;
}

#endif