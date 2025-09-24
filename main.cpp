#include "mainwindow.h"

#include <QApplication>
#include "dsp/utils/FftData.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/GpioException.h"
#include "io/control/device/gpio/GpioLines.h"
using json = nlohmann::json;

int main(int argc, char *argv[])
{
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  RadioConfig radioConfig;
  std::ifstream f("/home/murray/.config/cutesdr-vk6hl/cutesdr-vk6hl.json");
  json config = json::parse(f);
  if (config.contains("radio")) {
    radioConfig.initialise(config["radio"]);
  } else {
    qDebug() << "No radio config found";
  }

  try {
    Gpio::getInstance().open();
  } catch (GpioException& gpioErr) {
    qDebug() << gpioErr.what();
  }

  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication a(argc, argv);
  MainWindow w(radioConfig);
  w.show();
  int rc = a.exec();

  Gpio::getInstance().close();
  return rc;
}
