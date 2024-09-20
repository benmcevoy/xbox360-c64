#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board_api.h"
#include "devices/controllercontext.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "host/usbh_pvt.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pio_usb.h"
#include "tusb.h"

extern void hid_app_init(JoyPort_t *);

// this is global context
static JoyPort_t *_context;

static int hz = 2000;
static repeating_timer_t _timer;

#define GPIO_LEFT 2
#define GPIO_FIRE 3
#define GPIO_RIGHT 4
#define GPIO_UP 27
#define GPIO_DOWN 28

#define DPAD_N 0
#define DPAD_NE 1
#define DPAD_E 2
#define DPAD_SE 3
#define DPAD_S 4
#define DPAD_SW 5
#define DPAD_W 6
#define DPAD_NW 7

static void debug_context() {
  if (_context->IsConnected == false) return;

  // slow this down to a reasonable level for stdio
  const uint16_t DELAY = 2000;
  static uint16_t delay = DELAY;

  delay--;

  if (delay == 0) {
    delay = DELAY;
    printf(
        "DPAD:%03u A:%03u B:%03u X:%03u Y:%03u POT1_X:%03u POT1_Y:%03u "
        "POT2_X:%03u POT2_Y:%03u",
        _context->dpad, _context->A, _context->B, _context->X, _context->Y,
        _context->POT1_X, _context->POT1_Y, _context->POT2_X, _context->POT2_Y);

    printf("\r\n");
    fflush(stdout);
  }
}

bool auto_firing() {
  // want about 8Hz
  const uint32_t autofire_interval_ms = 120;
  static uint32_t autofire_start_ms = 0;
  static uint32_t autofire_duty_start_ms = 0;
  static uint32_t autofire_duty_ms = 20;
  static bool is_autofire_duty = false;

  uint32_t now = board_millis();

  bool is_autofire_button_pressed = (_context->X || _context->Y);
  bool is_autofire_interval_elapsed =
      now - autofire_start_ms < autofire_interval_ms;

  if (is_autofire_interval_elapsed) {
    autofire_duty_start_ms = autofire_start_ms + autofire_duty_ms;
    autofire_start_ms += autofire_interval_ms;
    is_autofire_duty = true;
  }

  is_autofire_duty = now - autofire_duty_start_ms < autofire_duty_ms;

  return is_autofire_button_pressed && is_autofire_duty;
}

bool sampler_callback(repeating_timer_t *rt) {
  const uint8_t THRESHOLD = 48;

  debug_context();

  bool isUp = (_context->dpad == DPAD_NW) || (_context->dpad == DPAD_N) ||
              (_context->dpad == DPAD_NE) || _context->B ||
              (_context->POT1_Y < THRESHOLD);
  bool isDown = (_context->dpad == DPAD_SE) || (_context->dpad == DPAD_S) ||
                (_context->dpad == DPAD_SW) ||
                (_context->POT1_Y > (255 - THRESHOLD));
  bool isRight = (_context->dpad == 1) || (_context->dpad == DPAD_E) ||
                 (_context->dpad == DPAD_SE) ||
                 (_context->POT1_X > (255 - THRESHOLD));
  bool isLeft = (_context->dpad == DPAD_SW) || (_context->dpad == DPAD_W) ||
                (_context->dpad == DPAD_NW) || (_context->POT1_X < THRESHOLD);

  bool is_auto_firing = auto_firing();

  gpio_put(GPIO_UP, isUp);
  gpio_put(GPIO_DOWN, isDown);
  gpio_put(GPIO_LEFT, isLeft);
  gpio_put(GPIO_RIGHT, isRight);
  gpio_put(GPIO_FIRE, is_auto_firing || _context->A);

  return true;
}

void blink_led(bool isConnected) {
  if (isConnected) {
    board_led_write(true);
    return;
  }

  const uint32_t interval_ms = 250;
  static uint32_t start_ms = 0;

  static bool led_state = false;

  // Blink every interval ms
  if (board_millis() - start_ms < interval_ms) return;
  start_ms += interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state;
}

void context_init() {
  _context = malloc(sizeof(JoyPort_t));

  _context->A = 0;
  _context->B = 0;
  _context->X = 0;
  _context->dpad = 8;  // 8 is released
  _context->POT1_X = 128;
  _context->POT1_Y = 128;
  _context->POT2_X = 128;
  _context->POT2_Y = 128;
  _context->IsConnected = false;
}

void sampler_init() {
  // negative timeout means exact delay (rather than delay between callbacks)
  if (!add_repeating_timer_us(-1000000 / hz, sampler_callback, NULL, &_timer)) {
    printf("Failed to add timer\n");
  }
}

void gpio_joyport_init() {
  gpio_init(GPIO_UP);
  gpio_set_dir(GPIO_UP, GPIO_OUT);

  gpio_init(GPIO_DOWN);
  gpio_set_dir(GPIO_DOWN, GPIO_OUT);

  gpio_init(GPIO_LEFT);
  gpio_set_dir(GPIO_LEFT, GPIO_OUT);

  gpio_init(GPIO_RIGHT);
  gpio_set_dir(GPIO_RIGHT, GPIO_OUT);

  gpio_init(GPIO_FIRE);
  gpio_set_dir(GPIO_FIRE, GPIO_OUT);
}

int main(void) {
  set_sys_clock_khz(240000, true);

  stdio_init_all();
  printf("X360C64 started.\r\n");
  sleep_ms(10);

  gpio_joyport_init();
  context_init();
  board_init();

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_cfg);
  tuh_init(BOARD_TUH_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  hid_app_init(_context);
  sampler_init();

  while (1) {
    tuh_task();
    blink_led(_context->IsConnected);
    stdio_flush();
  }

  return 0;
}
