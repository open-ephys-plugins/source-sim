//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#ifndef PROCESSORPLUGIN_H_DEFINED
#define PROCESSORPLUGIN_H_DEFINED

#include <DataThreadHeaders.h>

#include <ctime>
#include <ratio>
#include <chrono>

using namespace std::chrono;

class SourceSim : public Thread, public Timer
{
public:
	/** The class constructor, used to initialize any members. */
	SourceSim(int channels, float sampleRate);

	/** The class destructor, used to deallocate memory */
	~SourceSim();

	void run();

	DataBuffer* buffer;

	//My properties
	int numChannels;
	int packetSize;
	float sampleRate;
	int64 numSamples;
	uint64 eventCode;

	void timerCallback();

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

};

#endif