#ifndef PROCESSORPLUGIN_H_DEFINED
#define PROCESSORPLUGIN_H_DEFINED

#include <DataThreadHeaders.h>

#include <ctime>
#include <ratio>
#include <chrono>

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
				samples[j] = 1000.0f*sin(2*3.14159*(float)numSamples/(sampleRate / 60.0f));
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
				samples[j] = 1000.0f*sin(2*3.14159*(float)numSamples/(sampleRate / 60.0f));
			}
			numSamples++;
			buffer->addToBuffer(samples, &numSamples, &eventCode, 1);
		}

	};
};

// 	void run();
// };

// class NIDAQ : public SourceSim
// {
// public: 
// 	NIDAQ() : SourceSim(8, 30000.0f) {};
// 	~NIDAQ();

// 	void run();
// };

// class APTrain : public SourceSim
// {
// public:
// 	APTrain() : SourceSim(10, 30000) {};
// 	~APTrain();

// 	void run();
// };

#endif