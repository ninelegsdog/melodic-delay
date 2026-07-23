# Melodic Delay

A C++ audio delay effect with melodic echo, pitch shifting, and custom envelope control. Available as both a CLI tool and JUCE VST3/Standalone plugin.

## Features

- **Melodic echo:** Each repetition can change pitch, speed, and volume independently
- **Extended tail:** 10-30 second delay tails with efficient memory usage
- **Flexible control:** Custom envelope curves via spline interpolation
- **Dual pitch modes:** Resampling (tape-style) and independent phase vocoder (FFT-based)
- **Three repeat modes:** Cumulative, fixed, and independent parameter application
- **VST3 plugin:** Full JUCE plugin with GUI and parameter automation
- **Standalone app:** Process audio without a DAW
- **6 built-in presets:** Default, Dreamy, Shimmer, Melodic Up, Melodic Down, Cascade

## Building

### CLI Tool

```bash
cmake -B build
cmake --build build
```

### VST3 Plugin + Standalone

Prerequisites:
```bash
sudo apt install libasound2-dev libfreetype6-dev juce-modules-source juce-tools
```

Build:
```bash
cmake -B build-vst -S vst -DCMAKE_BUILD_TYPE=Release
cmake --build build-vst -j$(nproc)
```

Output:
- **VST3:** `build-vst/MelodicDelay_artefacts/Release/VST3/Melodic Delay.vst3/`
- **Standalone:** `build-vst/MelodicDelay_artefacts/Release/Standalone/Melodic Delay`

## Usage

### CLI

```bash
# Classic echo
./build/melodic-delay --delay-time 300 --feedback 0.6 input.wav output.wav

# Melodic echo with pitch rise
./build/melodic-delay --pitch 3 --pitch-mode independent --feedback 0.5 input.wav output.wav

# Tape effect (slowing echo)
./build/melodic-delay --speed 0.8 --pitch-mode resample --feedback 0.7 input.wav output.wav

# Custom envelope
./build/melodic-delay --envelope "0:1,0.5:0.3,1:0" --feedback 0.6 input.wav output.wav
```

### VST3 Plugin

Copy the `.vst3` bundle to your DAW's plugin directory:
```bash
cp -r "build-vst/MelodicDelay_artefacts/Release/VST3/Melodic Delay.vst3" ~/.vst3/
```

## Parameters

| Parameter | CLI Flag | VST Range | Default |
|-----------|----------|-----------|---------|
| Tail Duration | --tail-duration | 0.1-30.0 s | 5.0 |
| Delay Time | --delay-time | 1-5000 ms | 500 |
| Feedback | --feedback | 0-1 | 0.5 |
| Pitch Shift | --pitch | -24 to +24 st | 0 |
| Speed | --speed | 0.25-4.0x | 1.0 |
| Volume Decay | --volume-decay | 0-1 | 0.7 |
| Mix | --mix | 0-1 | 0.5 |

## Presets

| Preset | Description |
|--------|-------------|
| Default | Neutral starting point |
| Dreamy | Long ethereal delays with subtle pitch lift |
| Shimmer | Rising pitch for pad-like textures |
| Melodic Up | Ascending pitch cascades |
| Melodic Down | Descending pitch echoes |
| Cascade | Fast echoes with heavy feedback |

## Testing

```bash
cd build && ctest --output-on-failure
```

All 27 tests pass covering RingBuffer, PitchShifter, Envelope, and MelodicDelay.

## Architecture

- **RingBuffer:** Circular buffer with Lagrange interpolation for pitch-shifted reads
- **PitchShifter:** Resampling and FFT-based phase vocoder using KissFFT
- **Envelope:** Catmull-Rom spline through control points
- **MelodicDelay:** Core DSP engine with feedback loop and envelope modulation
- **JUCE Plugin:** VST3/Standalone wrapper with AudioProcessorValueTreeState for automation
- **Presets:** Built-in presets applied via parameter value tree updates
