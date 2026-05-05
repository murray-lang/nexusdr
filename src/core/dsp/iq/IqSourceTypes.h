#pragma once
#include "CrossPlatformTypes.h"
#include "core/dsp/iq/AudioIqSource.h"
#include "core/dsp/iq/AudioSignalIqSource.h"
#include "io/audio/AudioOutput.h"

using IqSourceVariant = variant<monostate, AudioSignalIqSource, AudioIqSource>;
