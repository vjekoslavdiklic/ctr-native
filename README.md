# CTR Native

A native PC port of Crash Team Racing (PS1, 1999), built on top of the [CTR-ModSDK](https://github.com/CTR-tools/CTR-ModSDK) decompilation project.

## Philosophy

- **No byte budget.** Game source lives in `game/` as our own copies. Edit freely.
- **No PSX toolchain.** Targets Windows and Linux with SDL3. No MIPS compiler needed.
- **Clean platform layer.** `main.c` owns process startup; host details stay in `platform/native_*`.
- **No build system nonsense.** Just `build.bat` / `build.sh`.
- **Fully static build.** Single executable, zero dependencies. SDL3 is compiled from vendored source and linked statically.

## Directory Layout

```
ctr_native/
  main.c              Entrypoint and native platform boundary
  platform/           Native-owned audio, input, memcard, CD, and PSX facade glue
  build-msvc.bat      Windows build (MSVC x86)
  build.bat           Windows build (MinGW i686)
  build.sh            Linux build
  CMakePresets.json   Shared CLion/command-line CMake configurations
  README.md           This file
  game/               Our copies of all decompiled game source (943 files)
    game_unity.h      Ordered unity include chain for all game source files
  include/            Project headers (structs, globals, declarations, platform facade)
  externals/
    SDL/              SDL3 source (static build)
```

## Prerequisites

### Windows

The recommended native Windows toolchain is MSVC x86:

1. Install Visual Studio 2022 or Visual Studio Build Tools 2022.
2. Select the **Desktop development with C++** workload and a current Windows SDK.
3. Ensure CMake 3.20 or newer is on `PATH` (standalone or the Visual Studio C++ CMake tools component).
4. Run `build-msvc.bat`, or select the `windows-msvc-x86` CMake preset in CLion.

The existing MinGW i686 build remains supported:

1. Install [MSYS2](https://www.msys2.org/).
2. In an MSYS2 terminal:
   ```
   pacman -Syu
   pacman -S --needed git mingw-w64-i686-gcc mingw-w64-i686-cmake mingw-w64-i686-make
   ```
   If the update asks you to close the terminal, reopen MSYS2 and run the install command.
3. Add `C:\msys64\mingw32\bin` to your system PATH
4. Open a new Command Prompt or PowerShell and run `build.bat`.

That's it. SDL3 is compiled from vendored source -- no separate install needed.

### Linux (Debian/Ubuntu)

```
sudo apt install gcc-multilib
sudo apt install libx11-dev libxext-dev libgl1-mesa-dev libasound2-dev libudev-dev libdbus-1-dev
```

## Building

```
build-msvc.bat       # Windows, MSVC x86 (recommended)
build.bat            # Windows, MinGW i686
chmod +x build.sh
./build.sh           # Linux
```

The shared CMake presets can also be used directly or selected as CLion CMake profiles:

```
cmake --preset windows-msvc-x86
cmake --build --preset windows-msvc-x86-debug
ctest --preset windows-msvc-x86-debug
```

First build compiles SDL3 from source. This is cached as a static library in the selected build directory.

Output:

- MSVC: `build-msvc-x86/Release/ctr_native.exe`
- MinGW: `build/ctr_native.exe`
- Linux: `build/ctr_native`

### Clean build

```
rmdir /s /q build    # Windows: delete cached libraries
build.bat            # Windows: rebuild everything

rmdir /s /q build-msvc-x86
build-msvc.bat       # Windows MSVC: rebuild everything

rm -rf build/        # Linux: delete cached libraries
./build.sh           # Linux: rebuild everything
```

## Running

### Normal Setup

If you downloaded a release build, you only need two things for normal play:

1. The game executable:
   - `ctr_native.exe` on Windows
   - `ctr_native` on Linux
2. Your own NTSC-U retail CTR disc image, named (put in directory called `assets`):
   - `assets/ctr-u.bin`

Example:

```
CTR-Native/
  ctr_native.exe
  assets/
    ctr-u.bin
```

Then run `ctr_native.exe`.

The disc image must be the common single-track raw PSX BIN layout: MODE2/2352 sectors, with the data track starting at byte 0. A cooked 2048-byte `.iso` does not preserve the XA/STR sector data needed for audio and video playback.

For development builds run from `build/`, put the same `assets/ctr-u.bin` next to the source tree:

```
ctr-native/
  build/
    ctr_native.exe
  assets/
    ctr-u.bin
```

### Extracted Asset Override

You do not need extracted assets for normal play.

Extracted files are still supported for development, modding, and debugging. If present, they override files from `ctr-u.bin`.

Extracted-asset override structure:

```
CTR-Native/
  ctr_native.exe
  assets/
    BIGFILE.BIG
    SOUNDS/KART.HWL
    TEST.STR
    XA/
      ENG.XNF
      ENG/EXTRA/S00.XA ... S05.XA
      ENG/GAME/S00.XA ... S20.XA
      MUSIC/S00.XA ... S01.XA
```

The full extracted asset list is:

- `BIGFILE.BIG`
- `SOUNDS/KART.HWL`
- `TEST.STR`
- `XA/ENG.XNF`
- `XA/ENG/EXTRA/S00.XA` through `S05.XA`
- `XA/ENG/GAME/S00.XA` through `S20.XA`
- `XA/MUSIC/S00.XA` through `S01.XA`

## PC Enhancement Profile

This build includes a native PC-focused enhancement pass. The original PS1
gameplay and asset formats are preserved, while the renderer is allowed to use
modern CPU and GPU resources.

### Resolution and image quality

- The game uses a 4K-class internal render target (`3840×2160` minimum), not
  merely a larger startup window. World geometry, effects, HUD, menus, and the
  final presentation are all rendered through the higher-resolution native
  pipeline.
- Internal supersampling is enabled above the base target when GPU headroom is
  available. This improves polygon edges, texture sampling, and UI clarity
  before the image is presented to the window.
- Native texture sampling uses bilinear filtering and the presentation path is
  tuned to reduce PS1-era shimmering, dithering, and visibly quantized output.
- The world renderer has an extended draw distance (10× the original tuned
  distance), allowing track geometry and scenery to remain visible much farther
  from the camera.
- Render-target source rectangles are kept in original PSX coordinates before
  being scaled. This prevents high-resolution off-screen captures from sampling
  unrelated VRAM areas.

### Text, HUD, and menus

- Menu and HUD text is rendered through the high-resolution native path, with
  stronger filtering/sharpening applied selectively to UI content rather than
  blurring the 3D world.
- Race-position digits use a direct scalable font draw in place of the original
  depth-selected 3D number model. This fixes the incorrect/stuck position digit
  and keeps it legible at 4K in single-player and split-screen modes.
- The position digit has separate scale and placement tuning so it matches the
  suffix text while remaining large enough to read at high resolution.
- Built-in debug menus and tools are enabled in the native build.

### 60 Hz presentation and timing

- Native presentation can run at 60 Hz while legacy game timing is handled
  separately from render cadence.
- Frame-authored gameplay paths were audited and converted or gated against
  elapsed time/legacy cadence where needed. This includes jump timing, TNT and
  hazard timing, item roulette/weapon cooldowns, animation/state-machine
  counters, and related race logic.
- Camera and world-render updates are tuned to reduce stepped motion without
  advancing gameplay, physics, or animations twice as quickly.
- The goal is smoother presentation while retaining the original game-speed
  behavior. Some systems remain deliberately tied to their legacy cadence where
  changing them would alter CTR gameplay.

### Multiplayer and battle rendering

- The original split-screen renderer used a shared 96×64 PSX VRAM capture for
  every remote kart. On native builds this capture path is bypassed.
- In 2-, 3-, and 4-player modes, every active kart is now submitted directly
  into every active viewport. Remote karts therefore use the same high internal
  resolution, filtering, and normal geometry path as the owning player's kart.
- The old capture-tile cache is no longer used by the native multiplayer path,
  eliminating stale poses, incorrect character images, and ghost pixels from
  prior remote-kart frames.

### Native usability additions

The native menu additions are intentionally separate from the retail overlay
data, so the original menu tables remain usable by non-native builds.

#### Main menu, Options, and Exit

- The bottom of the main menu is now **HIGH SCORE**, **OPTIONS**, **EXIT**.
  When Scrapbook is available, the order is **HIGH SCORE**, **SCRAPBOOK**,
  **OPTIONS**, **EXIT**. This keeps Exit as the final main-menu item.
- **OPTIONS** opens a centered native submenu containing **CHEATS** and
  **BACK**. The menu is explicitly positioned at the native UI center
  (`256, 160` in the PSX-compatible UI coordinate space), so it remains
  centered independently of the main-menu title animation or output window
  size.
- **EXIT** calls the native orderly shutdown routine, matching a normal window
  close instead of leaving the process running in the background.

#### Cheats menu

- **OPTIONS → CHEATS** opens a scrollable, high-resolution native menu. It
  displays eleven entries at once, keeps the selected row visible while
  scrolling, and renders an explicit **ON** or **OFF** state beside every
  toggle.
- To keep the compact menu readable, every visible entry label is 16
  characters or fewer. Use Up/Down to move, Cross or Circle to toggle the
  selected item, and Triangle, Square, or the **BACK** row to return to
  Options.
- Built-in gameplay cheats exposed by the menu are: **MAX WUMPA FRUIT**,
  **INFINITE MASKS**, **MAX TURBOS**, **INVISIBILITY**, **MAX ENGINE**,
  **MAX BOMBS**, **ADV DIFFICULTY**, **SUPER HARD**, **ICY TRACKS**,
  **SUPER TURBO PADS**, **ONE LAP RACES**, and **TURBO COUNTER**. These map to
  the engine's existing gameplay-cheat flags and can be toggled both on and
  off without needing to enter the original controller sequences.
- The menu also exposes **ALL CHARACTERS**, **TRACKS & ARENAS**, and **UNLOCK
  SCRAPBOOK**. Tracks & Arenas unlocks Turbo Track plus the Parking Lot, North
  Bowl, and Lab Basement battle arenas.
- Unlock toggles remember the relevant unlock bits when first enabled. Turning
  a toggle off during the same session restores those saved bits, preserving
  progress that existed before the cheat was enabled rather than simply
  clearing all unlock data.
- The Spyro launcher code is deliberately not included: it is a separate-game
  launch action, not a CTR gameplay cheat.

#### Menu implementation notes

- Native-only menu string identifiers provide the custom **CHEATS** and
  **BACK** labels without consuming or overwriting entries in CTR's localized
  language table. Retail menu rows continue to use their normal language
  indexes.
- The Options screen uses the standard `RectMenu` input, highlight, sound, and
  centered-layout behavior. The longer Cheats screen uses a native callback so
  it can draw a scrollable ON/OFF list while retaining the same controller
  input conventions and UI ordering-table path.

### Performance expectations

The higher internal resolution, supersampling, longer visibility range, and
full-resolution multiplayer karts intentionally use more GPU and CPU resources
than the PS1 renderer. Performance depends on the GPU, driver, display mode,
and number of active split-screen viewports. If performance is insufficient,
reduce the native internal-resolution/supersampling settings before changing
gameplay timing.

## Bug Replays

Internal builds can record a small bug report folder. See `docs/REPLAYS.md`.

## Architecture

```
main.c (entrypoint)
  |
  +-- platform/native_* (platform shell, audio, input, memcard, CD, renderer, PSX facade glue)
  |
  +-- game/game_unity.h
        |
        +-- game/ (all decompiled game source)
              |
              +-- include/ (headers: structs, globals, declarations)
```

- `CTR_NATIVE` is defined for native host/platform-specific code
- First-party native code targets portable C17 with compiler extensions disabled
- The default build uses 32-bit mode while remaining PSX address-shaped data and host-pointer contracts are audited. GPU primitive links are bridged through 24-bit native tokens; see `docs/MEMORY_MODEL.md`.

## Roadmap

- Clean up `game/` copies strip byte budget hacks and route platform-specific code through `CTR_NATIVE`
- Keep reducing 32-bit host-pointer assumptions in PSX-shaped data, and keep pruning inherited compatibility code now owned in `include/` and `platform/`.

## Credits

- [CTR-ModSDK](https://github.com/CTR-tools/CTR-ModSDK) — the decompilation project this is built on
- [PsyCross](https://github.com/OpenDriver2/PsyCross) — original PS1 compatibility code from which parts of CTR Native's owned platform layer and PsyQ facade headers are derived
- [SDL3](https://github.com/libsdl-org/SDL) — cross-platform multimedia
- Crash Team Racing is a trademark of Sony Computer Entertainment / Naughty Dog
