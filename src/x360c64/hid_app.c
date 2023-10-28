#include "bsp/board.h"
#include "device.h"
#include "tusb.h"

static JoyPort_t* _context;

void hid_app_init(JoyPort_t* context) { _context = context; }

void hid_app_task(void) {}

static char* device_to_string(Device_t device) {
    switch (device) {
        case NINTENDO: return "NINTENDO";
        case PS4: return "PS4";
        case XBOX_360: return "XBOX_360";
        default: return "UNKNOWN";
    }
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
    printf("Identified device as %s\r\n", device_to_string(device));

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

/* Callbacks for XBOX Controller support */
bool tuh_hid_is_hid_device_cb(uint16_t vid, uint16_t pid, uint8_t itf_num)
{
    /* Support interface 0 on xbox contollers */
    return itf_num == 0 && is_xbox_360(vid, pid);
}

// Get the HID descriptor in descriptor for this device
bool tuh_hid_get_hid_descriptor_cb(uint16_t vid, uint16_t pid, tusb_hid_descriptor_hid_t *descriptor)
{
    static uint8_t xbox_descriptor[] = { 0x09, 0x21, 0x10, 0x01, 0x21, 0x01, 0x22, 0xff, 0x00 };

    if (!is_xbox_360(vid, pid))
        return false;
    
    memcpy(descriptor, xbox_descriptor, sizeof(tusb_hid_descriptor_hid_t));

    return true;
}

/* Get the vendor specific HID report */
bool tuh_descriptor_get_hid_report_cb(uint16_t vid, uint16_t pid, uint8_t **desc_report, uint16_t *desc_len)
{
    /* This is a minimal HID report descriptor just to get the buttons and X/Y inputs */
    static uint8_t xbox_hid[] = { 
        0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0xA1, 0x02, 0x85, 0x20, 0x75, 0x08, 0x95, 0x03, 
        0x06, 0x00, 0xFF, 0x81, 0x03, 0x75, 0x01, 0x95, 0x04, 0x06, 0x00, 0xFF, 0x81, 0x02, 
        0x95, 0x0C, 0x15, 0x00, 0x25, 0x01, 0x05, 0x09, 0x81, 0x02, 0x75, 0x08, 0x95, 0x04, 
        0x06, 0x00, 0xFF, 0x81, 0x03, 0xC0, 0xA1, 0x00, 0x75, 0x10, 0x95, 0x02, 0x15, 0x00, 
        0x27, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x30, 0x09, 0x31, 0x81, 0x03, 0xC0, 0xC0
    };
    
    if (!is_xbox_360(vid, pid))
        return false;
    
    *desc_report = xbox_hid;
    *desc_len = sizeof(xbox_hid);
    return true;
}

