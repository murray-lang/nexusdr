#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include "core/dsp/utils/FftData.h"

#include <fstream>
#include <ArduinoJson.h>
#include <iostream>

#include <etl/string.h>

#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/GpioException.h"
#include "io/control/device/gpio/digital/DigitalInputLinesRequest.h"

int main(int argc, char *argv[])
{
  etl::string<10> etlString;

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
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, f);
      if (error) {
        qDebug() << "Failed to parse config at" << configPath << ":" << error.c_str();
      } else if (doc["radio"]) {
        // Prefer fromJson for symmetry with toJson()
        radioConfig.fromJson(doc["radio"].as<JsonVariantConst>());
      } else {
        qDebug() << "Config file present but no 'radio' section found:" << configPath;
      }
    } catch (const std::exception& ex) {
      qDebug() << "Failed to read config at" << configPath << ":" << ex.what();
    }
  } else {
    qDebug() << "No config file found at" << configPath << "; using defaults.";
  }

  if (dumpConfig) {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    JsonObject radio = root["radio"].to<JsonObject>();
    radioConfig.toJson(radio);
    serializeJsonPretty(doc, std::cout);
    std::cout << std::endl;
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
