//
// Created by murray on 18/9/25.
//

#include "DigitalInputsRequestImplGpiod.h"
#include "../../digital/DigitalInput.h"
#include "../../GpioException.h"
#include <time.h>
#include <qdebug.h>

DigitalInputsRequestImplGpiod::DigitalInputsRequestImplGpiod(gpiod_chip* pChip, const char* consumer) :
  m_pChip   (pChip),  
  m_pCallback(nullptr),
  m_consumer(consumer),
  m_pLineRequest(nullptr),
  m_debouncePeriod(200'000'000)

{
  m_pEventBuffer = gpiod_edge_event_buffer_new(64);
  
}

DigitalInputsRequestImplGpiod::~DigitalInputsRequestImplGpiod()
{
  gpiod_edge_event_buffer_free(m_pEventBuffer);
}

bool
DigitalInputsRequestImplGpiod::isDebounced(int line) const
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
DigitalInputsRequestImplGpiod::getCurrentTime()
{
  timespec now{};
  clock_gettime(CLOCK_MONOTONIC, &now);
  return static_cast<uint64_t>(now.tv_sec) * 1'000'000'000ULL + now.tv_nsec;

}

void
DigitalInputsRequestImplGpiod::startCallbacks(Callback* callback)
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
DigitalInputsRequestImplGpiod::stopCallbacks()
{
  m_callbackMutex.lock();
  m_pCallback = nullptr;
  m_callbackMutex.unlock();
  wait();
}

void 
DigitalInputsRequestImplGpiod::request(const char * contextId, const std::vector<DigitalInput*>& inputs)
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
    throw GpioException("Failed to request GPIO lines");
  }
  m_pLineRequest = pLineRequest;
  initialiseLineStates(inputs);
}


void
DigitalInputsRequestImplGpiod::release()
{
  if (m_pLineRequest) {
    gpiod_line_request_release(m_pLineRequest);
    m_pLineRequest = nullptr;
  }
}

int
DigitalInputsRequestImplGpiod::getLineValue(uint32_t line)
{
  return gpiod_line_request_get_value(m_pLineRequest, line);
}

int
DigitalInputsRequestImplGpiod::waitEdgeEvents(int64_t timeout_ns) const
{
  return gpiod_line_request_wait_edge_events(m_pLineRequest, timeout_ns);
}

int
DigitalInputsRequestImplGpiod::readEdgeEvents(struct gpiod_edge_event_buffer* buf, size_t max_events) const
{
  return gpiod_line_request_read_edge_events(m_pLineRequest, buf, max_events);
}

void
DigitalInputsRequestImplGpiod::run()
{
  // readInitialInputStates();
  // gpiod_edge_event_buffer* eventBuff = gpiod_edge_event_buffer_new(64);
  // if (!eventBuff) {
  //   return; //TODO: flag the error
  // }

  LineStateMap debouncedLines;

  bool haveCallback = m_pCallback != nullptr;
  while (haveCallback) {
    constexpr int64_t idleTimeout = 200'000'000;
    int wr = waitEdgeEvents(idleTimeout);
    if (wr < 0) {
      // error; consider logging and continuing or breaking
      continue;
    }
    if (wr == 0) {
      haveCallback = callbackWithAnyDebouncedLineStates();
      continue;
    }
    // qDebug() << "-----------------------";
    // debouncedLines.clear();
    // int numChanges = getLineStateChanges(debouncedLines);
    // int numChanges = debounce(debouncedLines);
    updateLineStates();
    haveCallback = callbackWithChangedLineStates();
    // int numChanges = 0;
    // for ( auto& lineState : m_lineStates) {
    //   if (lineState.changed) {
    //     debouncedLines[lineState.line] = lineState;
    //     lineState.changed = false;
    //     ++numChanges;
    //   }
    // }
    // if (numChanges > 0) {
    //   std::lock_guard<std::mutex> lock(m_callbackMutex);
    //   if (m_pCallback) {
    //     m_pCallback->callback(debouncedLines);
    //   } else {
    //     haveCallback = false;
    //   }
    // }
    // if (numChanges > 0) {
    //   qDebug() << "Changes:";
    //   for (auto& infoPair : debouncedLines) {
    //     GpioLinesRequest::LineState& info = infoPair.second;
    //     qDebug() << "Line: " << info.line << ", LastRisingTime: " << info.lastRisingTime << ", LastFallingTime: " << info.lastFallingTime << ", Value: " << (int)info.value;
    //   }
    // }
  }
}

bool
DigitalInputsRequestImplGpiod::callbackWithChangedLineStates()
{
  LineStateMap lines;
  updateLineStates();
  for ( auto& lineState : m_lineStates) {
    if (lineState.changed) {
      lines[lineState.line] = lineState;
      lineState.changed = false;
    }
  }
  if (lines.empty()) {
    return true;
  }
  std::lock_guard<std::mutex> lock(m_callbackMutex);
  if (m_pCallback) {
    m_pCallback->callback(lines);
    return true;
  }
  return false;
}

bool
DigitalInputsRequestImplGpiod::callbackWithAnyDebouncedLineStates()
{
  uint64_t now = getCurrentTime();
  LineStateMap debouncedLines;
  updateLineStates();
  for ( auto& lineState : m_lineStates) {
    if (lineState.debounce
      && !lineState.isDebounced
      && lineState.candidateEdgeTime > 0
      && now - lineState.candidateEdgeTime > m_debouncePeriod
      ) {
      lineState.isDebounced = true;
      lineState.candidateEdgeTime = 0;
      lineState.changed = false;
      debouncedLines[lineState.line] = lineState;
    }
  }
  if (debouncedLines.empty()) {
    return true;
  }
  std::lock_guard<std::mutex> lock(m_callbackMutex);
  if (m_pCallback) {
    m_pCallback->callback(debouncedLines);
    return true;
  }
  return false;
}

int
DigitalInputsRequestImplGpiod::getLineStateChanges(LineStateMap& changes)
{
  return 0;
  // int numEvents = readEdgeEvents(m_pEventBuffer, 64);
  // if (numEvents < 0) {
  //   throw GpioException("Error reading edge events");
  // }
  // for (int i = 0; i < numEvents; ++i) {
  //
  //   gpiod_edge_event* ev = gpiod_edge_event_buffer_get_event(m_pEventBuffer, i);
  //   uint32_t line = gpiod_edge_event_get_line_offset(ev);
  //   gpiod_edge_event_type type = gpiod_edge_event_get_event_type(ev);
  //   uint64_t timestamp = gpiod_edge_event_get_timestamp_ns(ev);
  //
  //   auto stateIter = m_lineStates.find(line);
  //   // if (stateIter == m_lineStates.end()) {
  //   //   throw GpioException("Received event for unknown line");
  //   // }
  //   if (stateIter != m_lineStates.end()) {
  //     LineState& info = stateIter->second;
  //     info.changed = true;
  //     if (type == GPIOD_EDGE_EVENT_RISING_EDGE) {
  //       info.lastRisingTime = timestamp;
  //       info.value = 1;
  //     } else {
  //       info.lastFallingTime = timestamp;
  //       info.value = 0;
  //     }
  //     changes[line] = info;
  //   }
  // }
  // // Now read the affected line values to make sure we have that part of the story straight.
  // // for (auto& pair : m_lineStates) {
  // //   LineState& info = pair.second;
  // //   if (info.changed) {
  // //     info.value = static_cast<uint8_t>(getLineValue(pair.first));
  // //     changes[pair.first] = info;
  // //     info.changed = false;
  // //   }
  // // }
  // return numEvents;
}

int
DigitalInputsRequestImplGpiod::updateLineStates()
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
    if (type == GPIOD_EDGE_EVENT_RISING_EDGE) {
      lineState.lastRisingTime = timestamp;
      lineState.value = 1;
    } else {
      lineState.lastFallingTime = timestamp;
      lineState.value = 0;
    }
    if (lineState.debounce) {
      lineState.isDebounced = false;
      if (lineState.candidateEdgeTime == 0) {
        lineState.candidateEdgeTime = timestamp;
        lineState.candidateValue = lineState.value;
        lineState.changed = false;
      } else if (lineState.candidateValue != lineState.value) {
        lineState.candidateEdgeTime = 0;
        lineState.changed = false;
      } else if (timestamp - lineState.candidateEdgeTime > m_debouncePeriod) {
        lineState.isDebounced = true;
        lineState.candidateEdgeTime = 0;
        lineState.changed = true;
      }
    } else {
      lineState.changed = true;
    }
  }
  return numEvents;
}

int
DigitalInputsRequestImplGpiod::debounce(LineStateMap& changes)
{
  return 0;
  // constexpr int64_t debounceTimeout= 45'000'000;
  // //std::unordered_map<uint32_t, DebounceInfo> m_debouncedLines;
  // int waitResult = waitEdgeEvents(debounceTimeout);
  //
  // if (waitResult < 0) {
  //   throw GpioException("Error waiting for edge events");
  // }
  // // Whizz through the event storm until (we think) it's over
  // while(waitResult != 0) {
  //   int numEvents = readEdgeEvents(m_pEventBuffer, 64);
  //
  //   for (int i = 0; i < numEvents; ++i) {
  //
  //     gpiod_edge_event* ev = gpiod_edge_event_buffer_get_event(m_pEventBuffer, i);
  //     uint32_t line = gpiod_edge_event_get_line_offset(ev);
  //     gpiod_edge_event_type type = gpiod_edge_event_get_event_type(ev);
  //     uint64_t timestamp = gpiod_edge_event_get_timestamp_ns(ev);
  //
  //     auto stateIter = m_lineStates.find(line);
  //     if (stateIter == m_lineStates.end()) {
  //       throw GpioException("Received event for unknown line");
  //     }
  //     LineState& info = stateIter->second;
  //     info.changed = true;
  //     if (type == GPIOD_EDGE_EVENT_RISING_EDGE) {
  //       info.lastRisingTime = timestamp;
  //       info.value = 1;
  //     } else {
  //       info.lastFallingTime = timestamp;
  //       info.value = 0;
  //     }
  //     changes[line] = info;
  //   }
  //   waitResult = waitEdgeEvents(debounceTimeout);
  //   if (waitResult < 0) {
  //     throw GpioException("Error waiting for edge events");
  //   }
  // }
  // // Now read the affected line values to make sure we have that part of the story straight.
  // // for (auto& pair : m_lineStates) {
  // //   LineState& info = pair.second;
  // //   if (info.changed) {
  // //     info.value = static_cast<uint8_t>(getLineValue(pair.first));
  // //     changes[pair.first] = info;
  // //     info.changed = false;
  // //   }
  // // }
  // return changes.size();
}