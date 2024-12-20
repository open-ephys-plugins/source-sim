/*
	------------------------------------------------------------------

	This file is part of the Open Ephys GUI
	Copyright (C) 2020 Allen Institute for Brain Science and Open Ephys

	------------------------------------------------------------------

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "SimulatedSource.h"

SimulatedSource::SimulatedSource(String name, int channels_, float sampleRate_, SimulatedSourceType type)
	: Thread(name)
{

	numChannels = channels_;
	sampleRate = sampleRate_;

	clkEnabled = true; //TODO: make this configurable

	switch (type)
	{
	case AP_BAND:
		data = &simulatedData.ap_band;
		availableSamples = simulatedData.ap_band.size();
		samplesPerBuffer = 300;
		break;
	case LFP_BAND:
		data = &simulatedData.lfp_band;
		availableSamples = simulatedData.lfp_band.size();
		samplesPerBuffer = 25;
		break;
	case NIDAQ:
		data = &simulatedData.nidaq;
		availableSamples = simulatedData.nidaq.size();
		samplesPerBuffer = 300;
		break;
	}

}

void SimulatedSource::updateClockFrequency(int freq)
{
	if (freq > 0)
		clk_period = 1.0f / (float)freq;
	else
		clk_period = 0;
}

void SimulatedSource::run()
{

	int64 sampleNumber = 0;
	int64 uSecPerBuffer = samplesPerBuffer / sampleRate * 1e6;
	eventCode = 0;

	int64 start = Time::getHighResolutionTicks();
	int64 bufferCount = 0;

	while (!threadShouldExit())
	{

		float attenuation = 1.0f - float(numSamples) / (2.0f * 60.0 * float(sampleRate));

		bufferCount++;

		for (int sample_num = 0; sample_num < samplesPerBuffer; sample_num++)
		{

			for (int chan = 0; chan < numChannels; chan++)
			{
				samples[(chan * samplesPerBuffer) + sample_num] = attenuation * (*data)[sampleNumber % availableSamples];
			}

			sampleNumbers[sample_num] = sampleNumber++;
			timestamps[sample_num] = -1.0;

			if (clk_period > 0)
			{
				int samplesPerClkPeriod = int(sampleRate * clk_period / 2);
				if (sampleNumber % samplesPerClkPeriod == 0)
				{
					if (eventCode == 0)
						eventCode = 1;
					else
						eventCode = 0;
				}
			}
			else if (eventCode != 0)
			{
				eventCode = 0;
			}

			event_codes[sample_num] = eventCode;

			numSamples++;
		}

		buffer->addToBuffer(samples, sampleNumbers, timestamps, event_codes, samplesPerBuffer);

		int64 uSecElapsed = int64(Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks() - start) * 1e6);

		if (uSecElapsed < (uSecPerBuffer * bufferCount))
		{
			std::this_thread::sleep_for(std::chrono::microseconds((uSecPerBuffer * bufferCount) - uSecElapsed));
		}
	}
}