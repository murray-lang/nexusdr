//
// Created by murray on 12/04/23.
//

#include "../LibUsbControl.h"
#include <libusb-1.0/libusb.h>

LibUsbControl::LibUsbControl(uint16_t vendorId, uint16_t productId)
    : UsbControl(vendorId, productId){
    context = nullptr;
    init_rc = LIBUSB_ERROR_NOT_FOUND;
    handle = nullptr;
}

LibUsbControl::~LibUsbControl() {
    cleanup();
}

ResultCode LibUsbControl::initialise() {
    init_rc = libusb_init(&context);
    if (init_rc != LIBUSB_SUCCESS) {
       return ResultCode::ERR_USB_LIB_INIT;
    }
  return ResultCode::OK;
}

bool LibUsbControl::discover() {
    if (init_rc != LIBUSB_SUCCESS) {
      return false;
    }
    bool result = false;
    struct libusb_device** device_list;

    ssize_t device_count = libusb_get_device_list(context, &device_list);
    for (int i = 0; i < device_count; i++) {
        libusb_device* device = device_list[i];
        libusb_device_descriptor desc {};
        libusb_get_device_descriptor(device, &desc);
        if (desc.idVendor == m_vendorId && desc.idProduct == m_productId) {
            result = true;
            break;
        }
    }
    libusb_free_device_list(device_list, 1);
    return result;
}

ResultCode LibUsbControl::open() {
    if (init_rc != LIBUSB_SUCCESS) {
      return ResultCode::ERR_USB_LIB_INIT;
    }
    int rc = LIBUSB_ERROR_NOT_FOUND;
    struct libusb_device** device_list;

    ssize_t device_count = libusb_get_device_list(context, &device_list);
    for (int i = 0; i < device_count; i++) {
        libusb_device* device = device_list[i];
        libusb_device_descriptor desc {};
        libusb_get_device_descriptor(device, &desc);
        if (desc.idVendor == m_vendorId && desc.idProduct == m_productId) {
            rc = libusb_open(device, &handle);
            break;
        }
    }
    libusb_free_device_list(device_list, 1);
    if (rc < 0) {
      return ResultCode::ERR_USB_LIB_OPEN;
    }
    rc = libusb_detach_kernel_driver(handle,0);
    if(rc < 0) {
      return ResultCode::ERR_USB_LIB_DETACH_KERNEL_DRIVER;
    }
    rc = libusb_claim_interface(handle,0);
    if(rc < 0) {
      return ResultCode::ERR_USB_LIB_CLAIM_INTERFACE;
    }
  return ResultCode::OK;
}

void LibUsbControl::close() {
    if (handle != nullptr) {
        libusb_attach_kernel_driver(handle, 0);
        libusb_close(handle);
        handle = nullptr;
    }
}

void LibUsbControl::exit() {
    cleanup();
}

void LibUsbControl::cleanup() {
    if (context != nullptr) {
        libusb_exit(context);
        context = nullptr;
    }
    init_rc = LIBUSB_ERROR_NOT_FOUND;
}
