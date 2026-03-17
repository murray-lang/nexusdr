//
// Created by murray on 5/10/25.
//

#pragma once
#include <cstdint>
#include <string>
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

  Mode(Type type, std::string  name, std::string  label , int32_t loCut, int32_t hiCut, int32_t offset) :
    m_type(type),
    m_name(std::move(name)),
    m_label(std::move(label)),
    m_loCut(loCut),
    m_hiCut(hiCut),
    m_offset(offset)
  {}
  Mode(const Mode& rhs) = default;
  virtual ~Mode() = default;
  Mode& operator=(const Mode& rhs) = default;

  [[nodiscard]] Type getType() const { return m_type; }
  [[nodiscard]] const std::string& getName() const { return m_name; }
  [[nodiscard]] const std::string& getLabel() const { return m_label; }
  [[nodiscard]] int32_t getLoCut() const { return m_loCut; }
  [[nodiscard]] int32_t getHiCut() const { return m_hiCut; }
  [[nodiscard]] int32_t getOffset() const { return m_offset; }

protected:
  Type m_type;
  std::string m_name;
  std::string m_label;
  int32_t m_loCut{};
  int32_t m_hiCut{};
  int32_t m_offset{};

};
