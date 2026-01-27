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

SimulatedSource::SimulatedSource (String name, int channels_, float sampleRate_, SimulatedSourceType type)
    : Thread (name)
{
    numChannels = channels_;
    sampleRate = sampleRate_;

    clkEnabled = true; //TODO: make this configurable
    clk_period = 1.0f; // 1 Hz

    switch (type)
    {
        case AP_BAND:
            data = &simulatedData.ap_band;
            availableSamples = simulatedData.ap_band.size();
            samplesPerBuffer = 2400;
            break;
        case LFP_BAND:
            data = &simulatedData.lfp_band;
            availableSamples = simulatedData.lfp_band.size();
            samplesPerBuffer = 200;
            break;
        case NIDAQ:
            data = &simulatedData.nidaq;
            availableSamples = simulatedData.nidaq.size();
            samplesPerBuffer = 2400;
            break;
    }

    LOGC ("SimulatedSource created with " + String (numChannels) + " channels at " + String (sampleRate) + " Hz" + 
          " with samples per buffer size: " + String (samplesPerBuffer));
}

void SimulatedSource::updateClockFrequency (int freq)
{
    if (freq > 0)
        clk_period = 1.0f / (float) freq;
    else
        clk_period = 0;
}

void SimulatedSource::run()
{
    // Use a stack buffer to pre-calculate the signal for one channel.
    // This avoids recalculating the source data for every channel and enables linear writing.
    float singleChannelData[MAX_SAMPLES_PER_BUFFER];

    int64 sampleNumber = 0;
    // Calculate timing using double precision to minimize drift
    int64 uSecPerBuffer = int64 ((double) samplesPerBuffer / sampleRate * 1e6);
    eventCode = 0;

    int64 start = Time::getHighResolutionTicks();
    int64 bufferCount = 0;

    // Cache pointers and sizes to avoid repeated indirections
    const float* sourceDataPtr = data->data();
    const int sourceSize = availableSamples;

    while (! threadShouldExit())
    {
        float attenuation = 1.0f - float (numSamples) / (120.0f * float (sampleRate));

        bufferCount++;

        // Hoist invariant calculation out of the loop
        int samplesPerClkPeriod = 0;
        if (clk_period > 0)
            samplesPerClkPeriod = int (sampleRate * clk_period / 2);

        // Pass 1: Generate source data and fill metadata vectors
        // We do this once per buffer, instead of once per channel
        for (int sample_num = 0; sample_num < samplesPerBuffer; sample_num++)
        {
            // Compute source signal with attenuation
            singleChannelData[sample_num] = attenuation * sourceDataPtr[sampleNumber % sourceSize];

            // Fill Metadata
            sampleNumbers[sample_num] = sampleNumber;
            timestamps[sample_num] = -1.0;

            // Update Event Code Logic
            if (samplesPerClkPeriod > 0)
            {
                if (sampleNumber % samplesPerClkPeriod == 0)
                {
                    eventCode = (eventCode == 0) ? 1 : 0;
                }
            }
            else if (eventCode != 0)
            {
                eventCode = 0;
            }
            event_codes[sample_num] = eventCode;

            sampleNumber++;
            numSamples++;
        }

        // Pass 2: Write to the planar output buffer
        // By iterating channels then samples, we write linearly to memory
        // which is significantly more cache-friendly than the previous strided access.
        for (int chan = 0; chan < numChannels; chan++)
        {
            float* channelDest = samples + (chan * samplesPerBuffer);
            for (int s = 0; s < samplesPerBuffer; s++)
            {
                channelDest[s] = singleChannelData[s];
            }
        }

        buffer->addToBuffer (samples, sampleNumbers, timestamps, event_codes, samplesPerBuffer);

        int64 uSecElapsed = int64 (Time::highResolutionTicksToSeconds (Time::getHighResolutionTicks() - start) * 1e6);

        if (uSecElapsed < (uSecPerBuffer * bufferCount))
        {
            std::this_thread::sleep_for (std::chrono::microseconds ((uSecPerBuffer * bufferCount) - uSecElapsed));
        }
    }
}