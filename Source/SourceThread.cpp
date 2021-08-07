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

    continuousChannels->clear();
    eventChannels->clear();
    spikeChannels->clear();
    sourceStreams->clear();
    devices->clear();
    configurationObjects->clear();

    for (int i = 0; i < numProbes; i++)
    {



        //Add Neuropixels AP Band Stream
        DataStream::Settings apSettings {
            "Probe " + String(i) + " AP band",
            "Neural data sampled @30kHz ",
            "SP" + String(i) + "_AP",
            30000.0f
        };

        sourceStreams->add(new DataStream(apSettings));

        sources.add(new NPX_AP_BAND(numChannelsPerProbe));
        sourceBuffers.add(new DataBuffer(numChannelsPerProbe, 1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        for (int j = 0; j < numChannelsPerProbe; j++)
        {

            ContinuousChannel::Settings channelSettings
            {
                ContinuousChannel::Type::ELECTRODE,
                "CH" + String(j + 1),
                "AP voltage from electrode " + String(j+1),
                "source",
                0.195f, // BITVOLTS VALUE
                sourceStreams->getLast()
            };
            continuousChannels->add(new ContinuousChannel(channelSettings));
        }

		EventChannel *apSyncLine;

        EventChannel::Settings apSyncSettings {
            EventChannel::Type::TTL,
            "AP Sync Line",
            "Synchronization signal from the AP band of simulated probe " + String(i),
            "probe.sync",
            sourceStreams->getLast()
        };

        apSyncLine = new EventChannel(apSyncSettings);
		apSyncLine->setIdentifier("Probe" + String(i) + "AP sync line");
		eventChannels->add(apSyncLine);




        //Add a Neuropixels LFP Band Stream
        DataStream::Settings lfpSettings {
            "Probe " + String(i) + " LFP band",
            "Neural data sampled @2.5kHz ",
            "SP" + String(i) + "_LFP",
            2500.0f
        };

        sourceStreams->add(new DataStream(lfpSettings));

        sources.add(new NPX_LFP_BAND(numChannelsPerProbe));
        sourceBuffers.add(new DataBuffer(numChannelsPerProbe, 1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        for (int j = 0; j < numChannelsPerProbe; j++)
        {

            ContinuousChannel::Settings channelSettings
            {
                ContinuousChannel::Type::ELECTRODE,
                "CH" + String(j + 1),
                "LFP voltage from electrode " + String(j+1),
                "source",
                0.195f, // BITVOLTS VALUE
                sourceStreams->getLast()
            };
            continuousChannels->add(new ContinuousChannel(channelSettings));
        }


        EventChannel *lfpSyncLine;

        EventChannel::Settings lfpSyncSettings {
            EventChannel::Type::TTL,
            "LFP Sync Line",
            "Synchronization signal from the LFP band of simulated probe " + String(i),
            "probe.sync",
            sourceStreams->getLast()
        };

        lfpSyncLine = new EventChannel(lfpSyncSettings);
        lfpSyncLine->setIdentifier("Probe" + String(i) + "LFP sync line");
        eventChannels->add(lfpSyncLine);

    }

    for (int i = 0; i < numNIDevices; i++)
    {

        //Add a NIDAQ stream
        DataStream::Settings nidaqSettings {
            "NIDAQ Device",
            "Neural data sampled @2.5kHz ",
            "P" + String(i) + "_LFP",
            2500.0f
        };

        sourceStreams->add(new DataStream(nidaqSettings));

        sources.add(new NIDAQ(numChannelsPerProbe));
        sourceBuffers.add(new DataBuffer(numChannelsPerNIDAQDevice, 1000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        for (int j = 0; j < numChannelsPerNIDAQDevice; j++)
        {

            ContinuousChannel::Settings settings
            {
                ContinuousChannel::Type::ADC,
                "CH" + String(j + 1),
                "Analog voltage from input " + String(j+1),
                "source",
                0.195f, // BITVOLTS VALUE
                sourceStreams->getLast()
            };
            continuousChannels->add(new ContinuousChannel(settings));
        }

        EventChannel *nidaqDigitalLine;

        EventChannel::Settings eventSettings {
            EventChannel::Type::TTL,
            "NIDAQ Digital Line",
            "NIDAQ digital line events from device " + String(i),
            "nidaq.digital",
            sourceStreams->getLast()
        };

        nidaqDigitalLine = new EventChannel(eventSettings);
        nidaqDigitalLine->setIdentifier("NIDAQ device" + String(i) + "digital line");
        eventChannels->add(nidaqDigitalLine);

    }

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
    sn->update();
}

void SourceThread::updateNumProbes(int probes)
{
    numProbes = probes;
    sn->update();
}
	
void SourceThread::updateNIDAQChannels(int channels)
{
    numChannelsPerNIDAQDevice = channels;
    sn->update();
}

void SourceThread::updateNIDAQDeviceCount(int count)
{
    numNIDevices = count; 
    sn->update();
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
