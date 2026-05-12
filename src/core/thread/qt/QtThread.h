#pragma once
#include <QThread>

#include "QtRunnable.h"

class Thread : public QObject
{
  Q_OBJECT
public:
  explicit Thread(Runnable* runnable);
  ~Thread() override = default;

  void start();
  void join();

private:
  QThread m_workerThread;
  Runnable* m_runnable;
};