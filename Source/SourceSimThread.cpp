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

#include "SourceSimThread.h"

#include "SourceSimEditor.h"
#include <cmath>

DataThread* SourceSimThread::createDataThread(SourceNode *sn)
{
	return new SourceSimThread(sn);
}


std::unique_ptr<GenericEditor> SourceSimThread::createEditor(SourceNode* sn)
{
    std::unique_ptr<SourceSimEditor> e = std::make_unique<SourceSimEditor>(sn, this);
    sse = e.get();

    return e;
}


SourceSimThread::SourceSimThread(SourceNode* sn) :
	DataThread(sn)
{
}

SourceSimThread::~SourceSimThread()
{

}

void SourceSimThread::registerParameters()
{
    settings.numProbes = 1;
    settings.channelsPerProbe = 384;
    settings.numNIDAQ = 1;
    settings.channelsPerNIDAQ = 16;
    
    addIntParameter(Parameter::PROCESSOR_SCOPE, "npx_chans", "NPX Chans", "Number of channels per probe",
        settings.channelsPerProbe, 1, 10000, true);

    addIntParameter(Parameter::PROCESSOR_SCOPE, "npx_probes", "NPX Probes", "Number of probes",
        settings.numProbes, 0, 20, true);

    addIntParameter(Parameter::PROCESSOR_SCOPE, "nidaq_chans", "NIDAQ Chans", "Number of channels per device",
        settings.channelsPerNIDAQ, 1, 32, true);

    addIntParameter(Parameter::PROCESSOR_SCOPE, "nidaq_devices", "NIDAQ Devices", "Number of NIDAQs",
        settings.numNIDAQ, 0, 20, true);
}

void SourceSimThread::updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
		OwnedArray<EventChannel>* eventChannels,
		OwnedArray<SpikeChannel>* spikeChannels,
		OwnedArray<DataStream>* dataStreams,
		OwnedArray<DeviceInfo>* devices,
		OwnedArray<ConfigurationObject>* configurationObjects)
{

    continuousChannels->clear();
    eventChannels->clear();
    spikeChannels->clear();
    dataStreams->clear();
    devices->clear();
    configurationObjects->clear();

    sourceBuffers.clear();
    sources.clear();

    std::vector<std::string> probeNames = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O" , "P" };

    for (int i = 0; i < settings.numProbes; i++)
    {
        //std::cout << "Probe " << i << " AP; " << settings.channelsPerProbe << " channels." << std::endl;

        String apBandName = "Probe-" + probeNames[i] + "-AP";
        String lfpBandName = "Probe-" + probeNames[i] + "-LFP";

        DataStream::Settings apSettings{
            apBandName,
            "Neural data sampled @ 30kHz ",
            "SP" + String(i) + "_AP",
            30000.0f
        };

        dataStreams->add(new DataStream(apSettings));

        sources.add(new SimulatedSource(apBandName, settings.channelsPerProbe, 30000.0f, SimulatedSourceType::AP_BAND));
        sourceBuffers.add(new DataBuffer(settings.channelsPerProbe, 48000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        for (int j = 0; j < settings.channelsPerProbe; j++)
        {

            ContinuousChannel::Settings channelSettings
            {
                ContinuousChannel::Type::ELECTRODE,
                "CH" + String(j + 1),
                "AP voltage from electrode " + String(j + 1),
                "source",
                0.195f, // BITVOLTS VALUE
                dataStreams->getLast()
            };

            continuousChannels->add(new ContinuousChannel(channelSettings));
        }

        EventChannel* apSyncLine;

        EventChannel::Settings apSyncSettings{
            EventChannel::Type::TTL,
            "AP Sync Line",
            "Synchronization signal from the AP band of simulated probe " + String(i),
            "probe.sync",
            dataStreams->getLast()
        };

        apSyncLine = new EventChannel(apSyncSettings);
        eventChannels->add(apSyncLine);

        //std::cout << "Probe " << i << " LFP; " << settings.channelsPerProbe << " channels." << std::endl;

        DataStream::Settings lfpSettings{
            lfpBandName,
            "Neural data sampled @ 2.5kHz ",
            "SP" + String(i) + "_LFP",
            2500.0f
        };

        dataStreams->add(new DataStream(lfpSettings));
        sources.add(new SimulatedSource(lfpBandName, settings.channelsPerProbe, 2500.0, SimulatedSourceType::LFP_BAND));
        sourceBuffers.add(new DataBuffer(settings.channelsPerProbe, 48000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        for (int j = 0; j < settings.channelsPerProbe; j++)
        {

            ContinuousChannel::Settings channelSettings
            {
                ContinuousChannel::Type::ELECTRODE,
                "CH" + String(j + 1),
                "LFP voltage from electrode " + String(j + 1),
                "source",
                0.195f, // BITVOLTS VALUE
                dataStreams->getLast()
            };

            continuousChannels->add(new ContinuousChannel(channelSettings));
        }

        EventChannel* lfpSyncLine;

        EventChannel::Settings lfpSyncSettings{
            EventChannel::Type::TTL,
            "LFP Sync Line",
            "Synchronization signal from the LFP band of simulated probe " + String(i),
            "probe.sync",
            dataStreams->getLast()
        };

        lfpSyncLine = new EventChannel(lfpSyncSettings);
        eventChannels->add(lfpSyncLine);

    }

    for (int i = 0; i < settings.numNIDAQ; i++)
    {

        //std::cout << "NI Device " << i << "; " << settings.channelsPerNIDAQ << " channels." << std::endl;

        String nidaqName = "Dev" + String(i);

        DataStream::Settings nidaqSettings{
            nidaqName,
            "NIDAQ @ 30 kHz ",
            "NI" + String(i),
            30000.0f
        };

        dataStreams->add(new DataStream(nidaqSettings));
        sources.add(new SimulatedSource(nidaqName, settings.channelsPerNIDAQ, 30000.0f, SimulatedSourceType::NIDAQ));
        sourceBuffers.add(new DataBuffer(settings.channelsPerNIDAQ, 48000));
        sources.getLast()->buffer = sourceBuffers.getLast();

        for (int j = 0; j < settings.channelsPerNIDAQ; j++)
        {

            ContinuousChannel::Settings channelSettings
            {
                ContinuousChannel::Type::ADC,
                "CH" + String(j + 1),
                "ADC voltage from channel " + String(j + 1),
                "source",
                0.195f, // BITVOLTS VALUE
                dataStreams->getLast()
            };

            continuousChannels->add(new ContinuousChannel(channelSettings));
        }

        EventChannel* nidaqSyncLine;

        EventChannel::Settings nidaqSyncSettings{
            EventChannel::Type::TTL,
            "NIDAQ Sync Line",
            "Synchronization signal from the NIDAQ " + String(i),
            "probe.sync",
            dataStreams->getLast()
        };

        nidaqSyncLine = new EventChannel(nidaqSyncSettings);
        eventChannels->add(nidaqSyncLine);

    }
}


void SourceSimThread::parameterValueChanged(Parameter* param)
{
    LOGD("Parameter value changed: ", param->getName(), ". New value: ", param->getValueAsString());

    if (param->getName().equalsIgnoreCase("npx_chans"))
    {
        settings.channelsPerProbe = ((IntParameter*)param)->getIntValue();
    }
    else if (param->getName().equalsIgnoreCase("npx_probes"))
    {
        settings.numProbes = ((IntParameter*)param)->getIntValue();
    }
    else if (param->getName().equalsIgnoreCase("nidaq_chans"))
    {
        settings.channelsPerNIDAQ = ((IntParameter*)param)->getIntValue();
    }
    else if (param->getName().equalsIgnoreCase("nidaq_devices"))
    {
        settings.numNIDAQ = ((IntParameter*)param)->getIntValue();
    }

    CoreServices::updateSignalChain(sse);
}


void SourceSimThread::updateClkFreq(int freq, float tol)
{
    std::cout << "Update clk freq: " << freq << " tol: " << tol << std::endl;

    //for (auto source : sources)
   // {
    //    if (source->isTimerRunning())
    //        source->updateClkFreq(freq, tol);
    //}
}


void SourceSimThread::updateClkEnable(int subProcIdx, bool enable)
{
  // sources[subProcIdx]->updateClk(enable);
}


bool SourceSimThread::foundInputSource()
{
    return true;
}

bool SourceSimThread::startAcquisition()
{

    for (auto source : sources)
        source->buffer->clear();

    for (auto source : sources)
        source->startThread();

    return true;
}

bool SourceSimThread::stopAcquisition()
{

    for (auto source : sources)
        source->signalThreadShouldExit();

    return true;
}
