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

#include "SourceSim.h"

#include <DataThreadHeaders.h>
#include <stdio.h>
#include <string.h>

class SourceNode;
class SourceThread;

class RecordingTimer : public Timer
{

public:

	RecordingTimer(SourceThread* t_);
	void timerCallback();

	SourceThread* thread;
};


/**

	Communicates with imec Neuropixels probes.

	@see DataThread, SourceNode

*/



class SourceThread : public DataThread, public Timer
{

public:

	SourceThread(SourceNode* sn);
	~SourceThread();

	int numProbes;
	int numChannelsPerProbe;
	int numNIDevices;
	int numChannelsPerNIDAQDevice;

	bool updateBuffer();

	void updateNPXChannels(int channels);
	void updateNumProbes(int probes);
	void updateNIDAQChannels(int channels);
	void updateNIDAQDeviceCount(int count);

	/** Returns true if the data source is connected, false otherwise.*/
	bool foundInputSource();

	/** Returns version and serial number info for hardware and API as a string.*/
	String getInfoString();

	/** Returns version and serial number info for hardware and API as XML.*/
	XmlElement getInfoXml();

	void openConnection();

	/** Initializes data transfer.*/
	bool startAcquisition();

	/** Stops data transfer.*/
	bool stopAcquisition();

	// DataThread Methods
	void updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
		OwnedArray<EventChannel>* eventChannels,
		OwnedArray<SpikeChannel>* spikeChannels,
		OwnedArray<DataStream>* sourceStreams,
		OwnedArray<DeviceInfo>* devices,
		OwnedArray<ConfigurationObject>* configurationObjects);

	/** Selects which electrode is connected to each channel. */
	void selectElectrodes(unsigned char slot, signed char port, Array<int> channelStatus);

	/** Selects which reference is used for each channel. */
	void setAllReferences(unsigned char slot, signed char port, int refId);

	/** Sets the gain for each channel. */
	void setAllGains(unsigned char slot, signed char port, unsigned char apGain, unsigned char lfpGain);

	/** Sets the filter for all channels. */
	void setFilter(unsigned char slot, signed char port, bool filterState);

	/** Toggles between internal and external triggering. */
	void setTriggerMode(bool trigger);

	/** Toggles between saving to NPX file. */
	void setRecordMode(bool record);

	/** Select directory for saving NPX files. */
	void setDirectoryForSlot(int slotIndex, File directory);

	/** Select directory for saving NPX files. */
	File getDirectoryForSlot(int slotIndex);

	/** Toggles between auto-restart setting. */
	void setAutoRestart(bool restart);

	/** Starts data acquisition after a certain time.*/
	void timerCallback();

	/** Starts recording.*/
	void startRecording();

	/** Stops recording.*/
	void stopRecording();

	CriticalSection* getMutex()
	{
		return &displayMutex;
	}

	static DataThread* createDataThread(SourceNode* sn);

	std::unique_ptr<GenericEditor> createEditor(SourceNode* sn);

	OwnedArray<SourceSim> sources;

	void updateClkFreq(int freq, float tol);
	void updateClkEnable(int subProcIdx, bool enable);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceThread);

private:

	CriticalSection displayMutex;

	RecordingTimer recordingTimer;

	bool settingsUpdated;

};





#endif  // __NEUROPIXTHREAD_H_2C4CBD67__
