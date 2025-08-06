//
// Created by murray on 4/08/25.
//

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QIODevice>
#include <regex>
#include "../AudioException.h"


class AudioDevice : public QIODevice
{
protected:
  Q_OBJECT

public:
  explicit AudioDevice(const QAudioFormat &format)
  : m_format(format)
  {

  }

  static QAudioDevice findInputDevice(const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>& devices = QMediaDevices::audioInputs();
    if (devices.isEmpty())
    {
      throw AudioException("No audio input devices were found");
    }
    const qsizetype deviceIndex = findDeviceIndex(devices, descriptionRegex);
    return devices.at(deviceIndex);
  }

  static QAudioDevice findOutputDevice(const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>& devices = QMediaDevices::audioOutputs();
    if (devices.isEmpty())
    {
      throw AudioException("No audio output devices were found");
    }
    const qsizetype deviceIndex = findDeviceIndex(devices, descriptionRegex);
    return devices.at(deviceIndex);
  }

  static qsizetype findDeviceIndex(const QList<QAudioDevice>& devices, const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>::const_iterator deviceIter = std::find_if(
        devices.begin(), devices.end(),
        [&descriptionRegex](const QAudioDevice& device) -> bool {
          std::basic_regex<char> regex(descriptionRegex, std::regex_constants::ECMAScript | std::regex_constants::icase);
          std::smatch match;
          std::string description = device.description().toStdString();
          return std::regex_search(description, match, regex);
        }
    );
    if (deviceIter == devices.end()) {
      std::ostringstream stringStream;
      stringStream << "An audio device with a name matched by '" << descriptionRegex << "' was not found";
      std::string copyOfStr = stringStream.str();
      throw AudioException(copyOfStr);
    }
    return std::distance(devices.begin(), deviceIter);
  }

protected:
  const QAudioFormat m_format;
};

#endif //AUDIODEVICE_H
