//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_SOURCECONTROLLER_H
#define CUTESDR_VK6HL_SOURCECONTROLLER_H
#include "Controller.h"
#include "../../radio/settings/source/PttSource.h"
#include "../../radio/settings/source/RadioSettingsSource.h"


class SourceController : public Controller, public RadioSettingsSource
{
public:
  SourceController() : Controller(), m_pSink(nullptr)
  {
  }
  ~SourceController() override = default;

  void connect(RadioSettingsSink* pSink) override
  {
    m_pSink = pSink;
  }

  void supply(const RadioSettings& radioSettings) override
  {
    if (m_pSink) {
      m_pSink->apply(radioSettings);
    }
  }

protected:
  RadioSettingsSink* m_pSink;
};


#endif //CUTESDR_VK6HL_SOURCECONTROLLER_H