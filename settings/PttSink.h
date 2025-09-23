//
// Created by murray on 18/08/25.
//

#ifndef PTTSINK_H
#define PTTSINK_H

class PttSink
{
public:
  virtual ~PttSink() = default;
  virtual void ptt(bool on) = 0;
};

#endif //PTTSINK_H
