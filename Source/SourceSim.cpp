#include "SourceSim.h"

SourceSim::SourceSim(String name, int channels, float sampleRate) : Thread(name)
{
	this->name = name;
	numChannels = channels;
	packetSize = 10;
	this->sampleRate = sampleRate;

	clkEnabled = false;
	clk_period = 1; //s
	clk_tol = 0.001; //sc
	
}

SourceSim::~SourceSim()
{
}

void SourceSim::timerCallback()
{
	if (clkEnabled)
	{
		eventCode = (!(bool)eventCode);
	}

}

void SourceSim::updateClk(bool enable)
{
	clkEnabled = enable;
}

void SourceSim::run()
{

	numSamples = 0;

	startTimer(1000 * clk_period  / 2);

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