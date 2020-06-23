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

#include "SourceThread.h"
#include "SourceSimEditor.h"
#include <cmath>

#define NUM_PROBES 6

DataThread* SourceThread::createDataThread(SourceNode *sn)
{
	return new SourceThread(sn);
}


GenericEditor* SourceThread::createEditor(SourceNode* sn)
{
    return new SourceSimEditor(sn, this, true);
}


SourceThread::SourceThread(SourceNode* sn) : 
	DataThread(sn),
	recordingTimer(this)
{
    generateBuffers();
}
SourceThread::~SourceThread()
{

}

void SourceThread::updateClkFreq(int freq, float tol)
{
    std::cout << "Update clk freq: " << freq << " tol: " << tol << std::endl;

    for (auto source : sources)
    {
        if (source->isTimerRunning())
            source->updateClkFreq(freq, tol);
    }
}


void SourceThread::updateClkEnable(int subProcIdx, bool enable)
{
    sources[subProcIdx]->updateClk(enable);
}

void SourceThread::generateBuffers()
{

    for (int i = 0; i < NUM_PROBES; i++)
    {

        //Add Neuropixels AP Band
        sources.add(new APTrain());
        sourceBuffers.add(new DataBuffer(sources.getLast()->numChannels,1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        //Add Neuropixels LFP Band
        sources.add(new NPX_LFP_BAND());
        sourceBuffers.add(new DataBuffer(sources.getLast()->numChannels,1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

    }

    // //Add NIDAQ Band
    sources.add(new NIDAQ());
    sourceBuffers.add(new DataBuffer(sources.getLast()->numChannels,1000));
    sources.getLast()->buffer = sourceBuffers.getLast();

}

bool SourceThread::foundInputSource()
{
    return true;
}

bool SourceThread::startAcquisition()
{

	sourceBuffers.getLast()->clear();

    for (int i = 0; i < sources.size(); i++)
    {
        sources[i]->startThread();
    }

    this->startThread();
	
    return true;
}

void SourceThread::timerCallback()
{
	sources.getLast()->startThread();
    stopTimer();
}

void SourceThread::startRecording()
{

}

void SourceThread::stopRecording()
{
}

/** Stops data transfer.*/
bool SourceThread::stopAcquisition()
{

    for (auto source : sources)
        source->signalThreadShouldExit();

    if (isThreadRunning())
        signalThreadShouldExit();

    return true;
}

bool SourceThread::usesCustomNames() const
{
	return true;
}

void SourceThread::setDefaultChannelNames()
{

    int absChannel = 0;

    for (int i = 0; i < 2*NUM_PROBES; i+=2)
    {

        //AP
        for (int j = 0; j < sources[i]->numChannels; j++)
        {
            ChannelCustomInfo info;
            info.name = "AP" + String(j + 1);
            info.gain = 1.0f;
            channelInfo.set(absChannel, info);
            absChannel++;
        }

        //LFP
        for (int j = 0; j < sources[i+1]->numChannels; j++)
        {
            ChannelCustomInfo info;
            info.name = "LFP" + String(j + 1);
            info.gain = 1.0f;
            channelInfo.set(absChannel, info);
            absChannel++;
        }

    }

    // //NIDAQ
    for (int i = 0; i < sources[sources.size()-1]->numChannels; i++)
    {
        ChannelCustomInfo info;
        info.name = "AI" + String(i + 1);
        info.gain = 1.0f;
        channelInfo.set(absChannel, info);
        std::cout << "Setting channel " << absChannel << std::endl;
        absChannel++;
    }

}


/** Returns the number of virtual subprocessors this source can generate */
unsigned int SourceThread::getNumSubProcessors() const
{
	return sources.size();
}

/** Returns the number of continuous headstage channels the data source can provide.*/
int SourceThread::getNumDataOutputs(DataChannel::DataChannelTypes type, int subProcessorIdx) const
{

	if (type == DataChannel::DataChannelTypes::HEADSTAGE_CHANNEL && subProcessorIdx < 2 * NUM_PROBES)
        return sources[subProcessorIdx]->numChannels;
	else if (type == DataChannel::DataChannelTypes::ADC_CHANNEL && subProcessorIdx >= 2 * NUM_PROBES)
		return sources[subProcessorIdx]->numChannels;
    
    return 0;

}

/** Returns the number of TTL channels that each subprocessor generates*/
int SourceThread::getNumTTLOutputs(int subProcessorIdx) const 
{
    if (subProcessorIdx < 2 * NUM_PROBES)
	    return 1;
    else 
        return 8;
}

/** Returns the sample rate of the data source.*/
float SourceThread::getSampleRate(int subProcessorIdx) const
{

    return sources[subProcessorIdx]->sampleRate;
    
}

/** Returns the volts per bit of the data source.*/
float SourceThread::getBitVolts(const DataChannel* chan) const
{
	return 1.0f;
}

bool SourceThread::updateBuffer()
{
    return true;
}


RecordingTimer::RecordingTimer(SourceThread* t_)
{
	thread = t_;
}

void RecordingTimer::timerCallback()
{
	thread->startRecording();
	stopTimer();
}
