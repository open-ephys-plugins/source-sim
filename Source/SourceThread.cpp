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
#define NUM_NI_DEVICES 1
#define AP_CHANNELS 384
#define LFP_CHANNELS 384
#define APT_CHANNELS 384
#define NIDAQ_CHANNELS 8

DataThread* SourceThread::createDataThread(SourceNode *sn)
{
	return new SourceThread(sn);
}


std::unique_ptr<GenericEditor> SourceThread::createEditor(SourceNode* sn)
{
    return std::make_unique<SourceSimEditor>(sn, this, true);
}


SourceThread::SourceThread(SourceNode* sn) : 
	DataThread(sn),
	recordingTimer(this),
    numProbes(NUM_PROBES),
    numChannelsPerProbe(AP_CHANNELS),
	numNIDevices(NUM_NI_DEVICES),
	numChannelsPerNIDAQDevice(NIDAQ_CHANNELS)
{
    generateBuffers();
}

SourceThread::~SourceThread()
{

}

void SourceThread::updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
		OwnedArray<EventChannel>* eventChannels,
		OwnedArray<SpikeChannel>* spikeChannels,
		OwnedArray<DataStream>* sourceStreams,
		OwnedArray<DeviceInfo>* devices,
		OwnedArray<ConfigurationObject>* configurationObjects)
{
    //TODO
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

void SourceThread::updateNPXChannels(int channels)
{
    numChannelsPerProbe = channels;
    generateBuffers();
    sn->update();
}

void SourceThread::updateNumProbes(int probes)
{
    numProbes = probes;
    generateBuffers();
    sn->update();
}
	
void SourceThread::updateNIDAQChannels(int channels)
{
    numChannelsPerNIDAQDevice = channels;
    generateBuffers();
    sn->update();
}

void SourceThread::updateNIDAQDeviceCount(int count)
{
    numNIDevices = count; 
    generateBuffers();
    sn->update();
}

void SourceThread::generateBuffers()
{

    sources.clear();
    sourceBuffers.clear();

    for (int i = 0; i < numProbes; i++)
    {

        //Add Neuropixels AP Band
        sources.add(new NPX_AP_BAND(numChannelsPerProbe));
        sourceBuffers.add(new DataBuffer(sources.getLast()->numChannels,1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        //Add Neuropixels LFP Band
        sources.add(new NPX_LFP_BAND(numChannelsPerProbe));
        sourceBuffers.add(new DataBuffer(sources.getLast()->numChannels,1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

    }

    // //Add NIDAQ Band
    for (int i = 0; i < numNIDevices; i++)
    {
        sources.add(new NIDAQ(numChannelsPerNIDAQDevice));
        sourceBuffers.add(new DataBuffer(sources.getLast()->numChannels,1000));
        sources.getLast()->buffer = sourceBuffers.getLast();
    }	

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
