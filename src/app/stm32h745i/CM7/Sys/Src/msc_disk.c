/*
 * TinyUSB MSC Callbacks for STM32H745I-DISCO eMMC access
 * Bridges TinyUSB MSC class with BSP MMC driver
 */

#include "tusb.h"
#include "stm32h745i_discovery_mmc.h"
#include <stdio.h>

#if CFG_TUD_MSC

// MMC instance (0 = only eMMC on this board)
#define MMC_INSTANCE  0

// Block size is fixed at 512 bytes for MMC/SD cards
#define BLOCK_SIZE    512

// Whether host does safe-eject
static bool ejected = false;

// Card info cache
static BSP_MMC_CardInfo card_info;
static bool card_info_valid = false;

//--------------------------------------------------------------------
// TinyUSB MSC Callbacks
//--------------------------------------------------------------------

// Invoked when received SCSI_CMD_INQUIRY
// Application fills vendor id, product id and revision with string up to
// respective size
uint32_t tud_msc_inquiry2_cb(uint8_t lun, scsi_inquiry_resp_t *inquiry_resp, uint32_t bufsize) {
  (void) lun;
  (void) bufsize;

  const char vid[] = "STM32";
  const char pid[] = "H745I eMMC";
  const char rev[] = "1.0";

  strncpy((char*) inquiry_resp->vendor_id, vid, 8);
  strncpy((char*) inquiry_resp->product_id, pid, 16);
  strncpy((char*) inquiry_resp->product_rev, rev, 4);

  return sizeof(scsi_inquiry_resp_t);
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
  (void) lun;

  // If ejected via software eject, report not ready
  if (ejected) {
    // Additional Sense 3A-00 is NOT_FOUND
    tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
    return false;
  }

  // Check card state
  int32_t state = BSP_MMC_GetCardState(MMC_INSTANCE);
  if (state != BSP_ERROR_NONE) {
    // Card not ready
    tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
    return false;
  }

  return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY
// to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
  (void) lun;

  // Get card info if not cached
  if (!card_info_valid) {
    int32_t result = BSP_MMC_GetCardInfo(MMC_INSTANCE, &card_info);
    if (result == BSP_ERROR_NONE) {
      card_info_valid = true;
    } else {
      // Error getting card info - return minimal size
      printf("[MSC]:\tERROR: GetCardInfo failed (%ld)\r\n", result);
      *block_count = 0;
      *block_size = BLOCK_SIZE;
      return;
    }
  }

  *block_count = card_info.BlockNbr;
  *block_size = card_info.BlockSize;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
  (void) lun;
  (void) power_condition;

  if (load_eject) {
    ejected = !start;
  }

  return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
  (void) lun;

  // Check bounds
  if (!card_info_valid) {
    return -1;
  }

  if (lba >= card_info.BlockNbr) {
    return -1;
  }

  // TinyUSB MSC read callback doesn't support partial block reads with offset
  // offset should always be 0, and bufsize should be multiple of block size
  if (offset != 0 || bufsize % BLOCK_SIZE != 0) {
    return -1;
  }

  uint32_t block_count = bufsize / BLOCK_SIZE;

  int32_t result = BSP_MMC_ReadBlocks(MMC_INSTANCE, (uint32_t*)buffer, lba, block_count);
  if (result != BSP_ERROR_NONE) {
    return -1;
  }

  return (int32_t) bufsize;
}

bool tud_msc_is_writable_cb(uint8_t lun) {
  (void) lun;
  return true;  // eMMC is writable
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
  (void) lun;

  // Check bounds
  if (!card_info_valid) {
    return -1;
  }

  if (lba >= card_info.BlockNbr) {
    return -1;
  }

  // TinyUSB MSC write callback doesn't support partial block writes with offset
  // offset should always be 0, and bufsize should be multiple of block size
  if (offset != 0 || bufsize % BLOCK_SIZE != 0) {
    return -1;
  }

  uint32_t block_count = bufsize / BLOCK_SIZE;

  // Write blocks to eMMC
  int32_t result = BSP_MMC_WriteBlocks(MMC_INSTANCE, (uint32_t*)buffer, lba, block_count);
  if (result != BSP_ERROR_NONE) {
    printf("[MSC]:\tERROR: WriteBlocks LBA=%lu cnt=%lu err=%ld\r\n", lba, block_count, result);
    return -1;
  }

  return (int32_t) bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize) {
  (void) lun;
  (void) buffer;
  (void) bufsize;

  // Set Sense = Invalid Command Operation
  tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

  return -1;  // Stall/failed command request
}

#endif  // CFG_TUD_MSC
