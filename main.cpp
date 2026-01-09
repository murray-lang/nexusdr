#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include "dsp/utils/FftData.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/GpioException.h"
#include "io/control/device/gpio/digital/DigitalInputLinesRequest.h"
using json = nlohmann::json;

int main(int argc, char *argv[])
{
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  RadioConfig radioConfig;
  bool dumpConfig = false;
  bool dumpSchema = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (arg == "--dump-config") dumpConfig = true;
    if (arg == "--dump-config-schema" || arg == "--dump-schema") dumpSchema = true;
  }
  // Resolve config path under the current user's home directory: ~/.config/cutesdr-vk6hl/cutesdr-vk6hl.json
  const QString configHome = QDir::homePath() + "/.config/cutesdr-vk6hl";
  const QString configPath = configHome + "/cutesdr-vk6hl.json";
  if (QFile::exists(configPath)) {
    try {
      std::ifstream f(configPath.toStdString());
      json config = json::parse(f);
      if (config.contains("radio")) {
        // Prefer fromJson for symmetry with toJson()
        radioConfig.fromJson(config["radio"]);
      } else {
        qDebug() << "Config file present but no 'radio' section found:" << configPath;
      }
    } catch (const std::exception& ex) {
      qDebug() << "Failed to parse config at" << configPath << ":" << ex.what();
    }
  } else {
    qDebug() << "No config file found at" << configPath << "; using defaults.";
  }

  if (dumpConfig) {
    nlohmann::json out = nlohmann::json{{"radio", radioConfig.toJson()}};
    std::cout << out.dump(2) << std::endl;
    return 0;
  }

  try {
    Gpio::getInstance().open();
  } catch (GpioException& gpioErr) {
    qDebug() << gpioErr.what();
  }

  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication app(argc, argv);

  const QString styleSheetPath = configHome + "/cutesdr-vk6hl.qss";
  QFile file(styleSheetPath);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream ts(&file);
    app.setStyleSheet(ts.readAll());
    file.close();
    qDebug() << "Loaded custom stylesheet from:" << styleSheetPath;
  }

  MainWindow w(radioConfig);
  w.show();
  int rc = app.exec();

  Gpio::getInstance().close();
  return rc;
}
