/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board.h"
#include "tusb.h"

/* From https://www.kernel.org/doc/html/latest/input/gamepad.html
          ____________________________              __
         / [__ZL__]          [__ZR__] \               |
        / [__ TL __]        [__ TR __] \              | Front Triggers
     __/________________________________\__         __|
    /                                  _   \          |
   /      /\           __             (N)   \         |
  /       ||      __  |MO|  __     _       _ \        | Main Pad
 |    <===DP===> |SE|      |ST|   (W) -|- (E) |       |
  \       ||    ___          ___       _     /        |
  /\      \/   /   \        /   \     (S)   /\      __|
 /  \________ | LS  | ____ |  RS | ________/  \       |
|         /  \ \___/ /    \ \___/ /  \         |      | Control Sticks
|        /    \_____/      \_____/    \        |    __|
|       /                              \       |
 \_____/                                \_____/

     |________|______|    |______|___________|
       D-Pad    Left       Right   Action Pad
               Stick       Stick

                 |_____________|
                    Menu Pad

  Most gamepads have the following features:
  - Action-Pad 4 buttons in diamonds-shape (on the right side) NORTH, SOUTH,
WEST and EAST.
  - D-Pad (Direction-pad) 4 buttons (on the left side) that point up, down, left
and right.
  - Menu-Pad Different constellations, but most-times 2 buttons: SELECT - START.
  - Analog-Sticks provide freely moveable sticks to control directions,
Analog-sticks may also provide a digital button if you press them.
  - Triggers are located on the upper-side of the pad in vertical direction. The
upper buttons are normally named Left- and Right-Triggers, the lower buttons
Z-Left and Z-Right.
  - Rumble Many devices provide force-feedback features. But are mostly just
simple rumble motors.
 */

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

typedef struct TU_ATTR_PACKED {
    // this is "packed"
    // e.g. d-pad is 3 bits of a byte

    // b0: buttons =  A:1, B:2, x:4, rt: 128, lt: 64
    // b1: buttons: +:8 -:4 zr:2 zl:1, left analog stick push: 32  right analog stick push: 64
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
        uint8_t dpad : 4;    // (hat format, 0x08 is released, 0=N, 1=NE, 2=E,
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

    // comment out since not used by this example
    // uint8_t l2_trigger; // 0 released, 0xff fully pressed
    // uint8_t r2_trigger; // as above

    //  uint16_t timestamp;
    //  uint8_t  battery;
    //
    //  int16_t gyro[3];  // x, y, z;
    //  int16_t accel[3]; // x, y, z

    // there is still lots more info

} nintendo_pro_report_t;

// check if device is Sony DualShock 4
static inline bool is_sony_ds4(uint8_t dev_addr) {
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (
        (vid == 0x054c && (pid == 0x09cc || pid == 0x05c4))  // Sony DualShock4
        || (vid == 0x0f0d && pid == 0x005e)                  // Hori FC4
        || (vid == 0x0f0d && pid == 0x00ee)  // Hori PS4 Mini (PS4-099U)
        || (vid == 0x1f4f && pid == 0x1002)  // ASW GG xrd controller
    );
}

static inline bool is_nintendo_pro(uint8_t dev_addr) {
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    // my kmart/anko controller reports itself as many things
    return ((vid == 0x057e && pid == 0x2009)  // nin switch controller/pro
            || (vid == 0x2563 &&
                pid == 0x0575)  // Shenzhen ShanWan Technology Co., Ltd. ZD-V+
                                // Wired Gaming Controller
            || (vid == 0x20BC &&
                pid == 0x5500)  // Guangzhou Pinzhong Electronic Technology CO.,
                                // LTD Frostbite controller

    );
}

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

void hid_app_task(void) {
    // nothing to do
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const* desc_report, uint16_t desc_len) {
    (void)desc_report;
    (void)desc_len;
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr,
           instance);
    printf("VID = %04x, PID = %04x\r\n", vid, pid);

    // Sony DualShock 4 [CUH-ZCT2x]
    if (is_sony_ds4(dev_addr)) {
        // request to receive report
        // tuh_hid_report_received_cb() will be invoked when report is available
        if (!tuh_hid_receive_report(dev_addr, instance)) {
            printf("Error: cannot request to receive report\r\n");
        }
    }

    if (is_nintendo_pro(dev_addr)) {
        if (!tuh_hid_receive_report(dev_addr, instance)) {
            printf("Error: cannot request to receive report\r\n");
        }
    }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr,
           instance);
}

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

void debug_report(uint8_t const* report, uint16_t len) {
    // we have a pointer to a bunch of bytes, len bytes in fact
    printf("report:\r");
    for (uint16_t i = 0; i < len; i++) {
        printf("%u ", *(report + (i * sizeof(uint8_t))));
    }
    fflush(stdout);
}

void process_nintendo_pro(uint8_t const* report, uint16_t len) {
    const char* dpad_str[] = {"N",  "NE", "E",  "SE",  "S",
                              "SW", "W",  "NW", "none"};

    // previous report used to compare for changes
    static nintendo_pro_report_t prev_report = {0};

    uint8_t const report_id = report[0];

    nintendo_pro_report_t nin_report;
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

void process_sony_ds4(uint8_t const* report, uint16_t len) {
    const char* dpad_str[] = {"N",  "NE", "E",  "SE",  "S",
                              "SW", "W",  "NW", "none"};

    debug_report(report, len);
    return;

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

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const* report, uint16_t len) {
    if (is_sony_ds4(dev_addr)) {
        process_sony_ds4(report, len);
    }

    if (is_nintendo_pro(dev_addr)) {
        process_nintendo_pro(report, len);
    }

    // continue to request to receive report
    if (!tuh_hid_receive_report(dev_addr, instance)) {
        printf("Error: cannot request to receive report\r\n");
    }
}
