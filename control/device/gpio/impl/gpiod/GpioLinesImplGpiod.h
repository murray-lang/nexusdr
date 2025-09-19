//
// Created by murray on 18/9/25.
//

#ifndef CUTESDR_VK6HL_GPIOLINESIMPLGPIOD_H
#define CUTESDR_VK6HL_GPIOLINESIMPLGPIOD_H
#include <QThread>
#include <vector>

#include "GpioImplGpiod.h"
#include "../../GpioLines.h"
#include <mutex>


class GpioLinesImplGpiod : public GpioLines, public QThread
{
  friend GpioImplGpiod;
public:
  explicit GpioLinesImplGpiod(gpiod_line_request * pLineRequest, const char* consumer = "");
  ~GpioLinesImplGpiod() override;

  void run() override;

  void startCallbacks(Callback* callback) override;
  void stopCallbacks() override;

  void release() override;

  int debounce(LineStateMap& changes) override;

  int getLineValue(uint32_t line) override;

  // Wait for edge events with a timeout in nanoseconds.
  // Returns:
  //   >0 if events are ready,
  //    0 on timeout,
  //   <0 on error (errno set by libgpiod).
  [[nodiscard]] int waitEdgeEvents(int64_t timeout_ns) const;

  // Read up to max_events edge events into the provided buffer.
  // Returns number of events read, 0 if none available, or <0 on error.
  int readEdgeEvents(struct gpiod_edge_event_buffer* buf, size_t max_events) const;

protected:
  Callback* m_pCallback;
  std::mutex m_callbackMutex;
  std::string m_consumer;
  gpiod_line_request *m_pLineRequest;
  gpiod_edge_event_buffer* m_pEventBuffer;

};


#endif //CUTESDR_VK6HL_GPIOLINESIMPLGPIOD_H