//
// Created by murray on 12/04/23.
//

#pragma once

#include "UsbControl.h"


struct hid_device_;
typedef hid_device_ hid_device;

class HidUsbControl : public UsbControl {
public:
    HidUsbControl(uint16_t vendorId, uint16_t productId);
    ~HidUsbControl() override;

    void initialise() override;
    bool discover() override;
    void open() override;
    void close() override;
    void exit() override;

    int read(unsigned char * data, size_t length) const;
    int write(const unsigned char * data, size_t length) const;

protected:
    hid_device* m_device;
};
