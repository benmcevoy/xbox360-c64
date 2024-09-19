#include "bsp/board.h"
#include "device.h"
#include "tusb.h"
#include "xinput_host.h"

static JoyPort_t *_context;

void hid_app_init(JoyPort_t *context) { _context = context; }

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const *desc_report, uint16_t desc_len) {
  (void)desc_report;
  (void)desc_len;

  printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr,
         instance);

  x360c64_device_identify(dev_addr, instance, desc_report, desc_len, _context);

  printf("Identified device as %s\r\n", device_to_string(_context->Device));

  if (_context->Device != UNKNOWN) {
    if (!tuh_hid_receive_report(dev_addr, instance)) {
      printf("Error: cannot request to receive report\r\n");
    }
    _context->IsConnected = true;
  }
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr,
         instance);
  _context->IsConnected = false;
  _context->Device = UNKNOWN;
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const *report, uint16_t len) {
  x360c64_device_get_report(dev_addr, instance, report, len, _context);

  if (!tuh_hid_receive_report(dev_addr, instance)) {
    printf("Error: cannot request to receive report\r\n");
  }
}

#if (TUSB_OPT_HOST_ENABLED && CFG_TUH_XINPUT)
// registers the XINPUT driver with tinyusb
usbh_class_driver_t const *usbh_app_driver_get_cb(uint8_t *driver_count) {
  *driver_count = 1;

  printf("XINPUT driver registered\r\n");

  return &usbh_xinput_driver;
}
#endif

void tuh_xinput_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                   xinputh_interface_t const *xid_itf,
                                   uint16_t len) {
  // the general HID callback  tuh_xinput_mount_cb resets device to UNKNOWN
  // because my code sucks there are two very different ways of identifying
  // devices here...
  _context->Device = XBOX_360;
  // dodgy, but structs align xid_itf is basically my old xbox_report_t
  x360c64_device_get_report(dev_addr, instance, (uint8_t *)xid_itf, len,
                            _context);

  tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_mount_cb(uint8_t dev_addr, uint8_t instance, const xinputh_interface_t *xinput_itf)
{
  TU_LOG1("XINPUT MOUNTED %02x %d\n", dev_addr, instance);
  // If this is a Xbox 360 Wireless controller we need to wait for a connection
  // packet on the in pipe before setting LEDs etc. So just start getting data
  // until a controller is connected.
  if (xinput_itf->type == XBOX360_WIRELESS && xinput_itf->connected == false) {
    tuh_xinput_receive_report(dev_addr, instance);
    return;
  }
  // tuh_xinput_set_led(dev_addr, instance, 0, true);
  // stops the light blinking on the controller adn sets the first led segment on
  tuh_xinput_set_led(dev_addr, instance, 1, true);
  // tuh_xinput_set_rumble(dev_addr, instance, 0, 0, true);
  
  tuh_xinput_receive_report(dev_addr, instance);

  _context->Device = XBOX_360;
  _context->IsConnected = true;
}

void tuh_xinput_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  TU_LOG1("XINPUT UNMOUNTED %02x %d\n", dev_addr, instance);
  _context->Device = UNKNOWN;
  _context->IsConnected = false;
}
