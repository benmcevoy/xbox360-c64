#ifndef X360C64_DEVICE_
#define X360C64_DEVICE_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// enum for known devices
typedef enum Device { UNKNOWN = 0, PS4, NINTENDO } Device_t;

static inline bool is_sony_ds4(uint16_t vid, uint16_t pid) {
    return (
        (vid == 0x054c && (pid == 0x09cc || pid == 0x05c4))  // Sony DualShock4
        || (vid == 0x0f0d && pid == 0x005e)                  // Hori FC4
        || (vid == 0x0f0d && pid == 0x00ee)  // Hori PS4 Mini (PS4-099U)
        || (vid == 0x1f4f && pid == 0x1002)  // ASW GG xrd controller
    );
}

static inline bool is_nintendo_pro(uint16_t vid, uint16_t pid) {
    // my kmart/anko controller reports itself as many things
    return (
        (vid == 0x057e && pid == 0x2009)     // nin switch controller/pro
        || (vid == 0x2563 && pid == 0x0575)  // Shenzhen Wired Controller
        || (vid == 0x20BC && pid == 0x5500)  // Guangzhou P frostbite controller
    );
}

// identify method
Device_t x360c64_device_identify(uint16_t vid, uint16_t pid) {
    if (is_nintendo_pro(vid, pid)) return NINTENDO;
    if (is_sony_ds4(vid, pid)) return PS4;

    return UNKNOWN;
}

// TODO: get report in standard/normalized struct

#endif