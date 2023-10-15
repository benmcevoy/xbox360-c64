#ifndef X360C64_DEVICE_PS4
#define X360C64_DEVICE_PS4

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
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

// check if different than 2
bool diff_than_2(uint8_t x, uint8_t y) { return (x - y > 2) || (y - x > 2); }

// check if 2 reports are different enough
bool diff_report(sony_ds4_report_t const* rpt1, sony_ds4_report_t const* rpt2) {
    bool result;

    // x, y, z, rz must different than 2 to be counted
    result = diff_than_2(rpt1->x, rpt2->x) || diff_than_2(rpt1->y, rpt2->y) ||
             diff_than_2(rpt1->z, rpt2->z) || diff_than_2(rpt1->rz, rpt2->rz);

    // check the reset with mem compare
    result |=
        memcmp(&rpt1->rz + 1, &rpt2->rz + 1, sizeof(sony_ds4_report_t) - 4);

    return result;
}

void process_sony_ds4(uint8_t const* report, uint16_t len) {
    const char* dpad_str[] = {"N",  "NE", "E",  "SE",  "S",
                              "SW", "W",  "NW", "none"};

    // previous report used to compare for changes
    static sony_ds4_report_t prev_report = {0};

    uint8_t const report_id = report[0];
    report++;
    len--;

    // all buttons state is stored in ID 1
    if (report_id == 1) {
        sony_ds4_report_t ds4_report;
        memcpy(&ds4_report, report, sizeof(ds4_report));

        // counter is +1, assign to make it easier to compare 2 report
        prev_report.counter = ds4_report.counter;

        // only print if changes since it is polled ~ 5ms
        // Since count+1 after each report and  x, y, z, rz fluctuate within 1
        // or 2 We need more than memcmp to check if report is different enough
        if (diff_report(&prev_report, &ds4_report)) {
            printf("(x, y, z, rz) = (%u, %u, %u, %u)\r\n", ds4_report.x,
                   ds4_report.y, ds4_report.z, ds4_report.rz);
            printf("DPad = %s ", dpad_str[ds4_report.dpad]);

            if (ds4_report.square) printf("Square ");
            if (ds4_report.cross) printf("Cross ");
            if (ds4_report.circle) printf("Circle ");
            if (ds4_report.triangle) printf("Triangle ");

            if (ds4_report.l1) printf("L1 ");
            if (ds4_report.r1) printf("R1 ");
            if (ds4_report.l2) printf("L2 ");
            if (ds4_report.r2) printf("R2 ");

            if (ds4_report.share) printf("Share ");
            if (ds4_report.option) printf("Option ");
            if (ds4_report.l3) printf("L3 ");
            if (ds4_report.r3) printf("R3 ");

            if (ds4_report.ps) printf("PS ");
            if (ds4_report.tpad) printf("TPad ");

            printf("\r\n");
        }

        prev_report = ds4_report;
    }
}



#endif