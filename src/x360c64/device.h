#ifndef X360C64_DEVICE_
#define X360C64_DEVICE_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "devices/controllercontext.h"
#include "devices/mouse.h"
#include "devices/nintendo.h"
#include "devices/ps4.h"
#include "devices/xbox_360.h"

static char *device_to_string(Device_t device) {
  switch (device) {
    case NINTENDO:
      return "NINTENDO";
    case PS3:
      return "PS3";
    case PS4:
      return "PS4";
    case XBOX_360:
      return "XBOX_360";
    case MOUSE:
      return "MOUSE";

    default:
      return "UNKNOWN";
  }
}

static inline bool is_ps3(uint16_t vid, uint16_t pid) {
  // Batoh Device / PlayStation 3 Controller
  return (vid == 0x054C && pid == 0x0268);
}

static void debug_report(uint8_t const *report, uint16_t len) {
  // we have a pointer to a bunch of bytes, len bytes in fact
  printf("\r\nreport: ");
  for (uint16_t i = 0; i < len; i++) {
    printf("%03u ", *(report + (i * sizeof(uint8_t))));
  }
  printf("\r\n");
  fflush(stdout);
}

// identify device
void x360c64_device_identify(uint8_t dev_addr, uint8_t instance,
                             uint8_t const *desc_report, uint16_t desc_len,
                             JoyPort_t *context) {
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  printf("VID = %04x, PID = %04x\r\n", vid, pid);

  if (is_mouse(vid, pid, dev_addr, instance, desc_report, desc_len)) {
    context->Device = MOUSE;
    return;
  }

  if (is_nintendo(vid, pid)) {
    context->Device = NINTENDO;
    return;
  }

  if (is_sony_ds4(vid, pid)) {
    context->Device = PS4;
    return;
  }

  if (is_xbox_360(vid, pid)) {
    context->Device = XBOX_360;
    return;
  }

  if (is_ps3(vid, pid)) {
    context->Device = PS3;
    return;
  }

  context->Device = UNKNOWN;
}

// get report in standard/normalized struct
void x360c64_device_get_report(uint8_t dev_addr, uint8_t instance,
                               uint8_t const *report, uint16_t len,
                               JoyPort_t *context) {
  switch (context->Device) {
    case PS4:
      process_sony_ds4(report, len, context);
      break;
    case NINTENDO:
      process_nintendo_pro(report, len, context);
      break;
    case XBOX_360:
      process_xbox_360(report, len, context);
      break;
    case MOUSE:
      process_mouse(report, len, context);
      break;

    default:
      debug_report(report, len);
      break;
  }
}

#endif