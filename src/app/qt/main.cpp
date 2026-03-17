#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include "core/dsp/utils/FftData.h"

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
  // Resolve config path under the current user's home directory: ~/.config/nexusdr/nexusdr.json
  const QString configHome = QDir::homePath() + "/.config/nexusdr";
  const QString configPath = configHome + "/nexusdr.json";
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
#ifdef USE_GPIO
  try {
    Gpio::getInstance().open();
  } catch (GpioException& gpioErr) {
    qDebug() << gpioErr.what();
  }
#endif
  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication app(argc, argv);

  const QString styleSheetPath = configHome + "/nexusdr.qss";
  const QString standardFaceQssPath = configHome + "/StandardFace.qss";

  auto readQssFile = [](const QString& path) -> QString {
    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
      return {};
    }
    QTextStream ts(&f);
    return ts.readAll();
  };

  QString combinedQss;
  const QString baseQss = readQssFile(styleSheetPath);
  if (!baseQss.isEmpty()) {
    combinedQss += baseQss;
    combinedQss += "\n";
  }

  const QString faceQss = readQssFile(standardFaceQssPath);
  if (!faceQss.isEmpty()) {
    combinedQss += "\n/* ---- StandardFace.qss ---- */\n";
    combinedQss += faceQss;
    combinedQss += "\n";
  }

  if (!combinedQss.isEmpty()) {
    app.setStyleSheet(combinedQss);
    qDebug() << "Loaded stylesheets from:" << styleSheetPath << "and" << standardFaceQssPath;
  } else {
    qDebug() << "No stylesheets loaded (missing/empty):" << styleSheetPath << "and" << standardFaceQssPath;
  }

  MainWindow w(radioConfig);
  w.show();
  int rc = app.exec();

#ifdef USE_GPIO
  Gpio::getInstance().close();
#endif

  return rc;
}
