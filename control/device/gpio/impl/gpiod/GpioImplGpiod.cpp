
#include "GpioImplGpiod.h"

#include "GpioLinesImplGpiod.h"
#include "control/device/gpio/GpioException.h"


GpioImplGpiod::GpioImplGpiod() :
    m_pChip()
{
    
}

GpioImplGpiod::~GpioImplGpiod()
{
  GpioImplGpiod::close();
}
    
bool
GpioImplGpiod::isPresent()
{
    gpiod_chip* pChip = gpiod_chip_open(defaultChipPath);
    if (pChip == nullptr) {
        return false;
    }
    gpiod_chip_close(pChip);
    return true;
}
bool
GpioImplGpiod::open()
{
  m_pChip = gpiod_chip_open(defaultChipPath);
  if (m_pChip == nullptr) {
    std::cerr << "GpioImplGpiod::open: Failed to open GPIO chip at " << defaultChipPath << std::endl;
    return false;
  }
  return true;
}

bool
GpioImplGpiod::close()
{
  if (m_pChip) {
    gpiod_chip_close(m_pChip);
    m_pChip = nullptr;
    return true;
  }
  return false;
}

GpioLines*
GpioImplGpiod::requestLines(
  const char * contextId,
  const std::vector<uint32_t>& lines,
  GpioLines::Direction direction,
  GpioLines::Bias bias,
  GpioLines::Edge edge
)
{
  gpiod_line_config *lcfg = gpiod_line_config_new();
  if (lcfg == nullptr) {
    throw GpioException("Failed to allocate line config");
  }
  gpiod_line_settings *ls = gpiod_line_settings_new();
  gpiod_line_settings_set_direction(ls, static_cast<gpiod_line_direction>(direction));
  gpiod_line_settings_set_bias(ls, static_cast<gpiod_line_bias>(bias));
  gpiod_line_settings_set_edge_detection(ls, static_cast<gpiod_line_edge>(edge));
  gpiod_line_config_add_line_settings(lcfg, lines.data(), lines.size(), ls);
  gpiod_line_settings_free(ls);

  gpiod_request_config *rcfg = gpiod_request_config_new();
  gpiod_request_config_set_consumer(rcfg, contextId);

  gpiod_line_request *pLineRequest = gpiod_chip_request_lines(m_pChip, rcfg, lcfg);
  gpiod_request_config_free(rcfg);
  gpiod_line_config_free(lcfg);
  if (pLineRequest == nullptr) {
    throw GpioException("Failed to request GPIO lines");
  }
  auto pLines = new GpioLinesImplGpiod(pLineRequest);
  return dynamic_cast<GpioLines*>(pLines);

}
