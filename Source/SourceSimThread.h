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

#ifndef __SOURCESIMTHREAD_H__
#define __SOURCESIMTHREAD_H__

#include <DataThreadHeaders.h>
#include <stdio.h>
#include <string.h>

#include "SimulatedSource.h"

class SourceSimEditor;

struct PluginSettingsObject
{
    int clkFreq; // Hz
    int numProbes;
    int channelsPerProbe;
    int numNIDAQ;
    int channelsPerNIDAQ;
};

/**

	Simulates multiple data streams, for testing purposes.

	@see DataThread, SourceNode

*/

class SourceSimThread : public DataThread
{
public:
    /** Constructor */
    SourceSimThread (SourceNode* sn);

    /** Destructor */
    ~SourceSimThread();

    /** Static method to create DataThread */
    static DataThread* createDataThread (SourceNode* sn);

    /** Creates the custom editor */
    std::unique_ptr<GenericEditor> createEditor (SourceNode* sn);

    /**Create & register parameters*/
    void registerParameters() override;

    /** Not used -- data buffers are updated inside simulated sources*/
    bool updateBuffer() { return true; }

    /** Returns true if the data source is connected, false otherwise.*/
    bool foundInputSource();

    /** Initializes data transfer.*/
    bool startAcquisition();

    /** Stops data transfer.*/
    bool stopAcquisition();

    // DataThread Methods
    void updateSettings (OwnedArray<ContinuousChannel>* continuousChannels,
                         OwnedArray<EventChannel>* eventChannels,
                         OwnedArray<SpikeChannel>* spikeChannels,
                         OwnedArray<DataStream>* sourceStreams,
                         OwnedArray<DeviceInfo>* devices,
                         OwnedArray<ConfigurationObject>* configurationObjects);

    void updateClkFreq (int freq, float tol);
    void updateClkEnable (int subProcIdx, bool enable);

    /* Called when a parameter value is updated, to allow plugin-specific responses*/
    void parameterValueChanged (Parameter* param) override;

private:
    OwnedArray<SimulatedSource> sources;

    SourceSimEditor* sse;

    PluginSettingsObject settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceSimThread);
};

#endif // __NEUROPIXTHREAD_H_2C4CBD67__
