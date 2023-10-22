#ifndef X360C64_DEVICE_
#define X360C64_DEVICE_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "devices/controllercontext.h"
#include "devices/nintendo.h"
#include "devices/ps4.h"
#include "devices/xbox_360.h"

// enum for known devices
typedef enum Device { UNKNOWN = 0, PS4, NINTENDO, XBOX_360 } Device_t;

static inline bool is_sony_ds4(uint16_t vid, uint16_t pid) {
    return (
        (vid == 0x054c && (pid == 0x09cc || pid == 0x05c4))  // Sony DualShock4
        || (vid == 0x0f0d && pid == 0x005e)                  // Hori FC4
        || (vid == 0x0f0d && pid == 0x00ee)  // Hori PS4 Mini (PS4-099U)
        || (vid == 0x1f4f && pid == 0x1002)  // ASW GG xrd controller
    );
}

static inline bool is_xbox_360(uint16_t vid, uint16_t pid) {
    return (vid == 0x045E && pid == 0x028E);
}

static inline bool is_nintendo_pro(uint16_t vid, uint16_t pid) {
    // my kmart/anko controller reports itself as many things
    return (
        (vid == 0x057e && pid == 0x2009)     // nin switch controller/pro
        || (vid == 0x2563 && pid == 0x0575)  // Shenzhen Wired Controller
        || (vid == 0x20BC && pid == 0x5500)  // Guangzhou P frostbite controller
    );
}

static void debug_report(uint8_t const* report, uint16_t len) {
    // we have a pointer to a bunch of bytes, len bytes in fact
    printf("report:\r");
    for (uint16_t i = 0; i < len; i++) {
        printf("%u ", *(report + (i * sizeof(uint8_t))));
    }
    fflush(stdout);
}

// identify device
Device_t x360c64_device_identify(uint16_t vid, uint16_t pid) {
    if (is_nintendo_pro(vid, pid)) return NINTENDO;
    if (is_sony_ds4(vid, pid)) return PS4;
    if (is_xbox_360(vid, pid)) return XBOX_360;

    return UNKNOWN;
}

// get report in standard/normalized struct
void x360c64_device_get_report(uint8_t const* report, uint16_t len,
                               uint16_t vid, uint16_t pid, JoyPort_t* context) {
    Device_t device = x360c64_device_identify(vid, pid);

    if (device == PS4) {
        process_sony_ds4(report, len, context);
    }

    if (device == NINTENDO) {
        process_nintendo_pro(report, len, context);
    }

    if (device == XBOX_360) {
        debug_report(report, len);
        //process_xbox_360(report, len, context);
    }

    if (device == UNKNOWN) debug_report(report, len);
}

#endif