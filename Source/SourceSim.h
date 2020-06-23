#ifndef PROCESSORPLUGIN_H_DEFINED
#define PROCESSORPLUGIN_H_DEFINED

#include <DataThreadHeaders.h>

#include <ctime>
#include <ratio>
#include <chrono>

#define PI 3.14159f

#define AP_CHANNELS 384
#define LFP_CHANNELS 384
#define NIDAQ_CHANNELS 1
#define APT_CHANNELS 384

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

	bool clkEnabled;
	uint64 eventCode;
	float clk_period;
	float clk_tol;

	void timerCallback();
	bool risingEdgeReceived;
	bool fallingEdgeReceived;
	int lastRisingEdgeSampleNum;
	int lastFallingEdgeSampleNum;
	bool risingEdgeProcessed;
	bool fallingEdgeProcessed;

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	void updateClk(bool enable);
	void updateClkFreq(int freq, float tol);

	/*Add data as a function of processor sample rate + audio card sample rate */ 
	virtual void generateDataPacket() = 0;

};

/* Simulates expected Neuropixels AP Band when probe is in air (60 Hz) */
class NPX_AP_BAND : public SourceSim
{

public:
	NPX_AP_BAND() : SourceSim("AP", AP_CHANNELS, 30000.0f) {};
	~NPX_AP_BAND() {};

	void generateDataPacket() {

		float samples[AP_CHANNELS];

		for (int i = 0; i < packetSize; i++)
		{

			//Generate sine wave at 60 Hz with amplitude 1000
			float sample =  1000.0f*sin(2*PI*(float)numSamples/(sampleRate / 60.0f));
			for (int j = 0; j < numChannels; j++)
			{
				samples[j] = sample;
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
	NPX_LFP_BAND() : SourceSim("LFP", LFP_CHANNELS, 2500.0f) {};
	~NPX_LFP_BAND() {};

	void generateDataPacket() {

		float samples[LFP_CHANNELS];

		for (int i = 0; i < packetSize; i++)
		{
			for (int j = 0; j < numChannels; j++)
			{
				//Generate sine wave at 60 Hz with amplitude 1000
				samples[j] = (j % 2 == 0 ? 1.0f : -1.0f) * 1000.0f*sin(2*PI*(float)numSamples/(sampleRate / 60.0f));
			}
			numSamples++;
			buffer->addToBuffer(samples, &numSamples, &eventCode, 1);
		}

	};
};

/* Simulates NIDAQ Analog + Digital acquisition w/ 60 Hz sine wave */
class NIDAQ : public SourceSim
{
public:
	NIDAQ() : SourceSim("AI", NIDAQ_CHANNELS, 30000.0f) {};
	~NIDAQ() {};

	void generateDataPacket() {

		float samples[NIDAQ_CHANNELS];

		for (int i = 0; i < packetSize; i++)
		{
			for (int j = 0; j < numChannels; j++)
			{
				//Generate sine wave at 10 Hz with amplitude 1000
				samples[j] = 1000.0f*sin(2*PI*(float)numSamples/(sampleRate / 10.0f));
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

#define RESTING_MEMBRANE_POTENTATION_IN_MV 10.0f
#define THRESHOLD_POTENTIAL_IN_MV -25.0f
#define PEAK_DEPOLARIZATION_POTENTIAL_IN_MV -100.0f

static unsigned int g_seed;

inline int fastrand() {
  g_seed = (214013*g_seed+2531011);
  return (g_seed>>16)&0x7FFF;
}

/* Simulates AP signal based on crude piece-wise function */
class APTrain : public SourceSim
{
public:
	APTrain() : SourceSim("APT", APT_CHANNELS, 30000.0f) {};
	~APTrain() {};

	void generateDataPacket() {

		float samples[APT_CHANNELS];
		float sample_out;

		for (int i = 0; i < packetSize; i++)
		{

			float time = 1000.0f * (float)(numSamples - lastRisingEdgeSampleNum) / sampleRate;

			if (!risingEdgeProcessed)
			{

				if (time < DEPOLARIZATION_START_TIME_IN_MS) 
				{
					sample_out = RESTING_MEMBRANE_POTENTATION_IN_MV + (THRESHOLD_POTENTIAL_IN_MV) * time / (DEPOLARIZATION_START_TIME_IN_MS);
				}
				else if (time < REPOLARIZATION_START_TIME_IN_MS)
				{
					time -= DEPOLARIZATION_START_TIME_IN_MS;
					sample_out = THRESHOLD_POTENTIAL_IN_MV + (PEAK_DEPOLARIZATION_POTENTIAL_IN_MV - THRESHOLD_POTENTIAL_IN_MV) * time / (REPOLARIZATION_START_TIME_IN_MS - DEPOLARIZATION_START_TIME_IN_MS);
				}
				else if (time < HYPERPOLARIZATION_START_TIME_IN_MS)
				{ 
					time -= REPOLARIZATION_START_TIME_IN_MS;
					sample_out = PEAK_DEPOLARIZATION_POTENTIAL_IN_MV + (RESTING_MEMBRANE_POTENTATION_IN_MV - PEAK_DEPOLARIZATION_POTENTIAL_IN_MV) * time / (HYPERPOLARIZATION_START_TIME_IN_MS - REPOLARIZATION_START_TIME_IN_MS);
				}
				else //TODO: Refractory period
				{
					risingEdgeProcessed = true;
				}
			}
			else
			{
				/* TODO: Implement more meaningful simulated resting membrane potential instead of rand() numbers */
				/*
				srand((unsigned) std::time(NULL));
				float randomNoise = (float) fastrand() / 2147.5;
				sample_out = RESTING_MEMBRANE_POTENTATION_IN_MV + (randomNoise - 1.0f)*5.0f;
				*/
				sample_out = 0;
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