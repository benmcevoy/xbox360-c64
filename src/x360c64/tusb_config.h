/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

  // CFG_TUSB_DEBUG for debugging - printf to uart
  // 0 : no debug
  // 1 : print error
  // 2 : print warning
  // 3 : print info

#undef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG  0

// enable xbox support
#define CFG_TUH_XINPUT 1

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+
#if CFG_TUSB_MCU == OPT_MCU_RP2040
// change to 1 if using pico-pio-usb as host controller for raspberry rp2040
// I cannot get both ports (native and PIO) runnign as host simultaneously
// might not be supported? hatahc of tinyusb says using two PIO ports it is possible
// refer: https://github.com/hathach/tinyusb/issues/1673
// but also refer: https://github.com/sekigon-gonnoc/Pico-PIO-USB/discussions/102
// who says you can?

#define CFG_TUH_RPI_PIO_USB 0
#define BOARD_TUH_RHPORT CFG_TUH_RPI_PIO_USB
#endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED OPT_MODE_DEFAULT_SPEED
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS OPT_OS_NONE
#endif

// Enable Host stack
#define CFG_TUH_ENABLED 1

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUH_MAX_SPEED BOARD_TUH_MAX_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUH_MEM_SECTION
#define CFG_TUH_MEM_SECTION
#endif

#ifndef CFG_TUH_MEM_ALIGN
#define CFG_TUH_MEM_ALIGN __attribute__((aligned(4)))
#endif

//--------------------------------------------------------------------
// CONFIGURATION
//--------------------------------------------------------------------

// Size of buffer to hold descriptors and other data used for enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE 400

#define CFG_TUH_HUB 1                        // number of supported hubs
#define CFG_TUH_CDC 0                        // CDC ACM
#define CFG_TUH_CDC_FTDI 0                   // FTDI Serial.  FTDI is not part of CDC class, only to re-use CDC driver API
#define CFG_TUH_CDC_CP210X 0                 // CP210x Serial. CP210X is not part of CDC class, only to re-use CDC driver API
#define CFG_TUH_HID (3 * CFG_TUH_DEVICE_MAX) // typical keyboard + mouse device can have 3-4 HID interfaces
#define CFG_TUH_MSC 0
#define CFG_TUH_VENDOR 0

// max device support (excluding hub device): 1 hub typically has 4 ports
#define CFG_TUH_DEVICE_MAX (3 * CFG_TUH_HUB + 1)

//------------- HID -------------//
#define CFG_TUH_HID_EPIN_BUFSIZE 64
#define CFG_TUH_HID_EPOUT_BUFSIZE 64

//------------- CDC -------------//

// Set Line Control state on enumeration/mounted:
// DTR ( bit 0), RTS (bit 1)
#define CFG_TUH_CDC_LINE_CONTROL_ON_ENUM 0x03

// Set Line Coding on enumeration/mounted, value for cdc_line_coding_t
// bit rate = 115200, 1 stop bit, no parity, 8 bit data width
#define CFG_TUH_CDC_LINE_CODING_ON_ENUM                                  \
  {                                                                      \
    115200, CDC_LINE_CONDING_STOP_BITS_1, CDC_LINE_CODING_PARITY_NONE, 8 \
  }

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
