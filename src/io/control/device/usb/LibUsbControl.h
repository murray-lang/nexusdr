//
// Created by murray on 12/04/23.
//

#pragma once

#include "UsbControl.h"

struct libusb_context;
struct libusb_device_handle;

class LibUsbControl: public UsbControl {
public:
    LibUsbControl(uint16_t vendorId, uint16_t productId);
    ~LibUsbControl() override;

    void initialise() override;
    bool discover() override;
    void open() override;
    void close() override;
    void exit() override;

protected:
    void cleanup();

    libusb_context * context;
    int init_rc;
    libusb_device_handle* handle;
};

