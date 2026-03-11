//
// Created by murray on 18/9/25.
//

#include "DigitalInputLinesRequestImplGpiod.h"
#include "../../digital/DigitalInput.h"
#include "../../GpioException.h"
#include <time.h>
#include <qdebug.h>

DigitalInputLinesRequestImplGpiod::DigitalInputLinesRequestImplGpiod(gpiod_chip* pChip, const char* consumer) :
  m_pChip   (pChip),  
  m_pCallback(nullptr),
  m_consumer(consumer),
  m_pLineRequest(nullptr),
  m_debouncePeriod(5'000'000)

{
  m_pEventBuffer = gpiod_edge_event_buffer_new(64);
  
}

DigitalInputLinesRequestImplGpiod::~DigitalInputLinesRequestImplGpiod()
{
  gpiod_edge_event_buffer_free(m_pEventBuffer);
  DigitalInputLinesRequestImplGpiod::release();
}

bool
DigitalInputLinesRequestImplGpiod::isDebounced(int line) const
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
DigitalInputLinesRequestImplGpiod::getCurrentTime()
{
  timespec now{};
  clock_gettime(CLOCK_MONOTONIC, &now);
  return static_cast<uint64_t>(now.tv_sec) * 1'000'000'000ULL + now.tv_nsec;

}

void
DigitalInputLinesRequestImplGpiod::startCallbacks(Callback* callback)
{
  if (m_pCallback != nullptr) {
    throw GpioException("Line state callback already set");
  }
  // std::lock_guard<std::mutex> lock(m_callbackMutex);
  m_callbackMutex.lock();
  m_pCallback = callback;
  m_callbackMutex.unlock();
  QThread::start();
}

void
DigitalInputLinesRequestImplGpiod::stopCallbacks()
{
  m_callbackMutex.lock();
  m_pCallback = nullptr;
  m_callbackMutex.unlock();
  wait();
}

void 
DigitalInputLinesRequestImplGpiod::request(const char * contextId, const std::vector<DigitalInput*>& inputs)
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
    throw GpioException("Failed to allocate line config");
  }
  for (const auto& input : inputs) {
    gpiod_line_settings *ls = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(ls, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(ls, static_cast<gpiod_line_bias>(input->getBias()));
    gpiod_line_settings_set_edge_detection(ls, static_cast<gpiod_line_edge>(input->getEdge()));
    // gpiod_line_settings_set_debounce_period_us(ls, 2000);
    const std::vector<uint32_t>& lineNos = input->getLines();
    gpiod_line_config_add_line_settings(lcfg, lineNos.data(), lineNos.size(), ls);
    gpiod_line_settings_free(ls);
  }


  gpiod_request_config *rcfg = gpiod_request_config_new();
  gpiod_request_config_set_consumer(rcfg, contextId);

  gpiod_line_request *pLineRequest = gpiod_chip_request_lines(m_pChip, rcfg, lcfg);
  gpiod_request_config_free(rcfg);
  gpiod_line_config_free(lcfg);
  if (pLineRequest == nullptr) {
    throw GpioException("Failed to request GPIO digital input lines");
  }
  m_pLineRequest = pLineRequest;
  initialiseLineStates(inputs);
}


void
DigitalInputLinesRequestImplGpiod::release()
{
  if (m_pLineRequest) {
    gpiod_line_request_release(m_pLineRequest);
    m_pLineRequest = nullptr;
  }
}

int
DigitalInputLinesRequestImplGpiod::getLineValue(uint32_t line)
{
  return gpiod_line_request_get_value(m_pLineRequest, line);
}

int
DigitalInputLinesRequestImplGpiod::waitEdgeEvents(int64_t timeout_ns) const
{
  return gpiod_line_request_wait_edge_events(m_pLineRequest, timeout_ns);
}

int
DigitalInputLinesRequestImplGpiod::readEdgeEvents(struct gpiod_edge_event_buffer* buf, size_t max_events) const
{
  return gpiod_line_request_read_edge_events(m_pLineRequest, buf, max_events);
}

void
DigitalInputLinesRequestImplGpiod::run()
{
  // readInitialInputStates();
  // gpiod_edge_event_buffer* eventBuff = gpiod_edge_event_buffer_new(64);
  // if (!eventBuff) {
  //   return; //TODO: flag the error
  // }

  // LineStateMap debouncedLines;

  bool haveCallback = m_pCallback != nullptr;
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
DigitalInputLinesRequestImplGpiod::callbackWithChangedLineStates()
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
    m_pCallback->callback(m_lineStates);
    return true;
  }
  return false;
}

bool
DigitalInputLinesRequestImplGpiod::callbackWithAnyDebouncedLineStates()
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
DigitalInputLinesRequestImplGpiod::updateLineStates()
{
  int numEvents = readEdgeEvents(m_pEventBuffer, 64);
  if (numEvents < 0) {
    throw GpioException("Error reading edge events");
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
DigitalInputLinesRequestImplGpiod::continueDebouncing()
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

// int
// DigitalInputsRequestImplGpiod::debounce(LineStateMap& changes)
// {
//   return 0;
// }