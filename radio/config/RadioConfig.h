//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_RADIOCONFIG_H
#define CUTESDR_VK6HL_RADIOCONFIG_H
#include "../../io/controller/config/ControllersConfig.h"
#include "../../config/JsonConfig.h"
#include "ReceiverConfig.h"
#include "TransmitterConfig.h"

class RadioConfig : public JsonConfig
{
public:
  explicit RadioConfig() = default;

  RadioConfig(const RadioConfig& rhs) :
    m_receiver(rhs.m_receiver),
    m_control(rhs.m_control)
  {
//    operator=(rhs);
  }

  RadioConfig& operator=(const RadioConfig& rhs)
  {
    m_receiver = rhs.m_receiver;
    m_control = rhs.m_control;
    return *this;
  }

  static RadioConfig fromJson(const nlohmann::json& json)
  {
    RadioConfig result;
    if(json.contains("receiver"))
    {
      result.m_receiver = ReceiverConfig::fromJson(json["receiver"]);
    }
    if (json.contains("control"))
    {
      result.m_control = ControllersConfig::fromJson(json["control"]);
      // for (auto& controller : json["control"])
      // {
      //   result.m_control.push_back(ControllerConfig::fromJson(controller));
      // }
    }
    return result;
  }

  const ReceiverConfig& getReceiver() const { return m_receiver; }
  // [[nodiscard]] const std::vector<ControllerConfig>& getControllers() const { return m_control; }
  [[nodiscard]] const ControllersConfig& getControl() const { return m_control; }

protected:
  ControllersConfig m_control;
  ReceiverConfig m_receiver;
};

#endif //CUTESDR_VK6HL_RADIOCONFIG_H
