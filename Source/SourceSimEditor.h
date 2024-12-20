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

#ifndef __SOURCESIMEDITOR_H__
#define __SOURCESIMEDITOR_H__

#include <EditorHeaders.h>

#include "SourceSimThread.h"

/** 
	An editable label that only accepts numbers
*/
class NumericEntry : public Label
{
public:
    /** Constructor */
    NumericEntry (String name, String text) : Label (name, text) {};

    /** Destructor */
    ~NumericEntry() {};

    /** Creates the editor component */
    virtual TextEditor* createEditorComponent() override;
};

/** 

	Used to change the number of data streams, and the
	number of channels per stream.

*/
class SourceSimEditor : public GenericEditor
{
public:
    /** Constructor */
    SourceSimEditor (GenericProcessor* parentNode, SourceSimThread* thread);

    /** Destructor */
    virtual ~SourceSimEditor();

private:
    ScopedPointer<Label> clockLabel;
    ScopedPointer<Label> freqLabel;

    ScopedPointer<Label> deviceLabel;
    ScopedPointer<Label> channelsLabel;
    ScopedPointer<Label> quantityLabel;

    ScopedPointer<Label> NPXDeviceLabel;
    ScopedPointer<Label> NIDAQDeviceLabel;

    SourceSimThread* thread;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceSimEditor);
};

#endif
