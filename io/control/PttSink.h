//
// Created by murray on 18/08/25.
//

#pragma once


class PttSink
{
public:
  virtual ~PttSink() = default;
  virtual void ptt(bool on) = 0;
};
