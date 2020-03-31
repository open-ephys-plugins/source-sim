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

#include <VisualizerEditorHeaders.h>

class UtilityButton;
class SourceNode;

class SourceSimCanvas;
class SourceSimInterface;

class NumericEntry : public Label
{
public:
	NumericEntry(String name, String text) : Label(name, text) {};
	~NumericEntry() {};
	virtual TextEditor* createEditorComponent() override;
};

class SourceSimEditor : public VisualizerEditor, public ComboBox::Listener, public Label::Listener
{
public:
	SourceSimEditor(GenericProcessor* parentNode, SourceThread* thread, bool useDefaultParameterEditors);
	virtual ~SourceSimEditor();

	void collapsedStateChanged() override;

	void comboBoxChanged(ComboBox*);
	void labelTextChanged (Label*);
	void buttonEvent(Button*) override;

	void saveEditorParameters(XmlElement*);
	void loadEditorParameters(XmlElement*);

	Visualizer* createNewCanvas(void);


private:

	ScopedPointer<Label> clockFreqLabel;
	ScopedPointer<NumericEntry> clockFreqEntry;

	ScopedPointer<Label> clockTolLabel;
	ScopedPointer<NumericEntry> clockTolEntry;

	ScopedPointer<Label> NPX1Label;
	ScopedPointer<ToggleButton> NPX1ClkEnable;

	ScopedPointer<Label> NPX2Label;
	ScopedPointer<ToggleButton> NPX2ClkEnable;

	ScopedPointer<Label> NIDAQLabel;
	ScopedPointer<ToggleButton> NIDAQClkEnable;

	ScopedPointer<UtilityButton> addDeviceButton;

	Viewport* viewport;
	SourceSimCanvas* canvas;
	SourceThread* thread;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceSimEditor);

};

class SourceSimCanvas : public Visualizer, public Button::Listener
{
public:
	SourceSimCanvas(GenericProcessor* p, SourceSimEditor*, SourceThread*);
	~SourceSimCanvas();

	void paint(Graphics& g);

	void refresh();

	void beginAnimation();
	void endAnimation();

	void refreshState();
	void update();

	void setParameter(int, float);
	void setParameter(int, int, int, float);
	void buttonClicked(Button* button);;

	void saveVisualizerParameters(XmlElement* xml);
	void loadVisualizerParameters(XmlElement* xml);

	void resized();

	SourceNode* processor;
	ScopedPointer<Viewport> sourceSimViewport;
	OwnedArray<SourceSimInterface> sourceSimInterfaces;

	int option;

	SourceSimEditor* editor;

};

class SourceSimInterface : public Component, public Button::Listener, public ComboBox::Listener, public Label::Listener, public Timer
{
public:
	SourceSimInterface(XmlElement info_, int id, SourceThread*, SourceSimEditor*);
	~SourceSimInterface();

	void paint(Graphics& g);

	void updateInfoString();

	void mouseMove(const MouseEvent& event);
	void mouseDown(const MouseEvent& event);
	void mouseDrag(const MouseEvent& event);
	void mouseUp(const MouseEvent& event);
	void mouseWheelMove(const MouseEvent&  event, const MouseWheelDetails&   wheel) ;
	MouseCursor getMouseCursor();

	void buttonClicked(Button*);
	void comboBoxChanged(ComboBox*);

	void saveParameters(XmlElement* xml);
	void loadParameters(XmlElement* xml);

	void timerCallback();

	int id;

private:
	
	SourceThread* thread;
	SourceSimEditor* editor;
	DataBuffer* inputBuffer;
	AudioSampleBuffer displayBuffer;

	
	XmlElement source_sim_info;

	MouseCursor::StandardCursorType cursorType;

};

#endif 
