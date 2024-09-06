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

#include "SourceSimEditor.h"

TextEditor* NumericEntry::createEditorComponent()
{
	TextEditor* const ed = Label::createEditorComponent();
    ed->setInputRestrictions(4, "0123456789.");
    return ed;
}


SourceSimEditor::SourceSimEditor(GenericProcessor* parentNode, SourceSimThread* t)
	: GenericEditor(parentNode)
{

	desiredWidth = 180;
    thread = t;

	clockFreqLabel = new Label("clkFreqLabel", "CLK (Hz)");
	clockFreqLabel->setBounds(5,30,50,20);
	addAndMakeVisible(clockFreqLabel);

	clockFreqEntry = new NumericEntry("clkFreqEntry", "10");
	clockFreqEntry->setBounds(55,30,40,20);
	clockFreqEntry->setEditable(false, true);
	clockFreqEntry->setJustificationType(Justification::centredRight);
	clockFreqEntry->setText("1", juce::NotificationType::sendNotification);
	clockFreqEntry->addListener(this);
	addAndMakeVisible(clockFreqEntry);

	clockTolLabel = new Label("clkFreqLabel", "+/- ");
	clockTolLabel->setBounds(95,30,30,20);
	addAndMakeVisible(clockTolLabel);

	clockTolEntry = new NumericEntry("clkFreqEntry", "0");
	clockTolEntry->setBounds(120,30,40,20);
	clockTolEntry->setEditable(false, true);
	clockTolEntry->setJustificationType(Justification::centredRight);
	clockTolEntry->setText("0", juce::NotificationType::sendNotification);
	clockTolEntry->setEnabled(false);
	clockTolEntry->addListener(this);
	addAndMakeVisible(clockTolEntry);

	//Add title labels
	deviceLabel = new Label("Dev:", "Device:");
	deviceLabel->setBounds(5,55,120,20);
	addAndMakeVisible(deviceLabel);

	channelsLabel = new Label("CH:", "CH:");
	channelsLabel->setBounds(95,55,30,20);
	addAndMakeVisible(channelsLabel);

	quantityLabel = new Label("QTY:", "QTY:");
	quantityLabel->setBounds(130,55,40,20);
	addAndMakeVisible(quantityLabel);

	NPXDeviceLabel = new Label("NPX1 Probe", "NPX1 Probe");
	NPXDeviceLabel->setBounds(5,80,85,20);
	addAndMakeVisible(NPXDeviceLabel);

	addTextBoxParameterEditor(Parameter::PROCESSOR_SCOPE, "npx_chans", 95, 80);
	getParameterEditor("npx_chans")->setLayout(ParameterEditor::nameHidden);
    getParameterEditor("npx_chans")->setSize(40, 20);

	addTextBoxParameterEditor(Parameter::PROCESSOR_SCOPE, "npx_probes", 140, 80);
	getParameterEditor("npx_probes")->setLayout(ParameterEditor::nameHidden);
    getParameterEditor("npx_probes")->setSize(30, 20);

	NIDAQDeviceLabel = new Label("NIDAQ", "NIDAQ-Sim");
	NIDAQDeviceLabel->setBounds(5,105,85,20);
	addAndMakeVisible(NIDAQDeviceLabel);

	addTextBoxParameterEditor(Parameter::PROCESSOR_SCOPE, "nidaq_chans", 95, 105);
	getParameterEditor("nidaq_chans")->setLayout(ParameterEditor::nameHidden);
    getParameterEditor("nidaq_chans")->setSize(40, 20);

	addTextBoxParameterEditor(Parameter::PROCESSOR_SCOPE, "nidaq_devices", 140, 105);
	getParameterEditor("nidaq_devices")->setLayout(ParameterEditor::nameHidden);
    getParameterEditor("nidaq_devices")->setSize(30, 20);

}

SourceSimEditor::~SourceSimEditor()
{

}

void SourceSimEditor::labelTextChanged (Label* label)
{

	std::cout << "Detected label change" << std::endl;

	int freq = clockFreqEntry->getText().getIntValue();
	float tol = clockTolEntry->getText().getFloatValue();

	if (label == clockFreqEntry)
	{
		/* Restrict to integer values only */
		label->setText(String(freq),juce::NotificationType::sendNotification);

		thread->updateClkFreq(freq, tol);
		return;
	}
	else if (label == clockTolEntry)
	{
		/*Restrict to % of clk freq */
		if (!(tol <= (float)freq / 10))
		{
			label->setText("0", juce::NotificationType::sendNotification);
			tol = 0;
		}

		thread->updateClkFreq(freq, tol);
		return;
	}
	
    CoreServices::updateSignalChain(this);	
	
}
