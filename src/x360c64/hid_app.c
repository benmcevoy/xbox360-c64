#include "bsp/board.h"
#include "device.h"
#include "tusb.h"

static JoyPort_t* _context;

void hid_app_init(JoyPort_t* context) { _context = context; }

void hid_app_task(void) {}

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
    if (is_xbox_360(vid, pid)) {
        static uint8_t xbox_descriptor[] = { 0x09, 0x21, 0x10, 0x01, 0x21, 0x01, 0x22, 0xff, 0x00 };
        memcpy(descriptor, xbox_descriptor, sizeof(tusb_hid_descriptor_hid_t));
        return true;
    }

    // if (is_nintendo_pro(vid, pid)) {
    //     static uint8_t nin_descriptor[] = {
    //         0x09,        // bLength
    //         0x21,        // bDescriptorType (HID)
    //         0x11, 0x01,  // bcdHID 1.11
    //         0x00,        // bCountryCode
    //         0x01,        // bNumDescriptors
    //         0x22,        // bDescriptorType[0] (HID)
    //         0x56, 0x00,  // wDescriptorLength[0] 86
    //     };

    //     memcpy(descriptor, nin_descriptor, sizeof(tusb_hid_descriptor_hid_t));
    //     return true;
    //  }

    return false;
}

/* Get the vendor specific HID report */
bool tuh_descriptor_get_hid_report_cb(uint16_t vid, uint16_t pid, uint8_t **desc_report, uint16_t *desc_len)
{
    if (is_xbox_360(vid, pid)) {
        /* This is a minimal HID report descriptor just to get the buttons and X/Y inputs */
        static uint8_t xbox_hid[] = { 
            0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0xA1, 0x02, 0x85, 0x20, 0x75, 0x08, 0x95, 0x03, 
            0x06, 0x00, 0xFF, 0x81, 0x03, 0x75, 0x01, 0x95, 0x04, 0x06, 0x00, 0xFF, 0x81, 0x02, 
            0x95, 0x0C, 0x15, 0x00, 0x25, 0x01, 0x05, 0x09, 0x81, 0x02, 0x75, 0x08, 0x95, 0x04, 
            0x06, 0x00, 0xFF, 0x81, 0x03, 0xC0, 0xA1, 0x00, 0x75, 0x10, 0x95, 0x02, 0x15, 0x00, 
            0x27, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x30, 0x09, 0x31, 0x81, 0x03, 0xC0, 0xC0
        };
                
        *desc_report = xbox_hid;
        *desc_len = sizeof(xbox_hid);
        return true;
    }

    // if(is_nintendo_pro(vid, pid)) {
    //     static uint8_t switch_report_descriptor[] =
    //     {
    //         0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    //         0x09, 0x05,        // Usage (Game Pad)
    //         0xA1, 0x01,        // Collection (Application)
    //         0x15, 0x00,        //   Logical Minimum (0)
    //         0x25, 0x01,        //   Logical Maximum (1)
    //         0x35, 0x00,        //   Physical Minimum (0)
    //         0x45, 0x01,        //   Physical Maximum (1)
    //         0x75, 0x01,        //   Report Size (1)
    //         0x95, 0x10,        //   Report Count (16)
    //         0x05, 0x09,        //   Usage Page (Button)
    //         0x19, 0x01,        //   Usage Minimum (0x01)
    //         0x29, 0x10,        //   Usage Maximum (0x10)
    //         0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    //         0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    //         0x25, 0x07,        //   Logical Maximum (7)
    //         0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    //         0x75, 0x04,        //   Report Size (4)
    //         0x95, 0x01,        //   Report Count (1)
    //         0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
    //         0x09, 0x39,        //   Usage (Hat switch)
    //         0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
    //         0x65, 0x00,        //   Unit (None)
    //         0x95, 0x01,        //   Report Count (1)
    //         0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    //         0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    //         0x46, 0xFF, 0x00,  //   Physical Maximum (255)
    //         0x09, 0x30,        //   Usage (X)
    //         0x09, 0x31,        //   Usage (Y)
    //         0x09, 0x32,        //   Usage (Z)
    //         0x09, 0x35,        //   Usage (Rz)
    //         0x75, 0x08,        //   Report Size (8)
    //         0x95, 0x04,        //   Report Count (4)
    //         0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    //         0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    //         0x09, 0x20,        //   Usage (0x20)
    //         0x95, 0x01,        //   Report Count (1)
    //         0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    //         0x0A, 0x21, 0x26,  //   Usage (0x2621)
    //         0x95, 0x08,        //   Report Count (8)
    //         0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    //         0xC0,              // End Collection
    //     };

    //     *desc_report = switch_report_descriptor;
    //     *desc_len = sizeof(switch_report_descriptor);
    //     return true;
    //}

    return false;

}

