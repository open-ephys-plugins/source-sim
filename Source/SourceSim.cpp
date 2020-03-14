#include "SourceSim.h"

SourceSim::SourceSim() : GenericProcessor("Source Simulator")
{

}

SourceSim::~SourceSim()
{

}

void SourceSim::process(AudioSampleBuffer& buffer)
{

	//checkForEvents(false);
	int numChannels = getNumOutputs();

	for (int chan = 0; chan < numChannels; chan++)
	{
		int numSamples = getNumSamples(chan);
		int64 timestamp = getTimestamp(chan);

		//Do whatever processing needed
	}
	 
}

