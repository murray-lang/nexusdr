#include "DigitalOutputs.h"
#include "DigitalOutputFactory.h"


#ifdef USE_ETL
using etl::visit;
#else
using std::visit;
#endif


ResultCode
DigitalOutputs::configure(const Config::DigitalOutputs::Fields& config)
{
  return createOutputs(config);
}

bool
DigitalOutputs::discover()
{
  return Gpio::isPresent();
}

ResultCode
DigitalOutputs::open()
{
  for (auto& output : m_outputs) {
    ResultCode rc = ResultCode::OK;
    visit([&rc](auto&& dov)
    {
      rc = dov.open();
    }, output);
    if (rc != ResultCode::OK) {
      return rc;
    }
  }
  return ResultCode::OK;
}

void
DigitalOutputs::close()
{
  for (auto& output : m_outputs) {
    visit([](auto&& dov)
    {
      dov.close();
    }, output);
  }
}

void
DigitalOutputs::exit()
{

}

ResultCode
DigitalOutputs::createOutputs(const Config::DigitalOutputs::Fields& config)
{
  m_outputs.clear();
  ResultCode rc = ResultCode::OK;
  for (const auto& inputConfig : config.outputs) {
    DigitalOutputVariant digitalOutput;
    rc = DigitalOutputFactory::create(inputConfig, digitalOutput);
    if (rc == ResultCode::OK) {
      m_outputs.emplace_back(std::move(digitalOutput));
    }
    return rc;
  }
  return rc;
}