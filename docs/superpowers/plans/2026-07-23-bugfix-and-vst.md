# Plan: Bug Fixes + JUCE VST Plugin

## Phase 1: Commit Bug Fixes (5 min)

### Task 1: Commit 3 bug fixes
- **Tail truncation fix** (`src/cli/main.cpp`): Output buffer now extended by `tailDuration * sampleRate`, tail flushed with zeros
- **KissFFT buffer overflow** (`src/core/PitchShifter.cpp`): `freqData` and `outputFreqData` changed from `FFT_SIZE/2+1` to `FFT_SIZE`
- **Integer overflow in independent mode** (`src/core/PitchShifter.cpp`): Pitch clamped to ±24 semitones, `newBin` bounds check added, output buffer cleared after read

**Commit message:** `fix: tail truncation, KissFFT buffer overflow, and pitch integer overflow`

**Verification:** `cmake --build build && ./build/tests/melodic-delay-tests` (27/27 pass)

---

## Phase 2: JUCE VST3 Plugin (2-3 hours)

### Prerequisites
- Install JUCE: `sudo apt install juce-modules-source juce-tools`
- Install ALSA dev libs: `sudo apt install libasound2-dev`
- Install freetype dev: `sudo apt install libfreetype6-dev`
- Install webkit2gtk (for JUCE GUI): `sudo apt install libwebkit2gtk-4.0-dev`

### Task 2: JUCE Project Setup (~15 min)
- Create `vst/` directory with JUCE project structure
- CMake integration with `find_package(JUCE)` or module-based include
- Wrap existing `melodic-delay-core` library
- Configure VST3, Standalone, and AU plugin targets

**Files to create:**
- `vst/CMakeLists.txt` — JUCE project configuration
- `vst/Source/PluginProcessor.h` — Audio processor (wraps MelodicDelay)
- `vst/Source/PluginProcessor.cpp` — Processor implementation
- `vst/Source/PluginEditor.h` — GUI editor
- `vst/Source/PluginEditor.cpp` — GUI implementation

**Verification:** `cmake --build build-vst` produces `.vst3` bundle

### Task 3: Audio Processor (~30 min)
- Subclass `juce::AudioProcessor`
- Wrap `MelodicDelay` DSP engine
- Implement `processBlock()` calling `MelodicDelay::process()`
- Add state save/load via `getStateInformation()` / `setStateInformation()`
- Handle sample rate changes
- Parameter automation via `AudioProcessorValueTreeState`

**Parameters (7 total):**
| Index | Name | Range | Default | Unit |
|-------|------|-------|---------|------|
| 0 | Tail Duration | 0.1-30.0 | 5.0 | s |
| 1 | Delay Time | 1-5000 | 500 | ms |
| 2 | Feedback | 0-1 | 0.5 | |
| 3 | Pitch Shift | -24 to +24 | 0 | st |
| 4 | Speed Shift | 0.25-4.0 | 1.0 | x |
| 5 | Volume Decay | 0-1 | 0.7 | |
| 6 | Mix | 0-1 | 0.5 | |

**Verification:** Plugin loads in DAW, parameters respond to automation

### Task 4: GUI Editor (~45 min)
- Custom `juce::AudioProcessorEditor` subclass
- Sliders for each parameter with labels
- Preset selector (dropdown)
- Visual feedback (level meters, delay time display)
- Dark theme matching audio plugin aesthetics
- Minimum size: 600x400

**Verification:** GUI renders, sliders control parameters, presets load/save

### Task 5: Preset System (~20 min)
- Built-in presets: Dreamy, Shimmer, Melodic Up, Melodic Down, Cascade
- Save/load user presets as XML files
- Preset manager in GUI

**Files:**
- `vst/Source/Presets.h` — Preset definitions
- `vst/Source/Presets.cpp` — Preset manager implementation

**Verification:** Presets load correctly, parameters update

### Task 6: Build & Test (~15 min)
- Build VST3 bundle: `cmake --build build-vst --target MelodicDelay_VST3`
- Build Standalone: `cmake --build build-vst --target MelodicDelay_Standalone`
- Run standalone to verify audio processing
- Test parameter automation

**Verification:** Standalone app produces audio, VST3 loads in Ardour/LMMS

---

## Phase 3: Documentation (10 min)

### Task 7: Update README
- Add VST plugin section
- Build instructions for VST
- Preset descriptions
- Screenshot placeholder

**Verification:** README renders correctly on GitHub

---

## Risk Assessment

| Risk | Impact | Mitigation |
|------|--------|------------|
| JUCE version incompatibility | High | Use apt version (7.0.5), test early |
| GUI rendering issues | Medium | Start with minimal GUI, iterate |
| Plugin not loading in DAW | High | Test with Ardour (free, Linux-native) |
| Performance issues | Medium | Profile with large tail durations |

## Time Estimate
- Phase 1: 5 min
- Phase 2: 2-3 hours
- Phase 3: 10 min
- **Total: ~3 hours**

## Success Criteria
1. ✅ All 27 existing tests pass
2. ✅ VST3 plugin builds successfully
3. ✅ Standalone app processes audio
4. ✅ GUI renders and controls parameters
5. ✅ Presets load/save correctly
6. ✅ Plugin loads in Ardour or LMMS
