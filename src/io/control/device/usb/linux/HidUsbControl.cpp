//
// Created by murray on 12/04/23.
//

#include "../HidUsbControl.h"
#include <hidapi.h>

HidUsbControl::HidUsbControl(uint16_t vendorId, uint16_t productId)
    : UsbControl(vendorId, productId)
{
    m_device = nullptr;
}

HidUsbControl::~HidUsbControl()
{
    hid_exit();
}

ResultCode HidUsbControl::initialise() {
  int rc = hid_init();
  if (rc <0) {
    return ResultCode::ERR_USB_HID_INIT;
  }
  return ResultCode::OK;
}

bool HidUsbControl::discover()
{
  bool result = false;
  struct hid_device_info* list = hid_enumerate(this->m_vendorId, this->m_productId);
  if (list != nullptr) {
      hid_free_enumeration(list);
      result = true;
  }
  return result;
}

ResultCode HidUsbControl::open()
{
  m_device = hid_open(this->m_vendorId, this->m_productId, nullptr);
  if (m_device == nullptr) {
    return ResultCode::ERR_USB_HID_OPEN;
  }
  return ResultCode::OK;
}

void HidUsbControl::close()
{
    if (m_device != nullptr) {
        hid_close(m_device);
        m_device = nullptr;
    }
}

void HidUsbControl::exit()
{
    hid_exit();
}

ResultCode HidUsbControl::read(unsigned char * data, size_t length, size_t* bytesRead) const
{
  if (m_device == nullptr) {
    return ResultCode::ERR_USB_HID_NOT_OPEN;
  }
  int rc = hid_read(m_device, data, length);
  if (rc < 0) {
    return ResultCode::ERR_USB_HID_READ;
  }
  *bytesRead = rc;
  return ResultCode::OK;
}

ResultCode HidUsbControl::write(const unsigned char * data, size_t length, size_t* bytesWritten) const
{
  if (m_device == nullptr) {
    return ResultCode::ERR_USB_HID_NOT_OPEN;
  }
  int rc = hid_write(m_device, data, length);
  if (rc < 0) {
    return ResultCode::ERR_USB_HID_WRITE;
  }
  *bytesWritten = rc;
  return ResultCode::OK;
}
