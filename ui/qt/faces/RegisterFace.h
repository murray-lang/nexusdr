//
// Created by murray on 27/1/26.
//

#pragma once
#include "FaceFactory.h"

#define REGISTER_FACE(FaceClass, nameLiteral)                         \
namespace {                                                           \
  struct FaceClass##Registrar {                                         \
    FaceClass##Registrar() {                                              \
      FaceFactory::instance().registerFace(                                 \
        std::string(nameLiteral),                                       \
        [](QWidget* parent) { return std::make_unique<FaceClass>(parent); }   \
      );                                                                    \
    }                                                                     \
  };                                                                    \
  static FaceClass##Registrar g_##FaceClass##Registrar;                 \
}
