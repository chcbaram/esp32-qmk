#ifndef __USB_HID_H
#define __USB_HID_H

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

enum 
{
  USB_HID_LED_NUM_LOCK    = (1 << 0),
  USB_HID_LED_CAPS_LOCK   = (1 << 1),
  USB_HID_LED_SCROLL_LOCK = (1 << 2),
  USB_HID_LED_COMPOSE     = (1 << 3),
  USB_HID_LED_KANA        = (1 << 4)
};

bool usbHidInit(void);
bool usbHidSetViaReceiveFunc(void (*func)(uint8_t *, uint8_t));
bool usbHidSendReport(uint8_t *p_data, uint16_t length);
bool usbHidSendReportEXK(uint8_t *p_data, uint16_t length);
void usbHidSetStatusLed(uint8_t led_bits);


#ifdef __cplusplus
}
#endif

#endif