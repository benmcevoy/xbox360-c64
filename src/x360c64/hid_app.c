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
#include "device.h"
#include "devices/nintendo.h"
#include "devices/ps4.h"

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

void hid_app_task(void) {
    // nothing to do
}

void debug_report(uint8_t const* report, uint16_t len) {
    // we have a pointer to a bunch of bytes, len bytes in fact
    printf("report:\r");
    for (uint16_t i = 0; i < len; i++) {
        printf("%u ", *(report + (i * sizeof(uint8_t))));
    }
    fflush(stdout);
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

    Device_t device = x360c64_device_identify(vid, pid);

    if (device != UNKNOWN) {
        // request to receive report
        // tuh_hid_report_received_cb() will be invoked when report is available
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

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const* report, uint16_t len) {

    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    Device_t device = x360c64_device_identify(vid, pid);

    if (device == PS4) {
        process_sony_ds4(report, len);
    }

    if (device == NINTENDO) {
        process_nintendo_pro(report, len);
    }

    // continue to request to receive report
    if (!tuh_hid_receive_report(dev_addr, instance)) {
        printf("Error: cannot request to receive report\r\n");
    }
}
