#pragma once

class Runnable : public QObject
{
  Q_OBJECT
public:
  ~Runnable() override = default;

public slots:
  virtual void run() = 0;
};
