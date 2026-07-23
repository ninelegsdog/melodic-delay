#include "MelodicDelay.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

void printUsage() {
    std::cout << "Usage: melodic-delay [options] input.wav output.wav\n"
              << "Options:\n"
              << "  --tail-duration <sec>     Tail duration (0.1-30.0, default: 5.0)\n"
              << "  --delay-time <ms>         Delay time (1-5000, default: 500)\n"
              << "  --feedback <0-1>          Feedback amount (default: 0.5)\n"
              << "  --pitch <semitones>       Pitch shift (±24, default: 0)\n"
              << "  --speed <ratio>           Speed ratio (0.25-4.0, default: 1.0)\n"
              << "  --volume-decay <0-1>      Volume decay per repeat (default: 0.7)\n"
              << "  --pitch-mode <mode>       resample | independent (default: resample)\n"
              << "  --repeat-mode <mode>      cumulative | fixed | independent\n"
              << "  --envelope <points>       Control points: \"t1:a1,t2:a2,...\"\n"
              << "  --mix <0-1>               Dry/wet mix (default: 0.5)\n"
              << "  --sample-rate <hz>        Sample rate (default: 48000)\n";
}

std::vector<std::pair<float, float>> parseEnvelope(const std::string& str) {
    std::vector<std::pair<float, float>> points;
    std::istringstream ss(str);
    std::string token;

    while (std::getline(ss, token, ',')) {
        size_t colon = token.find(':');
        if (colon != std::string::npos) {
            float time = std::stof(token.substr(0, colon));
            float amp = std::stof(token.substr(colon + 1));
            points.push_back({time, amp});
        }
    }

    return points;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage();
        return 1;
    }

    DelayParams params;
    std::string inputFile;
    std::string outputFile;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--tail-duration" && i + 1 < argc) {
            params.tailDuration = std::stof(argv[++i]);
        } else if (arg == "--delay-time" && i + 1 < argc) {
            params.delayTime = std::stof(argv[++i]);
        } else if (arg == "--feedback" && i + 1 < argc) {
            params.feedbackGain = std::stof(argv[++i]);
        } else if (arg == "--pitch" && i + 1 < argc) {
            params.pitchShift = std::stof(argv[++i]);
        } else if (arg == "--speed" && i + 1 < argc) {
            params.speedShift = std::stof(argv[++i]);
        } else if (arg == "--volume-decay" && i + 1 < argc) {
            params.volumeDecay = std::stof(argv[++i]);
        } else if (arg == "--pitch-mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            params.pitchMode = (mode == "independent") ? PitchMode::INDEPENDENT : PitchMode::RESAMPLE;
        } else if (arg == "--repeat-mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "cumulative") params.repeatMode = RepeatMode::CUMULATIVE;
            else if (mode == "independent") params.repeatMode = RepeatMode::INDEPENDENT;
            else params.repeatMode = RepeatMode::FIXED;
        } else if (arg == "--envelope" && i + 1 < argc) {
            params.envelopePoints = parseEnvelope(argv[++i]);
        } else if (arg == "--mix" && i + 1 < argc) {
            params.mix = std::stof(argv[++i]);
        } else if (arg == "--sample-rate" && i + 1 < argc) {
            i++;
        } else if (arg[0] != '-') {
            if (inputFile.empty()) inputFile = arg;
            else outputFile = arg;
        }
    }

    if (inputFile.empty() || outputFile.empty()) {
        printUsage();
        return 1;
    }

    if (params.envelopePoints.empty()) {
        params.envelopePoints = {{0.0f, 1.0f}, {params.tailDuration, 0.0f}};
    }

    drwav wav;
    if (!drwav_init_file(&wav, inputFile.c_str(), nullptr)) {
        std::cerr << "Error: Cannot open input file: " << inputFile << "\n";
        return 1;
    }

    size_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    std::vector<float> inputSamples(totalSamples);
    drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, inputSamples.data());

    int sampleRate = wav.sampleRate;
    int channels = wav.channels;
    drwav_uninit(&wav);

    MelodicDelay delay(sampleRate, params);
    std::vector<float> outputSamples(inputSamples.size());

    for (int ch = 0; ch < channels; ch++) {
        delay.reset();
        for (size_t i = ch; i < inputSamples.size(); i += channels) {
            outputSamples[i] = delay.process(inputSamples[i]);
        }
    }

    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    if (!drwav_init_file_write(&wav, outputFile.c_str(), &format, nullptr)) {
        std::cerr << "Error: Cannot create output file: " << outputFile << "\n";
        return 1;
    }

    drwav_write_pcm_frames(&wav, outputSamples.size(), outputSamples.data());
    drwav_uninit(&wav);

    std::cout << "Processed: " << inputFile << " -> " << outputFile << "\n";
    return 0;
}
