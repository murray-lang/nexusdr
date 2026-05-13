#pragma once
#include "core/config-settings/config/control/QtControlSourceConfig.h"
#include "core/thread/Thread.h"
#include "io/control/ControlSource.h"

class QtControlSource : public ControlSource, public Runnable
{
public:
  QtControlSource();
  QtControlSource(QtControlSource&& rhs) noexcept;
  ~QtControlSource() override;

  QtControlSource& operator=(QtControlSource&& rhs) noexcept;

  ResultCode configure(const Config::QtControlSource::Fields& config);

  bool discover() override;
  ResultCode open() override;
  void close() override;
  void exit() override;

  void run() override;

  void customEvent(QEvent* event) override;

// signals:
//   void finished();

protected:
  Thread m_thread;
};