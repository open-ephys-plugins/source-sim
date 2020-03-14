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

DataThread* SourceThread::createDataThread(SourceNode *sn)
{
	return new SourceThread(sn);
}

/*
GenericEditor* SourceThread::createEditor(SourceNode* sn)
{
    return new NeuropixEditor(sn, this, true);
}
*/

SourceThread::SourceThread(SourceNode* sn) : 
	DataThread(sn),
	recordingTimer(this)
{

}

SourceThread::~SourceThread()
{

}

bool SourceThread::foundInputSource()
{
    return false;
}

void SourceThread::setDefaultChannelNames()
{

}


/** Initializes data transfer.*/
bool SourceThread::startAcquisition()
{

    // clear the internal buffer (happens in initializeProbe)
	//for (int i = 0; i < sourceBuffers.size(); i++)
	//{
	//	sourceBuffers[i]->clear();
	//}

	//startTimer(500 * totalProbes); // wait for signal chain to be built
	
    return true;
}

void SourceThread::timerCallback()
{
	startThread();
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

    if (isThreadRunning())
        signalThreadShouldExit();

    return true;
}


bool SourceThread::usesCustomNames() const
{
	return true;
}


/** Returns the number of virtual subprocessors this source can generate */
unsigned int SourceThread::getNumSubProcessors() const
{
	return 1;
}

/** Returns the number of continuous headstage channels the data source can provide.*/
int SourceThread::getNumDataOutputs(DataChannel::DataChannelTypes type, int subProcessorIdx) const
{

	return 1;
}

/** Returns the number of TTL channels that each subprocessor generates*/
int SourceThread::getNumTTLOutputs(int subProcessorIdx) const 
{
	return 1;
}

/** Returns the sample rate of the data source.*/
float SourceThread::getSampleRate(int subProcessorIdx) const
{
	return 1;
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
