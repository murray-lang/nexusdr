message(STATUS "including CMakeLists_AudioLibs.txt")
set(AUDIO_LINK_LIBS)

include(FindPkgConfig)

# Check for Jack (any OS)
find_library(JACK_LIB jack)
if(JACK_LIB OR jack_FOUND)
  message(STATUS "Jack found")
else()
  message(STATUS "Jack not found")
endif()

if (JACK_LIB OR jack_FOUND)
    if(jack_FOUND)
      list(APPEND AUDIO_LINK_LIBS ${JACK_LIBRARIES})
      list(APPEND INCDIRS ${JACK_INCLUDE_DIRS})
    elseif(JACK_LIB)
      list(APPEND AUDIO_LINK_LIBS ${JACK_LIB})
    endif()
endif()

# Check for Pulse (any OS)
pkg_check_modules(pulse libpulse-simple)

find_package(Threads REQUIRED
  CMAKE_THREAD_PREFER_PTHREAD
  THREADS_PREFER_PTHREAD_FLAG
)
list(APPEND AUDIO_LINK_LIBS Threads::Threads)
if(${pulse_FOUND})
  find_library(PULSE_LIB pulse)
  find_library(PULSESIMPLE_LIB pulse-simple)
  list(APPEND AUDIO_LINK_LIBS ${PULSE_LIB} ${PULSESIMPLE_LIB})
endif()

# ALSA
find_package(ALSA)
if (NOT ALSA_FOUND)
  message(FATAL_ERROR "ALSA API requested but no ALSA dev libraries found")
endif()
list(APPEND INCDIRS ${ALSA_INCLUDE_DIR})
list(APPEND AUDIO_LINK_LIBS ${ALSA_LIBRARIES})


