#include "QtThread.h"

Thread::Thread(Runnable* runnable)
  : m_workerThread(std::make_unique<QThread>())
  , m_runnable(runnable)
{
  m_runnable->moveToThread(m_workerThread.get());
  QObject::connect(m_workerThread.get(), &QThread::started, m_runnable, &Runnable::run);
}

Thread::Thread(Thread&& other) noexcept
  : m_workerThread(std::move(other.m_workerThread))
  , m_runnable(other.m_runnable)
{
  other.m_runnable = nullptr;
}

Thread::~Thread()
{
  if (m_workerThread && m_workerThread->isRunning()) {
    m_workerThread->quit();
    m_workerThread->wait();
  }
}

Thread&
Thread::operator=(Thread&& other) noexcept
{
  m_workerThread = std::move(other.m_workerThread);
  m_runnable = other.m_runnable;
  other.m_runnable = nullptr;
  return *this;
}

void
Thread::start()
{
  if (m_workerThread) {
    m_workerThread->start();
  }
}

void
Thread::join()
{
  if (m_workerThread && m_workerThread->isRunning()) {
    m_workerThread->quit();
    m_workerThread->wait();
  }
}