#ifndef X360C64_DEVICE_XBOX_360
#define X360C64_DEVICE_XBOX_360

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "controllercontext.h"
#include "tusb.h"

typedef struct TU_ATTR_PACKED {
    // this is "packed".
    // e.g. d-pad is 3 bits of a byte

    // b0 and b1 are some kind of report
    uint8_t b0, b1;
    
    // byte 2 is d-pad
    struct {
        uint8_t dpad : 4;     // 0=released  0x0000LRDU
        uint8_t start: 1;      
        uint8_t back: 1;      
        uint8_t left_analog_click : 1;  
        uint8_t right_analog_click: 1;  
    };

    // b3: buttons = left_should=1,rs=2,  A:16, B:32, x:64, y=128
    
    struct {
        uint8_t LS : 1; // left shoulder button
        uint8_t RS : 1;
        uint8_t HOME : 1;
        uint8_t ignore1: 1;     // not used
        uint8_t A : 1;
        uint8_t B : 1;
        uint8_t X : 1; 
        uint8_t Y : 1;
    };

    uint8_t left_trigger;
    uint8_t right_trigger;

    // byte6,7,8,9,10,11,12,13 are analog sticks
    // 4 16 bit values
    // read the spec, lol - 16 bit SIGNED
    
    int16_t lax;
    int16_t lay;
    int16_t rax;
    int16_t ray;

    uint8_t unused1, unused2, unused3, unused4, unused5, unused6;

} xbox_report_t;

static uint8_t normalize_dpad(uint8_t xdpad) {
    // xdpad up=1 d=2, l=4, r=8  start=16 back=32  la_click=64 ra_click=128, 

    const uint8_t UP = 1;
    const uint8_t DOWN = 2;
    const uint8_t LEFT = 4;
    const uint8_t RIGHT = 8;


    // "normal" dpad is (hat format, 0x08 is released, 0=N, 1=NE, 2=E,
    // 3=SE, 4=S, 5=SW, 6=W, 7=NW)
    uint8_t dpad = 0;

    // UP
    if(xdpad == UP) return 0;
    if(xdpad == UP + LEFT) return 7;
    if(xdpad == UP + RIGHT) return 1;

    // DOWN
    if(xdpad == DOWN) return 4;
    if(xdpad == DOWN + LEFT) return 5;
    if(xdpad == DOWN + RIGHT) return 3;

    // LEFT
    if(xdpad == LEFT) return 6;

    // RIGHT
    if(xdpad == RIGHT) return 2;
    
    // released
    return 8;
}

static inline uint8_t scale(int16_t value) {
    // to -32768..32768 => 0..255
    return (value /256) + 128;
}

static void process_xbox_360(uint8_t const *report, uint16_t len,
                          JoyPort_t *joyPortState) {
    xbox_report_t deviceReport;
    memcpy(&deviceReport, report, sizeof(deviceReport));

    // xbox hat format is different
    uint8_t dpad = normalize_dpad(deviceReport.dpad);

    //printf("%u %u\r\n", deviceReport.dpad, dpad);

    joyPortState->dpad = dpad;
    joyPortState->A = deviceReport.A;
    joyPortState->B = deviceReport.B;
    joyPortState->X = deviceReport.X;
    joyPortState->Y = deviceReport.Y;

    uint8_t lax = scale(deviceReport.lax);
    uint8_t lay = scale(deviceReport.lay);
    uint8_t rax = scale(deviceReport.rax);
    uint8_t ray = scale(deviceReport.ray);

    // oh microsoft... normalise analog too - verticals are inverse
    if (hasChanged(joyPortState->POT1_X, lax)) {
        joyPortState->POT1_X = lax;
    }

    if (hasChanged(joyPortState->POT1_Y, 255 - lay)) {
        joyPortState->POT1_Y = 255 - lay;
    }

    if (hasChanged(joyPortState->POT2_X, rax)) {
         joyPortState->POT2_X = rax;
    }

    if (hasChanged(joyPortState->POT2_Y, 255 - ray)) {
        joyPortState->POT2_Y = 255 - ray;
    }
}

#endif