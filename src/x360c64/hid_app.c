#include "bsp/board.h"
#include "device.h"
#include "tusb.h"

static JoyPort_t* _context;

void hid_app_init(JoyPort_t *context){
  _context = context;
}

void hid_app_task(void) {}

void debug_report(uint8_t const* report, uint16_t len) {
    // we have a pointer to a bunch of bytes, len bytes in fact
    printf("report:\r");
    for (uint16_t i = 0; i < len; i++) {
        printf("%u ", *(report + (i * sizeof(uint8_t))));
    }
    fflush(stdout);
}

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
        if (!tuh_hid_receive_report(dev_addr, instance)) {
            printf("Error: cannot request to receive report\r\n");
        }
    }
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr,
           instance);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const* report, uint16_t len) {
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    x360c64_device_get_report(report, len, vid, pid, _context);

    if (!tuh_hid_receive_report(dev_addr, instance)) {
        printf("Error: cannot request to receive report\r\n");
    }
}
