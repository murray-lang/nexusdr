#include "CrossPlatformTypes.h"
#include "DigitalInputLinesRequestGpiod.h"
#include "../../digital/DigitalInputTypes.h"
#include <ctime>
#include <qdebug.h>

DigitalInputLinesRequest::DigitalInputLinesRequest() :
  m_gpio(Gpio::getInstance()),
  m_lineStates(MAX_GPIO),
  m_pChip (nullptr),
  // m_pCallback(nullptr),
  m_pLineRequest(nullptr),
  m_debouncePeriod(5'000'000)

{
  m_pEventBuffer = gpiod_edge_event_buffer_new(64);

}

DigitalInputLinesRequest::DigitalInputLinesRequest(DigitalInputLinesRequest&& other) noexcept :
  m_gpio(other.m_gpio),
  m_lineStates(std::move(other.m_lineStates)),
  m_pChip(other.m_pChip),
  m_pCallback(std::move(other.m_pCallback)),
  m_consumer(std::move(other.m_consumer)),
  m_pLineRequest(other.m_pLineRequest),
  m_pEventBuffer(other.m_pEventBuffer),
  m_debouncePeriod(other.m_debouncePeriod)
{
  // Clear the source object's pointers so destructor doesn't free them
  other.m_pChip = nullptr;
  other.m_pLineRequest = nullptr;
  other.m_pEventBuffer = nullptr;
  other.m_pCallback = nullopt;
}

DigitalInputLinesRequest& DigitalInputLinesRequest::operator=(DigitalInputLinesRequest&& other) noexcept
{
  if (this != &other) {
    // Stop any callbacks and wait for thread to finish
    stopCallbacks();

    // Clean up existing resources
    if (m_pEventBuffer) {
      gpiod_edge_event_buffer_free(m_pEventBuffer);
    }
    release();

    // Move data from other
    m_lineStates = std::move(other.m_lineStates);
    m_pChip = other.m_pChip;
    m_pCallback = std::move(other.m_pCallback);
    m_consumer = std::move(other.m_consumer);
    m_pLineRequest = other.m_pLineRequest;
    m_pEventBuffer = other.m_pEventBuffer;
    m_debouncePeriod = other.m_debouncePeriod;

    // Clear the source object's pointers so destructor doesn't free them
    other.m_pChip = nullptr;
    other.m_pLineRequest = nullptr;
    other.m_pEventBuffer = nullptr;
    other.m_pCallback = nullopt;
  }
  return *this;
}

DigitalInputLinesRequest::~DigitalInputLinesRequest()
{
  gpiod_edge_event_buffer_free(m_pEventBuffer);
  release();
}

void
DigitalInputLinesRequest::initialise(gpiod_chip* pChip, const char* consumer)
{
  m_pChip = pChip;
  m_consumer = consumer;
}

bool
DigitalInputLinesRequest::isDebounced(int line) const
{
  gpiod_line_info * li = gpiod_chip_get_line_info(m_pChip, line);
  if (li != nullptr) {
    bool result = gpiod_line_info_is_debounced(li);
    gpiod_line_info_free(li);
    return result;
  }
  return false;
  
}

uint64_t
DigitalInputLinesRequest::getCurrentTime()
{
  timespec now{};
  clock_gettime(CLOCK_MONOTONIC, &now);
  return static_cast<uint64_t>(now.tv_sec) * 1'000'000'000ULL + now.tv_nsec;

}

ResultCode
DigitalInputLinesRequest::startCallbacks(Callback& callback)
{
  if (m_pCallback) {
    return ResultCode::ERR_DIGITAL_INPUT_LINES_CALLBACK_ALREADY_SET;
  }
  // std::lock_guard<std::mutex> lock(m_callbackMutex);
  m_callbackMutex.lock();
  m_pCallback.emplace(callback);
  m_callbackMutex.unlock();
  start();
  return ResultCode::OK;
}

void
DigitalInputLinesRequest::stopCallbacks()
{
  m_callbackMutex.lock();
  m_pCallback = nullopt;
  m_callbackMutex.unlock();
  wait();
}

ResultCode
DigitalInputLinesRequest::request(const char * contextId, const DigitalInputVariantVector& inputs)
{
  // for (auto line : lines) {
  //   if (isDebounced(line)) {
  //     qDebug() << "Line " << line << " is debounced";
  //   } else {
  //     qDebug() << "Line " << line << " is NOT debounced";
  //   }
  // }
  gpiod_line_config *lcfg = gpiod_line_config_new();
  if (lcfg == nullptr) {
    return ResultCode::ERR_DIGITAL_INPUT_LINE_CONFIG;
  }
  for (const auto& input : inputs) {
    visit([&](const auto& concreteInput)
    {
      gpiod_line_settings *ls = gpiod_line_settings_new();
      gpiod_line_settings_set_direction(ls, GPIOD_LINE_DIRECTION_INPUT);
      gpiod_line_settings_set_bias(ls, static_cast<gpiod_line_bias>(concreteInput.getBias()));
      gpiod_line_settings_set_edge_detection(ls, static_cast<gpiod_line_edge>(concreteInput.getEdge()));
      // gpiod_line_settings_set_debounce_period_us(ls, 2000);
      const GpioLinesVector& lineNos = concreteInput.getLines();
      gpiod_line_config_add_line_settings(lcfg, lineNos.data(), lineNos.size(), ls);
      gpiod_line_settings_free(ls);
    }, input);
  }

  gpiod_request_config *rcfg = gpiod_request_config_new();
  gpiod_request_config_set_consumer(rcfg, contextId);

  gpiod_line_request *pLineRequest = gpiod_chip_request_lines(m_pChip, rcfg, lcfg);
  gpiod_request_config_free(rcfg);
  gpiod_line_config_free(lcfg);
  if (pLineRequest == nullptr) {
    return ResultCode::ERR_DIGITAL_INPUT_LINE_REQUEST_FAILED;
  }
  m_pLineRequest = pLineRequest;
  initialiseLineStates(inputs);
  return ResultCode::OK;
}


void
DigitalInputLinesRequest::release()
{
  if (m_pLineRequest) {
    gpiod_line_request_release(m_pLineRequest);
    m_pLineRequest = nullptr;
  }
}

int
DigitalInputLinesRequest::getLineValue(uint32_t line)
{
  return gpiod_line_request_get_value(m_pLineRequest, line);
}

int
DigitalInputLinesRequest::waitEdgeEvents(int64_t timeout_ns) const
{
  return gpiod_line_request_wait_edge_events(m_pLineRequest, timeout_ns);
}

int
DigitalInputLinesRequest::readEdgeEvents(struct gpiod_edge_event_buffer* buf, size_t max_events) const
{
  return gpiod_line_request_read_edge_events(m_pLineRequest, buf, max_events);
}

void
DigitalInputLinesRequest::run()
{
  // readInitialInputStates();
  // gpiod_edge_event_buffer* eventBuff = gpiod_edge_event_buffer_new(64);
  // if (!eventBuff) {
  //   return; //TODO: flag the error
  // }

  // LineStateMap debouncedLines;

  bool haveCallback = !!m_pCallback;
  while (haveCallback) {
    constexpr int64_t idleTimeout = 50'000'000;
    int wr = waitEdgeEvents(idleTimeout);
    if (wr < 0) {
      // error; consider logging and continuing or breaking
      continue;
    }
    
    if (wr == 0) {
      // haveCallback = callbackWithAnyDebouncedLineStates();
      // int numDebounced = updateDebouncingStates();
      // if (numDebounced == 0) {
      //   continue;
      // }
    }
    
    int numEvents = 0;
    int numDebounced = 0;
    if (wr > 0) {
      // qDebug() <<"--------------";
      numEvents = updateLineStates();
    } else {
      numDebounced = continueDebouncing();
    }
     
    // if (numEvents == 0) {
    //   numEvents = updateDebouncingStates();
    // }
    if (numEvents > 0 || numDebounced > 0) {
      // qDebug() << "Events: " << numEvents << " Debounced: " << numDebounced;
      haveCallback = callbackWithChangedLineStates();
    }
  }
}

bool
DigitalInputLinesRequest::callbackWithChangedLineStates()
{
  // LineStateMap lines;
  // updateLineStates();
  // for ( auto& lineState : m_lineStates) {
  //   if (lineState.changed) {
  //     lines[lineState.line] = lineState;
  //     lineState.changed = false;
  //     if (lineState.debounce) {
  //       lineState.firstEdgeTime = 0;
  //       lineState.isDebounced = false;
  //     }
      
  //   }
  // }
  // if (lines.empty()) {
  //   return true;
  // }
  std::lock_guard<std::mutex> lock(m_callbackMutex);
  if (m_pCallback) {
    m_pCallback->get().callback(m_lineStates);
    return true;
  }
  return false;
}

bool
DigitalInputLinesRequest::callbackWithAnyDebouncedLineStates()
{
  // uint64_t now = getCurrentTime();
  // LineStateMap debouncedLines;
  // updateLineStates();
  // for ( auto& lineState : m_lineStates) {
  //   if (lineState.debounce
  //     && !lineState.isDebounced
  //     && lineState.firstEdgeTime > 0
  //     && now - lineState.firstEdgeTime > m_debouncePeriod
  //     ) {
  //     lineState.isDebounced = true;
  //     lineState.firstEdgeTime = 0;
  //     lineState.changed = false;
  //     debouncedLines[lineState.line] = lineState;
  //     qDebug() << "callbackWithAnyDebouncedLineStates(): Line " << lineState.line << " debounced to " << (int)lineState.value;
  //   }
  // }
  // if (debouncedLines.empty()) {
  //   return true;
  // }
  // // qDebug() << "Found " << debouncedLines.size() << " debounced lines";
  // std::lock_guard<std::mutex> lock(m_callbackMutex);
  // if (m_pCallback) {
  //   m_pCallback->callback(debouncedLines);
  //   return true;
  // }
  return false;
}


int
DigitalInputLinesRequest::updateLineStates()
{
  int numEvents = readEdgeEvents(m_pEventBuffer, 64);
  if (numEvents < 0) {
    return 0;;
  }
  for (int i = 0; i < numEvents; ++i) {
    gpiod_edge_event* ev = gpiod_edge_event_buffer_get_event(m_pEventBuffer, i);
    uint32_t line = gpiod_edge_event_get_line_offset(ev);
    gpiod_edge_event_type type = gpiod_edge_event_get_event_type(ev);
    uint64_t timestamp = gpiod_edge_event_get_timestamp_ns(ev);
    LineState& lineState = m_lineStates[line];
    lineState.changed = true;
    if (lineState.firstEdgeTime == 0) {
      lineState.firstEdgeTime = timestamp;
    }
    if (type == GPIOD_EDGE_EVENT_RISING_EDGE) {
      lineState.lastRisingTime = timestamp;
      lineState.value = 1;
    } else {
      lineState.lastFallingTime = timestamp;
      lineState.value = 0;
    }
    if (lineState.debounce && !lineState.isDebounced) {
      lineState.isDebounced = timestamp - lineState.firstEdgeTime > m_debouncePeriod;
      // qDebug() << "updateLineStates(): Line " << lineState.line << " changed to " << (int)lineState.value
      //          << (lineState.isDebounced ? " (debounced)" : " (not debounced)");
    }
  }
  return numEvents;
}

int
DigitalInputLinesRequest::continueDebouncing()
{
  uint64_t now = getCurrentTime();
  int numDebounced = 0;
  for ( auto& lineState : m_lineStates) {
    if (lineState.debounce
      && !lineState.isDebounced
      && lineState.firstEdgeTime > 0
      && now - lineState.firstEdgeTime > m_debouncePeriod
      ) {
        lineState.isDebounced = true;
        lineState.firstEdgeTime = 0;
        lineState.changed = true;
        ++numDebounced;
        // qDebug() << "updateDebouncingStates(): Line " << lineState.line << " debounced to " << (int)lineState.value;
      }
  }
  return numDebounced;
}

void
DigitalInputLinesRequest::initialiseLineStates(const DigitalInputVariantVector& inputs)
{
  // m_lineStates.clear();

  for (const auto& input: inputs) {
    visit([&](const auto& concreteInput)
    {
      for (auto lineNo : concreteInput.getLines()) {
        const int value = getLineValue(lineNo);
        const LineState info{
          .line = lineNo,
          .debounce = concreteInput.getDebounce(),
          .isDebounced = false,
          // .candidateValue = static_cast<uint8_t>(value),
          // .candidateEdgeTime = 0,
          .firstEdgeTime = 0,
          .lastRisingTime = 0,
          .lastFallingTime = 0,
          .value = static_cast<uint8_t>(value),
          .changed = false,
          .processed = false
        };
        m_lineStates[lineNo] = info;
      }
    }, input);
  }
}
