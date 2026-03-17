//
// Created by murray on 27/1/26.
//

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include "FaceBase.h"

class FaceFactory {
public:
  static constexpr const char* defaultName = "standard";

  using Creator = std::function<std::unique_ptr<FaceBase>(QWidget* parent)>;

  static FaceFactory& instance();

  void registerFace(const std::string& name, Creator creator);
  std::unique_ptr<FaceBase> create(const std::string& name, QWidget* parents) const;


private:
  std::unordered_map<std::string, Creator> m_creators;
};
