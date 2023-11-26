#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "devices/controllercontext.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "tusb.h"

extern void hid_app_init(JoyPort_t *);
extern void hid_app_task(void);

// this is global context
static JoyPort_t *_context;

static int hz = 1000;
static repeating_timer_t _timer;

static const uint GPIO_LEFT = 2;
static const uint GPIO_FIRE = 3;
static const uint GPIO_RIGHT = 4;
static const uint GPIO_UP = 27;
static const uint GPIO_DOWN = 28;

static void debug_context()
{
    // slow this down to a reasonable level for stdio
    static uint8_t delay = 100;

    delay--;

    if (delay == 0)
    {
        delay = 100;
        printf("DPAD:%03u A:%03u B:%03u X:%03u Y:%03u POT1_X:%03u POT1_Y:%03u POT2_X:%03u POT2_Y:%03u", _context->dpad, _context->A,
               _context->B, _context->X, _context->Y, _context->POT1_X, _context->POT1_Y, _context->POT2_X, _context->POT2_Y);

        printf("\r");
        fflush(stdout);
    }
}

bool sampler_callback(repeating_timer_t *rt)
{
    const uint8_t THRESHOLD = 48;
    const uint8_t AUTOFIRE_DELAY = 200;
    const uint8_t AUTOFIRE_TRIGGER = 20;
    static uint8_t _autoFireDelay = AUTOFIRE_DELAY;

    debug_context();

    // TODO: add in POT2?
    bool isUp = (_context->dpad == 7) || (_context->dpad == 0) || (_context->dpad == 1) || _context->B || (_context->POT1_Y < THRESHOLD);
    bool isDown = (_context->dpad == 3) || (_context->dpad == 4) || (_context->dpad == 5) || (_context->POT1_Y > (255 - THRESHOLD));
    bool isRight = (_context->dpad == 1) || (_context->dpad == 2) || (_context->dpad == 3) || (_context->POT1_X > (255 - THRESHOLD));
    bool isLeft = (_context->dpad == 5) || (_context->dpad == 6) || (_context->dpad == 7) || (_context->POT1_X < THRESHOLD);

    gpio_put(GPIO_UP, isUp);
    gpio_put(GPIO_DOWN, isDown);
    gpio_put(GPIO_LEFT, isLeft);
    gpio_put(GPIO_RIGHT, isRight);

    _autoFireDelay--;

    bool isAutoFire = (_context->X || _context->Y) && _autoFireDelay < AUTOFIRE_TRIGGER;

    if (_autoFireDelay == 0)
        _autoFireDelay = AUTOFIRE_DELAY;

    gpio_put(GPIO_FIRE, _context->A || isAutoFire);

    return true;
}

void blink_led(void)
{
    const uint32_t interval_ms = 1000;
    static uint32_t start_ms = 0;

    static bool led_state = false;

    // Blink every interval ms
    if (board_millis() - start_ms < interval_ms)
        return;
    start_ms += interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state;
}

void context_init()
{
    _context = malloc(sizeof(JoyPort_t));

    _context->A = 0;
    _context->B = 0;
    _context->X = 0;
    _context->dpad = 8; // 8 is released
    _context->POT1_X = 128;
    _context->POT1_Y = 128;
    _context->POT2_X = 128;
    _context->POT2_Y = 128;
}

void sampler_init()
{
    // negative timeout means exact delay (rather than delay between callbacks)
    if (!add_repeating_timer_us(-1000000 / hz, sampler_callback, NULL,
                                &_timer))
    {
        printf("Failed to add timer\n");
    }
}

void gpio_joyport_init()
{
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

int main(void)
{
    stdio_init_all();
    sleep_ms(100);

    printf("X360C64 started.\r\n");

    gpio_joyport_init();
    board_init();
    context_init();
    tuh_init(BOARD_TUH_RHPORT);

    hid_app_init(_context);
    sleep_ms(500);

    sampler_init();

    while (1)
    {
        // TODO: can something go on the other core? gpio timer?
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