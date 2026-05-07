#pragma once

#include <QThread>

#include "GpioGpiod.h"
#include <mutex>

#ifdef USE_ETL
#include "etl/vector.h"
#include "etl/unordered_map.h"
#else
#include <vector>
#include <unordered_map>

#endif


class DigitalInputLinesRequest : public QThread
{

public:
  struct LineState
  {
    uint32_t line;
    bool debounce;
    bool isDebounced;
    // uint8_t candidateValue;
    // uint64_t candidateEdgeTime;
    uint64_t firstEdgeTime;
    uint64_t lastRisingTime;
    uint64_t lastFallingTime;
    uint8_t value;
    bool changed;
    bool processed;
  };

#ifdef USE_ETL
  using LineStateMap = etl::unordered_map<uint32_t, LineState, MAX_GPIO>;
  using LineStateVector = etl::vector<LineState, MAX_GPIO>;
#else
  using LineStateMap = std::unordered_map<uint32_t, LineState>;
  using LineStateVector = std::vector<LineState>;
#endif

  class Callback
  {
  public:
    virtual ~Callback() = default;
    virtual void callback(LineStateVector& lineStates) = 0;

  };

  explicit DigitalInputLinesRequest();

  // Delete copy operations
  DigitalInputLinesRequest(const DigitalInputLinesRequest&) = delete;
  DigitalInputLinesRequest& operator=(const DigitalInputLinesRequest&) = delete;

  // Move constructor
  DigitalInputLinesRequest(DigitalInputLinesRequest&& other) noexcept;

  // Move assignment operator
  DigitalInputLinesRequest& operator=(DigitalInputLinesRequest&& other) noexcept;

  ~DigitalInputLinesRequest() override;

  void initialise(gpiod_chip* pChip, const char* consumer = "");

  void run() override;

  ResultCode startCallbacks(Callback& callback);
  void stopCallbacks();

  ResultCode request(const char * contextId, const DigitalInputVariantVector& inputs);

  void release();

  // int debounce(LineStateMap& changes) override;

  int getLineValue(uint32_t line);


protected:
  void initialiseLineStates(const DigitalInputVariantVector& inputs);

  [[nodiscard]] bool isDebounced(int line) const;
  // int getLineStateChanges(DigitalInputLineStateMap& changes);
  bool updateLineStates(int* numEvents);
  bool continueDebouncing(int* numDebounced);
  bool callbackWithChangedLineStates();

  static uint64_t getCurrentTime();
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
  Gpio& m_gpio;
  // LineStateMap m_lineStates;
  LineStateVector m_lineStates;

  gpiod_chip* m_pChip;
  optional<reference_wrapper<Callback>> m_pCallback;
  std::mutex m_callbackMutex;
  std::string m_consumer;
  gpiod_line_request *m_pLineRequest;
  gpiod_edge_event_buffer* m_pEventBuffer;
  uint64_t m_debouncePeriod;
};
