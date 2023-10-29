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

    struct {
        uint8_t ZL : 1;
        uint8_t ZR : 1;
        uint8_t minus : 1;
        uint8_t plus : 1;
        uint8_t ignore2 : 1;
        uint8_t LA : 1;
        uint8_t RA : 1;
        uint8_t ignore3 : 1;
    };

    // byte6,7,8,9,10,11,12,13 are analog sticks
    // 4 16 bit values
    // TODO: what the hell, what endian is this?
    
    uint8_t la_b0, la_b1, la_b2, la_b3;
    uint8_t ra_b0, ra_b1, ra_b2, ra_b3;

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

    if (hasChanged(joyPortState->POT1_X,
                    deviceReport.la_b0)) {
        joyPortState->POT1_X = deviceReport.la_b0;
    }

    if (hasChanged(joyPortState->POT1_Y, deviceReport.la_b2)) {
        joyPortState->POT1_Y = deviceReport.la_b2;
    }

    if (hasChanged(joyPortState->POT2_X,
                    deviceReport.ra_b0)) {
        joyPortState->POT2_X = deviceReport.ra_b0;
    }

    if (hasChanged(joyPortState->POT2_Y, deviceReport.ra_b2)) {
        joyPortState->POT2_Y = deviceReport.ra_b2;
    }
}

#endif