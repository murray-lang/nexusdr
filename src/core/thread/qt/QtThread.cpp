#include "QtThread.h"

Thread::Thread(Runnable* runnable)
  : m_runnable(runnable)
{
  m_runnable->moveToThread(&m_workerThread);
  connect(&m_workerThread, &QThread::started, m_runnable, &Runnable::run);
  connect(&m_workerThread, &QThread::finished, m_runnable, &QObject::deleteLater);
}

void
Thread::start()
{
  m_workerThread.start();
}

void
Thread::join()
{
  m_workerThread.quit();
  m_workerThread.wait();
}