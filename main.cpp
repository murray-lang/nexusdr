#include "mainwindow.h"

#include <QApplication>
#include "dsp/utils/FftData.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "io/gpio/Gpio.h"
#include "io/gpio/GpioException.h"
#include "io/gpio/GpioLines.h"
using json = nlohmann::json;

int main(int argc, char *argv[])
{
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  RadioConfig radioConfig;
  std::ifstream f("/home/murray/.config/cutesdr-vk6hl/cutesdr-vk6hl.json");
  json config = json::parse(f);
  if (config.contains("radio")) {
    radioConfig = RadioConfig::fromJson(config["radio"]);
  } else {
    qDebug() << "No radio config found";
  }

  Gpio gpio;
  GpioLines centreFreqEncoder(&gpio, "centreFreqEncoder");
  std::vector<uint32_t> centreFreqEncoderLines = {20, 21};

  try {
    gpio.open();
    centreFreqEncoder.request(centreFreqEncoderLines, GPIOD_LINE_DIRECTION_INPUT, GPIOD_LINE_BIAS_PULL_UP, GPIOD_LINE_EDGE_BOTH);
  } catch (GpioException& gpioErr) {
    qDebug() << gpioErr.what();
  }

  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication a(argc, argv);
  MainWindow w(radioConfig);
  w.show();
  int rc = a.exec();

  centreFreqEncoder.release();
  gpio.close();
  return rc;
}
