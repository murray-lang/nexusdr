//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_RADIOCONFIG_H
#define CUTESDR_VK6HL_RADIOCONFIG_H
#include <config/ControlConfig.h>
#include "ConfigBase.h"
#include "ReceiverConfig.h"

class RadioConfig : public ConfigBase
{
public:
  static constexpr auto type = "radio";

   RadioConfig() : ConfigBase(type), m_pReceiver(nullptr), m_pControl(nullptr) {}
  ~RadioConfig() override
  {
    delete m_pReceiver;
    delete m_pControl;
  }
//   RadioConfig(const RadioConfig& rhs) :
//     m_receiver(rhs.m_receiver),
//     m_control(rhs.m_control)
//   {
// //    operator=(rhs);
//   }
//
//   RadioConfig& operator=(const RadioConfig& rhs)
//   {
//     m_receiver = rhs.m_receiver;
//     m_control = rhs.m_control;
//     return *this;
//   }

  void initialise(const nlohmann::json& json) override
  {
    if(json.contains("receiver"))
    {
      m_pReceiver = dynamic_cast<ReceiverConfig *>(ConfigFactory::create("receiver", json["receiver"]));
    }
    if (json.contains("control"))
    {
      m_pControl = dynamic_cast<ControlConfig *>(ConfigFactory::create("control", json["control"]));
    }
  }

  [[nodiscard]] const ReceiverConfig* getReceiver() const { return m_pReceiver; }
  // [[nodiscard]] const std::vector<ControllerConfig>& getControllers() const { return m_control; }
  [[nodiscard]] const ControlConfig* getControl() const { return m_pControl; }

protected:
  ControlConfig* m_pControl;
  ReceiverConfig* m_pReceiver;
};

#endif //CUTESDR_VK6HL_RADIOCONFIG_H
