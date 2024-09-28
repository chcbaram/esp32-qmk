#include "usbd_hid.h"


#define KBD_NAME                    "BARAM-45K-ESP32"

#define USB_VID                     0x0483
#define USB_PID                     0x5300


#include "cli.h"
#include "log.h"
#include "keys.h"
#include "qbuffer.h"



#include "tinyusb.h"
#include "class/hid/hid_device.h"



#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)


//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    // Use Interface Association Descriptor (IAD) for Audio
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0200,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+
const uint8_t hid_report_descriptor[] ={
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE))};


const char *hid_string_descriptor[5] =
{
  (char[]){0x09, 0x04},     // 0: is supported language is English (0x0409)
  "BARAM",                  // 1: Manufacturer
  KBD_NAME,                 // 2: Product
  "123456",                 // 3: Serials, should use chip ID
  "HID Keyboard",           // 4: HID
};

static const uint8_t hid_configuration_descriptor[] = {
  // Configuration number, interface count, string index, total length,        attribute,                   power in mA
  TUD_CONFIG_DESCRIPTOR(1, 1,               0,            TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),

  // Interface number,  string index, boot protocol, report descriptor len,         EP In address, size & polling inerval
  TUD_HID_DESCRIPTOR(0, 4,            true,          sizeof(hid_report_descriptor), 0x81,          16,    1),
};



bool usbHidInit(void)
{
  const tinyusb_config_t tusb_cfg = {
    .device_descriptor        = &desc_device,
    .string_descriptor        = hid_string_descriptor,
    .string_descriptor_count  = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]),
    .external_phy             = false,
    .configuration_descriptor = hid_configuration_descriptor,
    .self_powered             = false,
    .vbus_monitor_io          = -1,
  };

  tinyusb_driver_install(&tusb_cfg);
  return true;
}

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
  // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
  return hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  logPrintf("tud_hid_get_report_cb()\n");
  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
}

#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("usbhid info\n");
  }
}
#endif
