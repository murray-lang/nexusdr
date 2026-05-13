#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include "core/dsp/utils/FftData.h"

#include <fstream>
#include <ArduinoJson.h>
#include <iostream>

#include <etl/string.h>

#include "core/radio/qt/QtGlobalEventTargets.h"
#include "core/radio/qt/QtMeteringDispatcher.h"
#include "core/radio/qt/QtMonitorDispatcher.h"
#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/digital/DigitalInputLinesRequest.h"

ResultCode loadRadioConfig(const QString& configHome, Config::Radio::Fields& radioConfig)
{
  ResultCode rc = ResultCode::OK;
  // Resolve config path under the current user's home directory: ~/.config/nexusdr/nexusdr.json

  const QString configPath = configHome + "/nexusdr.json";
  if (QFile::exists(configPath)) {
    try {
      std::ifstream f(configPath.toStdString());
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, f);
      if (error) {
        qDebug() << "Failed to parse config at" << configPath << ":" << error.c_str();
        rc = ResultCode::ERR_CONFIG_INVALID_JSON;
      } else if (doc["radio"]) {
        // Prefer fromJson for symmetry with toJson()
        rc = Config::Radio::fromJson(doc["radio"].as<JsonVariantConst>(), radioConfig);
      } else {
        qDebug() << "Config file present but no 'radio' section found:" << configPath;
        rc = ResultCode::ERR_CONFIG_MISSING_RADIO;
      }
    } catch (const std::exception& ex) {
      qDebug() << "Failed to read config at" << configPath << ":" << ex.what();
      rc = ResultCode::ERR_CONFIG_FILE_READ_ERROR;
    }
  } else {
    qDebug() << "No config file found at" << configPath << "; using defaults.";
    rc = ResultCode::ERR_CONFIG_FILE_MISSING;
  }
  return rc;
}

ResultCode loadStylesheets(const QString& configHome, QApplication& app)
{
  ResultCode rc = ResultCode::OK;
  const QString styleSheetPath = configHome + "/nexusdr.qss";
  const QString standardFaceQssPath = configHome + "/StandardFace.qss";

  auto readQssFile = [](const QString& path, QString& qss) -> ResultCode {
    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
      return ResultCode::ERR_UI_STYLESHEET_OPEN;
    }
    QTextStream ts(&f);
    qss = ts.readAll();
    return ResultCode::OK;
  };

  QString combinedQss;
  QString baseQss;
  rc = readQssFile(styleSheetPath, baseQss);
  if (rc != ResultCode::OK) {
    qDebug() << "Error " << static_cast<uint32_t>(rc) << " reading stylesheet from:" << styleSheetPath;
    return rc;
  }
  if (baseQss.isEmpty()) {
    return ResultCode::ERR_UI_EMPTY_STYLESHEET;
  }
  combinedQss += baseQss;
  combinedQss += "\n";

  QString faceQss;
  rc = readQssFile(standardFaceQssPath, faceQss);
  if (rc == ResultCode::OK && !faceQss.isEmpty()) {
    combinedQss += "\n/* ---- StandardFace.qss ---- */\n";
    combinedQss += faceQss;
    combinedQss += "\n";
    app.setStyleSheet(combinedQss);
  }
  return rc;
}

ResultCode initialiseRadio(const Config::Radio::Fields& radioConfig, Radio& radio)
{
  ResultCode rc = radio.configure(radioConfig);
  if (rc != ResultCode::OK) {
    qDebug() << "Error configuring radio: " << (uint32_t)rc;
    return rc;
  }
  radio.start();

  radio.applyBand("20m");
  radio.applyAgcSpeed(AgcSpeed::OFF);
  //m_pRadio->split("40m", "20m");
  // m_pRadio->addPipeline();

  RfSettings rfSettings;
  rfSettings.setGain(30.0);
  rfSettings.setGainCoarseStep(1.0);
  rfSettings.setGainFineStep(0.1);
  radio.applyRfSettings(rfSettings, true);

  // int32_t centreFreqStepCoarse = 50000;
  // m_pRadio->applySetting("pipeline.rx-pipeline.rf.centre-frequency.coarse-step", centreFreqStepCoarse);

  IfSettings ifSettings;
  ifSettings.setGain(0.0);
  ifSettings.setBandwidth(200000);
  radio.applyIfSettings(ifSettings);

  radio.markAllSettingsUnchanged();
  return ResultCode::OK;
}

QtMeteringDispatcher m_meteringDispatcher([]() { return globalMeteringClientEventTarget;} );
QtMonitorDispatcher m_monitorDispatcher([]() { return globalMonitorClientEventTarget;} );

Radio radio(&m_meteringDispatcher, &m_monitorDispatcher);

int main(int argc, char *argv[])
{
#ifdef USE_GPIO

  ResultCode rc1 = Gpio::getInstance().open();
  if (rc1 != ResultCode::OK) {
    qDebug() << "Error opening GPIO instance.";
    return 1;
  }
#endif
  const QString configHome = QDir::homePath() + "/.config/nexusdr";
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  Config::Radio::Fields radioConfig;
  ResultCode rc = loadRadioConfig(configHome, radioConfig);
  if (rc != ResultCode::OK) {
    qDebug() << "Error loading radio config: " << static_cast<uint32_t>(rc);
    return 1;
  }

  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication app(argc, argv);

  rc = loadStylesheets(configHome, app);
  if (rc != ResultCode::OK) {
    qDebug() << "Error loading stylesheets: " << static_cast<uint32_t>(rc);
    return 1;
  }

  rc = initialiseRadio(radioConfig, radio);
  if (rc != ResultCode::OK) {
    qDebug() << "Error initialising radio: " << static_cast<uint32_t>(rc);
    return 1;
  }

  MainWindow w(radioConfig);

  // radio.setEventTarget(&w);

  w.show();
  int rc2 = app.exec();

  radio.stop();

#ifdef USE_GPIO
  Gpio::getInstance().close();
#endif

  return rc2;
}
