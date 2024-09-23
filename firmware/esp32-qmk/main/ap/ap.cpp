#include "ap.h"


#include "tinyusb.h"
#include "class/hid/hid_device.h"

#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

/**
 * @brief HID report descriptor
 *
 * In this example we implement Keyboard + Mouse HID device,
 * so we must define both report descriptors
 */
const uint8_t hid_report_descriptor[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE))};


const char *hid_string_descriptor[5] =
{
  (char[]){0x09, 0x04},     // 0: is supported language is English (0x0409)
  "BARAM",                  // 1: Manufacturer
  "BARAM-45K-ESP32",        // 2: Product
  "123456",                 // 3: Serials, should use chip ID
  "HID Keyboard",           // 4: HID
};

static const uint8_t hid_configuration_descriptor[] = {
  // Configuration number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

  // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
  // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
  return hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}


static void cliThread(void *args);
static void cliHid(cli_args_t *args);



void apInit(void)
{
  cliOpen(HW_UART_CH_CLI, 115200);


  if (xTaskCreate(cliThread, "cliThread", _HW_DEF_RTOS_THREAD_MEM_CLI, NULL, _HW_DEF_RTOS_THREAD_PRI_CLI, NULL) != pdPASS)
  {
    logPrintf("[NG] cliThread()\n");   
  }  

  delay(500);
  logBoot(false);

  cliAdd("hid", cliHid);
}


void apMain(void)
{
  uint32_t pre_time;
  uint8_t index = 0;
  bool is_mounted = false;


  const tinyusb_config_t tusb_cfg = {
    .device_descriptor        = NULL,
    .string_descriptor        = hid_string_descriptor,
    .string_descriptor_count  = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]),
    .external_phy             = false,
    .configuration_descriptor = hid_configuration_descriptor,
    .self_powered             = false,
    .vbus_monitor_io          = -1,
  };

  tinyusb_driver_install(&tusb_cfg);

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 2000)
    {
      pre_time = millis();

      index++;
    }
    delay(1);

    if (is_mounted != tud_mounted())
    {
      if (!is_mounted)
        logPrintf("mounted\n");
      else
        logPrintf("not mounted\n");

      is_mounted = tud_mounted();
    }
  }
}

void cliThread(void *args)
{
  while(1)
  {
    cliMain();
    delay(2);
  }
}

void cliHid(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("mounted   : %d\n", tud_mounted());
    cliPrintf("connedted : %d\n", tud_connected());
    cliPrintf("tud_suspended : %d\n", tud_suspended());
    ret = true;
  }

  if (!ret)
  {
    cliPrintf("hid info\n");
  }
}
