#pragma once

#include "AudioOutput.h"
#include "CrossPlatformTypes.h"

using AudioOutputVariant = variant<monostate, AudioOutput>;