#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "devices/controllercontext.h"

extern void hid_app_init(JoyPort_t*);
extern void hid_app_task(void);

// this is global context
static JoyPort_t* _context;

void context_init(){
  _context = malloc(sizeof(JoyPort_t));

  _context->A = 0;
  _context->B = 0;
  _context->X = 0;
  _context->dpad = 0;
  _context->POT1_X = 128;
  _context->POT1_Y = 128;
  _context->POT2_X = 128;
  _context->POT2_Y = 128;
}

void sampler_init(){
  // TODO: read the _context and set the GPIO accordingly
  // run on a timer or IRQ at 1kHz
}

void blink_led(void)
{
  const uint32_t interval_ms = 1000;
  static uint32_t start_ms = 0;

  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < interval_ms) return; 
  start_ms += interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; 
}

int main(void)
{
  stdio_init_all();

  printf("X360C64 started.\r\n");

  board_init();
  context_init();
  tuh_init(BOARD_TUH_RHPORT);
  hid_app_init(_context);

  while (1)
  {
    tuh_task();
    blink_led();
    hid_app_task();
  }

  return 0;
}

void tuh_mount_cb(uint8_t dev_addr)
{
  printf("A device with address %d is mounted\r\n", dev_addr);
}

void tuh_umount_cb(uint8_t dev_addr)
{
  printf("A device with address %d is unmounted \r\n", dev_addr);
}