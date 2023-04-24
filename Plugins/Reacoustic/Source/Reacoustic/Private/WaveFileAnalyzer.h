#include "AudioDevice.h"
#include "AudioAnalysis.h"

class WaveFileAnalyzer {
public:
	TArray<float> getOnsets(const FString& waveFilePath) {
		// Load the wave file
		USoundWave* soundWave = LoadObject<USoundWave>(nullptr, *waveFilePath, nullptr, LOAD_None, nullptr);

		// Create an audio device to playback the sound wave
		FAudioDevice* audioDevice = GEngine->GetAudioDevice();
		FAudioDevice::FCachedDevice* cachedDevice = audioDevice->GetCachedDevice(soundWave);
		if (cachedDevice == nullptr) {
			audioDevice->CreateCachedDevice(soundWave);
			cachedDevice = audioDevice->GetCachedDevice(soundWave);
		}

		// Analyze the sound wave
		TArray<float> audioData;
		TArray<float> onsets;
		float sampleRate;
		FAudioAnalysis::ExtractRawAudioData(cachedDevice->AudioData, soundWave->NumChannels, soundWave->Duration, audioData, sampleRate);
		FAudioAnalysis::DetectOnsets(audioData.GetData(), audioData.Num(), sampleRate, onsets);

		return onsets;
	}
};