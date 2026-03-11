//
// Created by murray on 12/04/23.
//

#include "../HidUsbControl.h"
#include "../UsbException.h"
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

void HidUsbControl::initialise() {
    int rc = hid_init();
    if (rc <0) {
        throw UsbException("Error initialising HIDUSB");
    }
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

void HidUsbControl::open()
{
    m_device = hid_open(this->m_vendorId, this->m_productId, nullptr);
    if (m_device == nullptr) {
        std::wstring hidError = L"Error opening HID USB io: ";
        hidError.append( hid_error(nullptr));
        throw UsbException(hidError);
    }
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

int HidUsbControl::read(unsigned char * data, size_t length) const
{
    if (m_device == nullptr) {
        throw UsbException("HID USB io not opened");
    }
    int rc = hid_read(m_device, data, length);
    if (rc < 0) {
        const wchar_t *err = hid_error(m_device);
        char mbstr[80];
        wcstombs(mbstr,err,sizeof(mbstr));
        throw UsbException(mbstr);
    }
    return rc;
}

int HidUsbControl::write(const unsigned char * data, size_t length) const
{
    if (m_device == nullptr) {
        throw UsbException("HID USB io not opened");
    }
    int rc = hid_write(m_device, data, length);
    if (rc < 0) {
        const wchar_t *err = hid_error(m_device);
        char mbstr[80];
        wcstombs(mbstr,err,sizeof(mbstr));
        throw UsbException(mbstr);
    }
    return rc;
}
