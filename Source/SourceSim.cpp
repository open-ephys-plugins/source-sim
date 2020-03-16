#include "SourceSim.h"

SourceSim::SourceSim() : Thread("Source Simulator")
{

}

SourceSim::~SourceSim()
{

}

void SourceSim::run()
{

	t1 = high_resolution_clock::now();

	while (!threadShouldExit())
	{
		t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

		if (time_span.count() > 0.0066666666666667)
		{
			float apSamples[384];
			eventCode = (!(bool)eventCode);

			for (int i = 0; i < 200; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					apSamples[j] = 1000*eventCode;
				}
				numSamples++;
				buffer->addToBuffer(apSamples, &numSamples, &eventCode, 1);
			}
			t1 = high_resolution_clock::now();
		}

	}
	 
}

