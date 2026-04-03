/*
 * USB Descriptors for STM32H745I-DISCO MSC Device
 * Based on TinyUSB CDC/MSC example
 */

#include "tusb.h"
#include "stm32h7xx_hal.h"

//--------------------------------------------------------------------
// Device Descriptors
//--------------------------------------------------------------------

#define USB_VID   0x0483  // STMicroelectronics VID
#define USB_PID   0x5720  // MSC device PID
#define USB_BCD   0x0200

static tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,

    .bDeviceClass       = 0x00,  // Defined at interface level
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------
// Configuration Descriptor
//--------------------------------------------------------------------

enum {
  ITF_NUM_MSC = 0,
  ITF_NUM_TOTAL
};

#define EPNUM_MSC_OUT     0x01
#define EPNUM_MSC_IN      0x81

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)

// Full speed configuration
static uint8_t const desc_fs_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 4, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void) index;
  return desc_fs_configuration;
}

//--------------------------------------------------------------------
// String Descriptors
//--------------------------------------------------------------------

// String Descriptor Index
enum {
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
  STRID_MSC_INTERFACE,
};

// Array of pointer to string descriptors
static char const *string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 }, // 0: Language ID (English)
    "STMicroelectronics",           // 1: Manufacturer
    "STM32H745I-DISCO MSC",         // 2: Product
    NULL,                           // 3: Serial will use MCU unique ID
    "eMMC Mass Storage",            // 4: MSC Interface
};

static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  size_t chr_count;

  switch (index) {
    case STRID_LANGID:
      memcpy(&_desc_str[1], string_desc_arr[0], 2);
      chr_count = 1;
      break;

    case STRID_SERIAL: {
      // Use STM32 96-bit unique ID
      uint32_t uid[3];
      uid[0] = HAL_GetUIDw0();
      uid[1] = HAL_GetUIDw1();
      uid[2] = HAL_GetUIDw2();

      // Convert to hex string
      char serial_str[25];
      snprintf(serial_str, sizeof(serial_str), "%08lX%08lX%08lX",
               uid[0], uid[1], uid[2]);

      // Convert ASCII string into UTF-16
      chr_count = strlen(serial_str);
      if (chr_count > 32) chr_count = 32;

      for (size_t i = 0; i < chr_count; i++) {
        _desc_str[1 + i] = serial_str[i];
      }
      break;
    }

    default:
      // Bounds check
      if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) {
        return NULL;
      }

      const char *str = string_desc_arr[index];

      // Cap at max char
      chr_count = strlen(str);
      size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1;
      if (chr_count > max_count) chr_count = max_count;

      // Convert ASCII string into UTF-16
      for (size_t i = 0; i < chr_count; i++) {
        _desc_str[1 + i] = str[i];
      }
      break;
  }

  // First byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
  return _desc_str;
}
