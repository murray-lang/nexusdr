//
// Created by murray on 2025-08-24.
//

#include "DigitalInputGroup.h"
#include <config/ConfigException.h>
#include "../../../ControlSource.h"
#include "../../../ControlSourceFactory.h"
#include <poll.h>
#include <qdebug.h>
#include <config/DigitalInputConfig.h>

#include "DigitalInputFactory.h"
#include "config/DigitalInputGroupConfig.h"

DigitalInputGroup::DigitalInputGroup(const char* consumer) :
  m_lines(consumer),
  m_running(false)
{

}

DigitalInputGroup::~DigitalInputGroup()
{
  deleteInputs();

}

void
DigitalInputGroup::configure(const ConfigBase* pConfig)
{
  const DigitalInputGroupConfig* config = dynamic_cast<const DigitalInputGroupConfig*>(pConfig);
  createInputs(config);
}

bool
DigitalInputGroup::discover()
{
    return Gpio::getInstance().isPresent();
}

void
DigitalInputGroup::open()
{
  if (!m_running) {
    std::vector<uint32_t> lines = gatherLinesFromInputs();
    m_lines.request(lines, GPIOD_LINE_DIRECTION_INPUT, GPIOD_LINE_BIAS_PULL_UP, GPIOD_LINE_EDGE_BOTH);
    m_running = true;
    QThread::start();
  }

}

void
DigitalInputGroup::close()
{
  if (m_running) {
    m_running = false;
    wait();
    m_lines.release();
  }
}

void
DigitalInputGroup::exit()
{

}

void
DigitalInputGroup::createInputs(const DigitalInputGroupConfig* pConfig)
{
  deleteInputs();
  for (const auto& pInputConfig : pConfig->getInputs()) {
    DigitalInput* input = DigitalInputFactory::create(pInputConfig, m_lines);
    if (input == nullptr) {
      throw ConfigException("digitalInputGroup input has unknown input type: " + pConfig->getType());
    }
    m_inputs.push_back(input);
  }
}

void
DigitalInputGroup::deleteInputs()
{
  for (auto input : m_inputs) {
    delete input;
  }
  m_inputs.clear();
}

std::vector<uint32_t>
DigitalInputGroup::gatherLinesFromInputs()
{
  m_lineToInputMap.clear();

  std::vector<uint32_t> result;
  for (auto input : m_inputs) {
    const std::vector<uint32_t>& lines = input->getLines();
    result.insert(result.end(), lines.begin(), lines.end());
    for (auto line : lines) {
      m_lineToInputMap[line] = input;
    }
  }
  return result;
}

void
DigitalInputGroup::readInitialInputStates()
{
  // for (auto encoder : m_encoders) {
  //   encoder->initialiseState(m_lines);
  // }
}

void
DigitalInputGroup::run()
{
  readInitialInputStates();
  // gpiod_edge_event_buffer* eventBuff = gpiod_edge_event_buffer_new(64);
  // if (!eventBuff) {
  //   return; //TODO: flag the error
  // }

  GpioLines::LineStateMap debouncedLines;

  constexpr int64_t idleTimeout = 100'000'000;
  while (m_running) {
    // int numChanges = m_lines.debounce(changes);
    // for (const auto& change : changes) {
    //   qDebug() << "Line " << change.line << " changed to " << (int)change.value;
    // }
    int wr = m_lines.waitEdgeEvents(idleTimeout);
    if (wr < 0) {
      // error; consider logging and continuing or breaking
      continue;
    }
    if (wr == 0) {
      // timeout: do periodic tasks if needed
      continue;
    }
    qDebug() << "-----------------------";
    debouncedLines.clear();
    int numChanges = m_lines.debounce(debouncedLines);
    int numHandled = 0;
    if (numChanges > 0) {
      for (auto& input : m_inputs) {
        if (input->handleLineChange(debouncedLines)) {
          numHandled++;
        }
      }
    }
    if (numHandled == 0 && numChanges > 0) {
      qDebug() << "Unhandled changes:";
      for (auto& infoPair : debouncedLines) { 
        GpioLines::LineState& info = infoPair.second;
        qDebug() << "Line: " << info.line << ", LastRisingTime: " << info.lastRisingTime << ", LastFallingTime: " << info.lastFallingTime << ", Value: " << (int)info.value;
      }
      
    }
  }
}
