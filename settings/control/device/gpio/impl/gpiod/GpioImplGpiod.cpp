
#include "GpioImplGpiod.h"



GpioImplGpiod::GpioImplGpiod() :
    m_pChip(std::make_unique<gpiod_chip>()) 
{
    
}

GpioImplGpiod::~GpioImplGpiod()
{
    close();
}
    
bool GpioImplGpiod::isPresent()
{
    gpiod_chip* pChip = gpiod_chip_open(defaultChipPath);
    if (pChip == nullptr) {
        return false;
    }
    gpiod_chip_close(pChip);
    return true;
}
bool GpioImplGpiod::open()
{
    m_pChip.reset(gpiod_chip_open(defaultChipPath));
    if (m_pChip == nullptr) {
        std::cerr << "GpioImplGpiod::open: Failed to open GPIO chip at " << defaultChipPath << std::endl;
        return false;
    }
    return true;
}

bool GpioImplGpiod::close()
{
    if (m_pChip) {
        gpiod_chip_close(m_pChip.release());
        return true;
    }
    return false;
}
