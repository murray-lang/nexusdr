//
// Created by murray on 12/04/23.
//

#pragma once

#include <stdexcept>
#include <cstdint>

class UsbControl {

public:
    UsbControl(uint16_t vendorId, uint16_t productId) : m_vendorId(vendorId), m_productId(productId) {}
    virtual ~UsbControl() = default;
    virtual void initialise() = 0;
    virtual bool discover() = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void exit() = 0;

protected:
    uint16_t m_vendorId;
    uint16_t m_productId;
};

