#ifndef X360C64_DEVICE_MOUSE
#define X360C64_DEVICE_MOUSE

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "controllercontext.h"
#include "tusb.h"

#define MAX_REPORT 4

// Each HID instance can has multiple reports
static struct {
  uint8_t report_count;
  tuh_hid_report_info_t report_info[MAX_REPORT];
} hid_info[CFG_TUH_HID];

typedef struct TU_ATTR_PACKED
{
    // nmouse is an encoder type device

    // buttons
    struct
    {
        // left
        uint8_t A : 1;
        // right
        uint8_t B : 1;
        // wheel
        uint8_t X : 1;
        uint8_t ignore : 5;
    };

    // signed movement acceleration
    int8_t x_acc;
    int8_t y_acc;

} mouse_report_t;


static inline bool is_mouse(uint16_t vid, uint16_t pid, uint8_t dev_addr,
                            uint8_t instance, uint8_t const *desc_report,
                            uint16_t desc_len) {
  // various logitech have pid start with 0xC0 or 0xc5
  uint16_t logitech_mouse = pid & 0xff00; 
  if (vid == 0x046d && ( logitech_mouse == 0xc000) || logitech_mouse == 0xc500) return true;
  

  hid_info[instance].report_count = tuh_hid_parse_report_descriptor(
      hid_info[instance].report_info, MAX_REPORT, desc_report, desc_len);

  uint8_t const rpt_count = hid_info[instance].report_count;
  tuh_hid_report_info_t *rpt_info_arr = hid_info[instance].report_info;
  tuh_hid_report_info_t *rpt_info = NULL;

  if (rpt_count == 1 && rpt_info_arr[0].report_id == 0) {
    // Simple report without report ID as 1st byte
    rpt_info = &rpt_info_arr[0];

  } else {
    // Composite report, 1st byte is report ID, data starts from 2nd byte
    uint8_t const rpt_id = desc_report[0];

    // Find report id in the array
    for (uint8_t i = 0; i < rpt_count; i++) {
      if (rpt_id == rpt_info_arr[i].report_id) {
        rpt_info = &rpt_info_arr[i];
        break;
      }
    }

    desc_report++;
    desc_len--;
  }

  // printf("rpt: %u %u \r\n", rpt_info->usage_page, rpt_info->usage);

  return rpt_info->usage_page == HID_USAGE_PAGE_DESKTOP &&
         rpt_info->usage == HID_USAGE_DESKTOP_MOUSE;
}

static void process_mouse(uint8_t const *report, uint16_t len,
                                 JoyPort_t *joyPortState)
{
    mouse_report_t deviceReport;
    memcpy(&deviceReport, report, sizeof(deviceReport));

    joyPortState->A = deviceReport.A;
    joyPortState->B = deviceReport.B;
    joyPortState->X = deviceReport.X;

    // TODO: JoyPort_t is no longer fit for purpose as mouse uses analog
    // and also has two buttons (CBM 1351)
    // these are signed 8-bit integers being shoved into an unsigned byte
    joyPortState->POT1_X = deviceReport.x_acc;
    joyPortState->POT1_Y = deviceReport.y_acc;
}

#endif