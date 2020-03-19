#include "SourceSim.h"

#define CLOCK_PERIOD_IN_MS 500

SourceSim::SourceSim(String name, int channels, float sampleRate) : Thread(name)
{
	this->name = name;
	numChannels = channels;
	packetSize = 100;
	this->sampleRate = sampleRate;
}

SourceSim::~SourceSim()
{
}

void SourceSim::timerCallback()
{
	eventCode = (!(bool)eventCode);
}

void SourceSim::run()
{

	numSamples = 0;

	startTimer(CLOCK_PERIOD_IN_MS / 2);

	t1 = high_resolution_clock::now();

	while (!threadShouldExit())
	{
		t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

		if (time_span.count() > 1 / (sampleRate / packetSize))
		{
			generateDataPacket();
			t1 = high_resolution_clock::now();
		}

	}

	stopTimer();
}