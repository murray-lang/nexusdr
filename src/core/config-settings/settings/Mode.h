//
// Created by murray on 5/10/25.
//

#pragma once
#include "SettingsCrossPlatformTypes.h"


#include <utility>

class Mode
{
public:
  enum Type
  {
    NONE = 0,
    AMN,
    AMW,
    LSB,
    USB,
    FMN,
    FMW,
    CWL,
    CWU
  };

  Mode() :
    m_type(NONE),
    m_name("none"),
    m_label("None"),
    m_loCut(-10000),
    m_hiCut(10000),
    m_offset(0)
  {}

  Mode(Type type, ModeNameString  name, ModeLabelString label , int32_t loCut, int32_t hiCut, int32_t offset) :
    m_type(type),
    m_name(move(name)),
    m_label(move(label)),
    m_loCut(loCut),
    m_hiCut(hiCut),
    m_offset(offset)
  {}
  Mode(const Mode& rhs)
  {
    if (this != &rhs)
    {
      m_type = rhs.m_type;
      m_name = move(rhs.m_name);
      m_label = move(rhs.m_label);
      m_loCut = rhs.m_loCut;
      m_hiCut = rhs.m_hiCut;
      m_offset = rhs.m_offset;
    }
  }
  virtual ~Mode() = default;
  Mode& operator=(const Mode& rhs) = default;

  [[nodiscard]] Type getType() const { return m_type; }
  [[nodiscard]] const ModeNameString& getName() const { return m_name; }
  [[nodiscard]] const ModeLabelString& getLabel() const { return m_label; }
  [[nodiscard]] int32_t getLoCut() const { return m_loCut; }
  [[nodiscard]] int32_t getHiCut() const { return m_hiCut; }
  [[nodiscard]] int32_t getOffset() const { return m_offset; }

protected:
  Type m_type;
  ModeNameString m_name;
  ModeLabelString m_label;
  int32_t m_loCut{};
  int32_t m_hiCut{};
  int32_t m_offset{};

};
