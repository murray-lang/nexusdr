//
// Created by murray on 20/8/25.
//

#ifndef CONTROLSOURCE_H
#define CONTROLSOURCE_H
#include "ControlBase.h"
#include "../../radio/settings/source/PttSource.h"
#include "../../radio/settings/source/RadioSettingsSource.h"


class ControlSource : public ControlBase, public RadioSettingsSource
{
public:
  ControlSource() : ControlBase(), m_pSink(nullptr)
  {
  }
  ~ControlSource() override = default;

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


#endif //CONTROLSOURCE_H