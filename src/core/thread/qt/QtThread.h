#pragma once
#include <QThread>
#include <memory>

#include "QtRunnable.h"

class Thread
{
public:
  explicit Thread(Runnable* runnable);
  Thread(Thread&& other) noexcept;
  ~Thread();

  Thread& operator=(Thread&& other) noexcept;

  void start();
  void join();

private:
  std::unique_ptr<QThread> m_workerThread;
  Runnable* m_runnable;
};