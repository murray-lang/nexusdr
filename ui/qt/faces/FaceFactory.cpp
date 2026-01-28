//
// Created by murray on 27/1/26.
//

#include "FaceFactory.h"
#include "FaceBase.h"

FaceFactory&
FaceFactory::instance() {
  static FaceFactory f;
  return f;
}

void
FaceFactory::registerFace(const std::string& name, Creator creator) {
  m_creators[name] = std::move(creator);
}

std::unique_ptr<FaceBase>
FaceFactory::create(const std::string& name, QWidget* parent) const {
  if (auto it = m_creators.find(name); it != m_creators.end()) {
    return (it->second)(parent);
  }
  if (auto it = m_creators.find(defaultName); it != m_creators.end()) {
    return (it->second)(parent);
  }
  return {}; // caller handles "no faces registered"
}