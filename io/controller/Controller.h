//
// Created by murray on 15/04/23.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../radio/settings/sink/RadioSettingsSink.h"
#include "../../radio/settings/sink/PttSink.h"

class Controller {
public:
  Controller() = default;
  virtual ~Controller() = default;
  // virtual void applySettings(const RadioSettings& radioSettings) = 0;
  // void apply(const ReceiverSettings& rxSettings) override;
  // virtual void readSettings(RadioSettings& radioSettings) = 0;

  virtual void initialise(const nlohmann::json& json) = 0;
  virtual bool discover() = 0;
  virtual void open() = 0;
  virtual void close() = 0;
  virtual void exit() = 0;

  const std::string& getId() { return id; }
  void setId(const std::string& newId ) { id = newId; }

protected:
    std::string id;
};

#endif //CONTROLLER_H
