# Melodic Delay

A C++ audio delay effect with melodic echo, pitch shifting, and custom envelope control.

## Features

- **Melodic echo:** Each repetition can change pitch, speed, and volume independently
- **Extended tail:** 10-30 second delay tails with efficient memory usage
- **Flexible control:** Custom envelope curves via spline interpolation
- **Dual pitch modes:** Resampling (tape-style) and independent phase vocoder
- **Three repeat modes:** Cumulative, fixed, and independent parameter application

## Building

```bash
cmake -B build
cmake --build build
```

## Usage

```bash
# Classic echo
./melodic-delay --delay-time 300 --feedback 0.6 input.wav output.wav

# Melodic echo with pitch rise
./melodic-delay --pitch 3 --pitch-mode independent --feedback 0.5 input.wav output.wav

# Tape effect (slowing echo)
./melodic-delay --speed 0.8 --pitch-mode resample --feedback 0.7 input.wav output.wav

# Custom envelope
./melodic-delay --envelope "0:1,0.5:0.3,1:0" --feedback 0.6 input.wav output.wav
```

## Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| --tail-duration | Tail duration in seconds (0.1-30) | 5.0 |
| --delay-time | Delay time in milliseconds (1-5000) | 500 |
| --feedback | Feedback amount (0-1) | 0.5 |
| --pitch | Pitch shift in semitones (±24) | 0 |
| --speed | Speed ratio (0.25-4.0) | 1.0 |
| --volume-decay | Volume decay per repeat (0-1) | 0.7 |
| --pitch-mode | resample or independent | resample |
| --repeat-mode | cumulative, fixed, or independent | fixed |
| --envelope | Control points "t1:a1,t2:a2,..." | linear fade |
| --mix | Dry/wet mix (0-1) | 0.5 |

## Testing

```bash
cd build && ctest --output-on-failure
```

## Architecture

- **RingBuffer:** Circular buffer with Lagrange interpolation
- **PitchShifter:** Resampling and phase vocoder modes
- **Envelope:** Catmull-Rom spline through control points
- **MelodicDelay:** Core DSP engine with feedback loop

## VST Support

VST plugin support is planned for future implementation using JUCE framework.
