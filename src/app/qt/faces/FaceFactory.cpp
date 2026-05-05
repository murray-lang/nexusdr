#include "FaceFactory.h"
#include "FaceBase.h"
#include "core/config-settings/config/ui/UiConfig.h"
#ifdef USE_ETL
#include "etl/utility.h"
#endif

FaceFactory::FaceFactory() : m_creators{}
{
}

FaceFactory&
FaceFactory::instance() {
  static FaceFactory f;
  return f;
}

void
FaceFactory::registerFace(const Config::Ui::FaceString& name, const FaceCreator& creator) {
#ifdef USE_ETL
  m_creators.insert(etl::make_pair(name, creator));
#else
  m_creators.insert(std::make_pair(name, creator));
#endif
}

unique_ptr<FaceBase>
FaceFactory::create(const Config::Ui::FaceString& name, QWidget* parent) const {
  if (auto it = m_creators.find(name); it != m_creators.end()) {
    return (it->second)(parent);
  }
  if (auto it = m_creators.find(defaultName); it != m_creators.end()) {
    return (it->second)(parent);
  }
  return {}; // caller handles "no faces registered"
}