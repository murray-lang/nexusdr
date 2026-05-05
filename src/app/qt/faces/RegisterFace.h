#pragma once
#include "FaceFactory.h"

#ifdef USE_ETL
#include "etl/memory.h"
#define MAKE_UNIQUE(FaceClass, parent) (unique_ptr<FaceClass>(new FaceClass(parent)))
#else
#include <memory>
#define MAKE_UNIQUE(FaceClass, parent) std::make_unique<FaceClass>(parent)
#endif

#define REGISTER_FACE(FaceClass, nameLiteral)             \
namespace {                                               \
  struct FaceClass##Registrar {                           \
    FaceClass##Registrar() {                              \
      FaceFactory::instance().registerFace(               \
        Config::Ui::FaceString(nameLiteral),              \
        [](QWidget* parent) -> unique_ptr<FaceBase> {     \
          return MAKE_UNIQUE(FaceClass, parent);          \
        }                                                 \
      );                                                  \
    }                                                     \
  };                                                      \
  static FaceClass##Registrar g_##FaceClass##Registrar;   \
}
