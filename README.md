# cutesdr-vk6hl
An SDR implementation drawing on the work of cutesdr (https://sourceforge.net/projects/cutesdr/)


Cross‑platform directory layout recommendations

This project already has a sensible separation (dsp, io, radio, settings, config, etc.). To make it easier to maintain across platforms (Linux, Windows, macOS) and to simplify packaging, consider converging on the following directory layout over time. This requires no immediate code changes; it’s a guide for incremental refactors and for placing new files.

Proposed top‑level layout
- cmake/ — CMake modules and toolchains (already present)
- docs/ — documentation, design notes, diagrams
- scripts/ — helper scripts (dev/build/CI); shell, PowerShell, Python
- third_party/ — vendored dependencies (e.g., pocketfft) with clear LICENSE files
- resources/ — icons, UI resources, translations, udev rules, etc.
  - resources/udev/ (e.g., 80-funcube.rules)
  - resources/translations/
- config/ — sample runtime configs and schemas (already present)
- src/ — all buildable source code
  - app/ — application entry points and UI
    - app/qt/ — main.cpp, mainwindow.* and .ui, Qt glue
  - core/ — platform‑agnostic domain logic
    - core/radio/ — Radio, receivers/transmitters (currently radio/)
    - core/dsp/ — DSP blocks and utilities (currently dsp/)
    - core/settings/ — settings domain and event bus (currently settings/)
    - core/util/ — common helpers (currently util/)
  - io/ — hardware and OS integration layers
    - io/audio/ — audio abstraction and device backends
      - device/ — platform or library specific (e.g., RtAudio)
    - io/control/ — GPIO, USB, dongle control
      - gpio/ — per‑platform implementations
      - usb/
      - devices/ — higher‑level device integrations (e.g., FunCube)
  - platform/ — optional per‑OS shims and implementations
    - linux/
    - windows/
    - macos/
  - include/ — public headers (if you decide to export a library/API)
- tests/ — unit and integration tests
- examples/ — example configs, sample audio, simple demo apps
- packaging/ — CI, installers (.deb/.rpm/.msi/.dmg), Flatpak manifests, brew formulae

Notes and mapping from current layout
- Keep the existing folders working; migrate gradually as you touch code.
- Current dsp/, radio/, settings/, io/, util/ map directly into src/core/* and src/io/*.
- main.cpp, mainwindow.* and .ui can live under src/app/qt/.
- 80-funcube.rules should move to resources/udev/ and be installed conditionally on Linux.
- pocketfft under dsp/utils could be placed under third_party/pocketfft with a README and license, while core code includes it via an internal include path.

Build system (CMake) tips
- Make src/ the root for targets and define interface libraries for clean separation:
  - add_library(core …) for src/core/**
  - add_library(io …) for src/io/**
  - add_library(app_qt …) for src/app/qt/** and link to core/io/Qt
  - target_link_libraries enforce one‑way dependencies: app -> core/io, io -> core (only if shared types), core -> third_party only
- Use target_sources with FILE_SETs to group headers per component for IDEs.
- Use generator expressions and options to include platform sources:
  - if(WIN32) target_sources(io PRIVATE src/platform/windows/…) etc.

Cross‑platform file locations at runtime
- Prefer QStandardPaths (AppConfigLocation, AppDataLocation) for config and cache instead of manually composing ~/.config. This ensures correctness on Windows and macOS.
- Install resources in conventional locations per platform (CMake’s GNUInstallDirs, bundle resources on macOS, per‑user AppData on Windows).

Incremental migration plan
1. Create src/ with app/, core/, io/ subfolders; move new files there; leave existing files where they are.
2. Update CMake to allow building from both old and new paths during the transition.
3. Move UI (mainwindow.*, .ui, main.cpp) to src/app/qt/.
4. Move dsp/, radio/, settings/ into src/core/ preserving namespaces.
5. Move io/ into src/io/ and split per‑backend when useful.
6. Relocate 80-funcube.rules to resources/udev/ and add an install rule guarded by UNIX AND NOT APPLE.

This structure helps:
- isolate platform‑specific code and make it easy to add new backends;
- clarify dependency directions and keep core logic portable;
- simplify packaging across Linux, Windows, and macOS.


Embedded/STM32 (no Qt) and RTOS/GuiX variants

If you also want to build this code for STM32 microcontrollers (no desktop Qt) and optionally use ThreadX and GUIX, extend the above layout with embedded/RTOS‑specific slices. The goal is to keep src/core/ fully portable and freestanding, while swapping app/ and io/ backends and adding platform glue under src/platform/.

Additional/adjusted layout
- src/
  - app/
    - app/qt/ — desktop Qt UI (unchanged)
    - app/guix/ — GUIX front‑end for RTOS targets (no Qt)
    - app/headless/ — non‑UI CLI or service entry points (useful for tests/benchmarks or minimal embedded shells)
  - core/ — remains platform‑agnostic; keep dependencies limited to the STL subset you can support on embedded (consider toggles for exceptions/RTTI)
  - io/
    - io/audio/
      - device/
        - device/rtaudio/ — desktop backend (existing)
        - device/stm32/ — STM32 audio out/in (I2S/SAI + DMA) backends
    - io/control/
      - gpio/
        - gpio/linux/ — libgpiod backend (existing)
        - gpio/stm32/ — STM32 HAL/LL backend
      - usb/
        - usb/stm32/ — STM32 USB device/host (TinyUSB or Cube) if needed
  - platform/
    - platform/linux/ — desktop/platform specifics (existing)
    - platform/windows/
    - platform/macos/
    - platform/embedded/ — shared embedded shims (assert/log/time abstractions)
    - platform/stm32/
      - boards/<family>_<board>/ — board support (pins, clocks, codecs, external peripherals)
      - cmsis/ — CMSIS headers (as submodule or toolchain‑provided include path)
      - hal/ — STM32 HAL/LL glue (prefer not to vendor; reference via toolchain where possible)
      - ld/ — linker scripts and memory maps per board/configuration
  - rtos/
    - threadx/ — RTOS wrappers (threads, mutexes, queues, timers) implemented over ThreadX APIs
    - freertos/ — optional alternative wrappers if you want FreeRTOS builds

- cmake/
  - toolchains/
    - arm-gcc-stm32.cmake — cross compile toolchain file (docs/template)
  - modules/ — CMake helpers to find CMSIS/HAL/GUIX/ThreadX


Abstraction guidelines (to make core work on STM32)
- Core timing and threading
  - Provide a small OS abstraction layer in rtos/<rtos>/ that offers Threads, Mutex, EventQueue, Sleep, and a monotonic Clock. Core code should depend on these interfaces via a thin header in core/util/rtos.h.
  - For desktop builds, implement these with std::thread/std::mutex/std::chrono; for STM32+ThreadX, map to tx_thread, tx_mutex, tx_queue, tx_time_get.
- Memory and buffers
  - Prefer fixed‑size buffers or allocator injection in performance‑critical paths. For DMA, ensure buffers are word‑aligned and placed in suitable memory regions; keep this policy in platform/stm32/boards/*.
- File system and configuration
  - On embedded targets without a filesystem, provide configuration via compile‑time constants, generated headers, or a small KV blob in flash. Mirror the JSON schema as C structs if desired. Keep the RadioConfig loader pluggable.
- Logging/assertions
  - Define a common logging macro (e.g., SDR_LOG(level, ...)) that maps to qDebug on desktop and to ITM/UART/SEGGER RTT on STM32. Place mapping in platform/*.
- Audio and GPIO
  - Keep the audio sink/source and GPIO interfaces in io/ generic. Implement device/stm32 backends using HAL/LL + DMA (I2S/SAI/DFSDM as appropriate). Select at build time.
- GUI
  - Put any GUI logic under app/. Keep processing and state in core/ and surface only the minimal view‑model API so app/qt and app/guix can reuse it.


Build system notes for STM32/ThreadX/GUIX
- Use a CMake toolchain file (cmake/toolchains/arm-gcc-stm32.cmake) or build from STM32CubeIDE. If using CubeMX to generate startup files and HAL, commit them under platform/stm32/boards/<board>/generated/ and add a thin CMake target that compiles them; avoid mixing generated code with core/ and io/ sources.
- Make backends selectable with options:
  - -DAPP=qt|guix|headless
  - -DAUDIO_BACKEND=rtaudio|stm32
  - -DGPIO_BACKEND=linux|stm32
  - -DRTOS=none|threadx|freertos
- Provide per‑board presets: cmake -S . -B build-stm32 -C platform/stm32/boards/<board>/preset.cmake
- Linker script and startup objects should be referenced only for embedded targets; guard with if(EMBEDDED_TARGET).


Incremental path to embedded
1. Identify Qt‑dependent code that leaked into core/ and move/abstract it behind app/ or io/ interfaces. ✓
2. Define minimal RTOS abstraction headers in core/util/ and provide desktop implementations. *
3. Introduce app/headless with a simple main() that drives the Radio without Qt; this will also help testing on desktop. *
4. Add io/audio/device/stm32 and io/control/gpio/stm32 stubs that compile on desktop (empty) and later flesh out for STM32. *
5. Add platform/stm32/boards/<board> skeleton with linker script references and CMake preset. *
6. Create app/guix entry (build‑time excluded on desktop) once core + io build freestanding. *

This extension keeps the original desktop workflow intact while creating clean seams for an embedded port. You can adopt ThreadX/GUIX without touching the DSP and radio pipeline, focusing only on the app/, io/ backends, and the small RTOS/platform facades.
