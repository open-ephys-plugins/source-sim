#ifndef PROCESSORPLUGIN_H_DEFINED
#define PROCESSORPLUGIN_H_DEFINED

#include <DataThreadHeaders.h>

#include <ctime>
#include <ratio>
#include <chrono>

#define PI 3.14159f

using namespace std::chrono;

/* Source Simulator Class to simulate actual sources generating data into OpenEphys */
class SourceSim : public Thread, public Timer
{
public:

	SourceSim(String name, int channels, float sampleRate);
	~SourceSim();

	String name;

	void run() override;

	DataBuffer* buffer;

	int numChannels;
	int packetSize;
	float sampleRate;
	int64 numSamples;
	uint64 eventCode;

	void timerCallback();

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	virtual void generateDataPacket() = 0;

};

/* Simulates expected Neuropixels AP Band when probe is in air (60 Hz) */
class NPX_AP_BAND : public SourceSim
{

public:
	NPX_AP_BAND() : SourceSim("AP", 384, 30000.0f) {};
	~NPX_AP_BAND() {};

	void generateDataPacket() {

		float samples[numChannels];

		for (int i = 0; i < packetSize; i++)
		{
			for (int j = 0; j < numChannels; j++)
			{
				//Generate sine wave at 60 Hz with amplitude 1000
				samples[j] = 1000.0f*sin(2*PI*(float)numSamples/(sampleRate / 60.0f));
			}
			numSamples++;
			buffer->addToBuffer(samples, &numSamples, &eventCode, 1);
		}

	};
};

/* Simulates expected Neuropixels LFP Band when probe is in air (60 Hz) */
class NPX_LFP_BAND : public SourceSim
{
public:
	NPX_LFP_BAND() : SourceSim("LFP", 384, 2500.0f) {};
	~NPX_LFP_BAND() {};

	void generateDataPacket() {

		float samples[numChannels];

		for (int i = 0; i < packetSize; i++)
		{
			for (int j = 0; j < numChannels; j++)
			{
				//Generate sine wave at 60 Hz with amplitude 1000
				samples[j] = 1000.0f*sin(2*PI*(float)numSamples/(sampleRate / 60.0f));
			}
			numSamples++;
			buffer->addToBuffer(samples, &numSamples, &eventCode, 1);
		}

	};
};

#define INITIATION_POTENTIAL_START_TIME_IN_MS 0
#define DEPOLARIZATION_START_TIME_IN_MS 0.8f
#define REPOLARIZATION_START_TIME_IN_MS 1.3f
#define HYPERPOLARIZATION_START_TIME_IN_MS 1.8f
#define REFRACTORY_PERIOD_DURAION_IN_MS 5.0f;

#define RESTING_MEMBRANE_POTENTATION_IN_MV -90.0f
#define THRESHOLD_POTENTIAL_IN_MV -65.0f
#define PEAK_DEPOLARIZATION_POTENTIAL_IN_MV 35.0f

/* Simulates AP signal based on crude piece-wise function */
class APTrain : public SourceSim
{
public:
	APTrain() : SourceSim("APT", 10, 30000) {};
	~APTrain() {};

	void generateDataPacket() {

		float samples[numChannels];
		float sample_out;

		for (int i = 0; i < packetSize; i++)
		{

			int sampleNum = numSamples % (int)sampleRate;
			float time = 1000.0f * (float)sampleNum / sampleRate;

			if (time < DEPOLARIZATION_START_TIME_IN_MS) 
			{
				std::cout << time << ":";
				eventCode = 1;
				sample_out = RESTING_MEMBRANE_POTENTATION_IN_MV + 25.0f * time / (DEPOLARIZATION_START_TIME_IN_MS);
				std::cout << sample_out << std::endl;
			}
			else if (time < REPOLARIZATION_START_TIME_IN_MS)
			{
				std::cout << time << ":";
				time -= DEPOLARIZATION_START_TIME_IN_MS;
				sample_out = THRESHOLD_POTENTIAL_IN_MV + (PEAK_DEPOLARIZATION_POTENTIAL_IN_MV - THRESHOLD_POTENTIAL_IN_MV) * time / (REPOLARIZATION_START_TIME_IN_MS - DEPOLARIZATION_START_TIME_IN_MS);
				std::cout << sample_out << std::endl;
			}
			else if (time < HYPERPOLARIZATION_START_TIME_IN_MS)
			{ 
				std::cout << time << ":";
				time -= REPOLARIZATION_START_TIME_IN_MS;
				sample_out = PEAK_DEPOLARIZATION_POTENTIAL_IN_MV - (PEAK_DEPOLARIZATION_POTENTIAL_IN_MV - THRESHOLD_POTENTIAL_IN_MV) * time / (HYPERPOLARIZATION_START_TIME_IN_MS - REPOLARIZATION_START_TIME_IN_MS);
				std::cout << sample_out << std::endl;
			}
			else //TODO: Refractory period
			{
				eventCode = 0;
				sample_out = -90.0f;
			}

			for (int j = 0; j < numChannels; j++)
			{
				samples[j] = sample_out;
			}
			numSamples++;
			buffer->addToBuffer(samples, &numSamples, &eventCode, 1);
		}

	};


};

#endif