#include "SourceSim.h"

SourceSim::SourceSim(String name, int channels, float sampleRate) : Thread(name)
{
	risingEdgeProcessed = false;
	fallingEdgeProcessed = false;

	this->name = name;
	numChannels = channels;
	packetSize = 500;
	this->sampleRate = sampleRate;

	clkEnabled = true;
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
		if (eventCode)
		{
			risingEdgeReceived = true;
			risingEdgeProcessed = false;
		}
		else
		{
			fallingEdgeReceived = true;
			fallingEdgeProcessed = false;
		}
	}

}

void SourceSim::updateClk(bool enable)
{
	clkEnabled = enable;
}

void SourceSim::updateClkFreq(int freq, float tol)
{
	stopTimer();

	clk_period = 1 / (float)freq;

	startTimer(1000 * clk_period / 2);

}

void SourceSim::run()
{

	//Keep track of total number of samples generated since starting acquisition
	numSamples = 0;

	//Start the TTL clock (50% duty cycle @ 1 / clk_period Hz)
	startTimer(1000 * clk_period  / 2);

	t1 = high_resolution_clock::now();

	int count = 0;

	while (!threadShouldExit())
	{

		//Stop the thread to simulate 
		wait((int)(1000.0f * (float) packetSize / (float) sampleRate));

		//Wait some more until sampling time has completely passed 
		//while (duration_cast<duration<double>>(high_resolution_clock::now()-t1).count() < packetSize / sampleRate)

		//Set event received flag for data packet generation based on events
		if (risingEdgeReceived)
		{
			lastRisingEdgeSampleNum = numSamples;
			risingEdgeReceived = false;
		}
		else if (fallingEdgeReceived)
		{
			lastFallingEdgeSampleNum = numSamples;
			fallingEdgeReceived = false;
		}

		//Generate the data packet
		generateDataPacket();

		//Reset the sampling check time
		t1 = high_resolution_clock::now();

	}

	stopTimer();
}