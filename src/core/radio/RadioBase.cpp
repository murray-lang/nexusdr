#include "RadioBase.h"

#include "core/config-settings/settings/base/SettingUpdateHelpers.h"
#include "core/config-settings/settings/events/SettingUpdateEvent.h"
#include "core/events/EventDispatcher.h"

RadioBase::RadioBase()
  : m_updateSequenceNo(0)
{

}

ResultCode
RadioBase::applySettingUpdate(SettingUpdate& update)
{
  if (m_settings.applyUpdate(update, m_bandSelector)) {
    return applySettings(m_settings);
  }
  return ResultCode::OK;
}

ResultCode
RadioBase::applySettingUpdates(SettingUpdate* updates, size_t count)
{
  if (!updates) return ResultCode::OK;

  bool anyChanged = false;

  for (size_t i = 0; i < count; ++i) {
    updates[i].resetCursor();
    anyChanged |= m_settings.applyUpdate(updates[i], m_bandSelector);
  }

  if (anyChanged) {
    return applySettings(m_settings);
  }
  return ResultCode::OK;
}

void
RadioBase::applyBand(const BandNameString& bandName)
{
  SettingUpdate update = SettingUpdateHelpers::makeSetBand(bandName);
  // qDebug() << "Radio::applyBand(): applying band " << bandName.c_str() << ". Existing band: " << m_settings.bandName.c_str() ;
  applySettingUpdate(update);
}

void
RadioBase::split(const BandNameString& bandA, const BandNameString& bandB)
{
  SettingUpdatePath splitPath({RadioSettings::BAND, ActiveBandSettings::SPLIT});
  SettingUpdate splitSetting(splitPath, true, SettingUpdate::Meaning::VALUE);
  applySettingUpdate(splitSetting);
}

void
RadioBase::applyAgcSpeed(AgcSpeed speed)
{
  SettingUpdate update = SettingUpdateHelpers::makeSetAgcSpeedOnFocusPipeline(speed);
  applySettingUpdate(update);
}