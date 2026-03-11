find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_LIBUSB QUIET libusb-1.0)
endif()

find_path(
        LIBUSB_INCLUDE_DIR
        NAMES libusb-1.0/libusb.h
        HINTS ${PC_LIBUSB_INCLUDEDIR} ${PC_LIBUSB_INCLUDE_DIRS}
)

find_library(
        LIBUSB_LIBRARY
        NAMES usb-1.0 libusb-1.0
        HINTS ${PC_LIBUSB_LIBDIR} ${PC_LIBUSB_LIBRARY_DIRS}
)

set(LIBUSB_VERSION "${PC_LIBUSB_VERSION}")
set(LIBUSB_LIBRARIES "${LIBUSB_LIBRARY}")
set(LIBUSB_INCLUDE_DIRS "${LIBUSB_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        USB
        REQUIRED_VARS LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR
        VERSION_VAR LIBUSB_VERSION
)

mark_as_advanced(
        LIBUSB_INCLUDE_DIR
        LIBUSB_LIBRARY
)
