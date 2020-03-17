#include "SourceSim.h"

#define CLOCK_PERIOD_IN_MS 500

SourceSim::SourceSim(int channels, float sampleRate) : Thread("Source Simulator")
{
	numChannels = channels;
	packetSize = 500;
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

	startTimer(CLOCK_PERIOD_IN_MS / 2);

	t1 = high_resolution_clock::now();

	while (!threadShouldExit())
	{
		t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

		if (time_span.count() > 1 / (sampleRate / packetSize))
		{
			float samples[numChannels];

			for (int i = 0; i < packetSize; i++)
			{
				for (int j = 0; j < numChannels; j++)
				{
					samples[j] = 1000*eventCode;
				}
				numSamples++;
				buffer->addToBuffer(samples, &numSamples, &eventCode, 1);
			}
			t1 = high_resolution_clock::now();
		}

	}

	stopTimer();
	 
}


