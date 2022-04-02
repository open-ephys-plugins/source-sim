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

	desiredWidth = 250;
    thread = t;

	clockFreqLabel = new Label("clkFreqLabel", "CLK (Hz)");
	clockFreqLabel->setBounds(5,30,50,20);
	addAndMakeVisible(clockFreqLabel);

	clockFreqEntry = new NumericEntry("clkFreqEntry", "10");
	clockFreqEntry->setBounds(55,30,40,20);
	clockFreqEntry->setEditable(false, true);
	clockFreqEntry->setColour(Label::backgroundColourId, Colours::grey);
	clockFreqEntry->setColour(Label::backgroundWhenEditingColourId, Colours::white);
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
	clockTolEntry->setColour(Label::backgroundColourId, Colours::grey);
	clockTolEntry->setColour(Label::backgroundWhenEditingColourId, Colours::white);
	clockTolEntry->setJustificationType(Justification::centredRight);
	clockTolEntry->setText("0", juce::NotificationType::sendNotification);
	clockTolEntry->setEnabled(false);
	clockTolEntry->addListener(this);
	addAndMakeVisible(clockTolEntry);

	//Add title labels
	deviceLabel = new Label("Dev:", "Dev:");
	deviceLabel->setBounds(5,55,120,20);
	addAndMakeVisible(deviceLabel);

	channelsLabel = new Label("CH:", "CH:");
	channelsLabel->setBounds(95,55,30,20);
	addAndMakeVisible(channelsLabel);

	quantityLabel = new Label("QTY:", "QTY:");
	quantityLabel->setBounds(130,55,40,20);
	addAndMakeVisible(quantityLabel);

	NPXDeviceLabel = new Label("NPX1 Probe", "NPX1 Probe");
	NPXDeviceLabel->setBounds(5,80,120,20);
	addAndMakeVisible(NPXDeviceLabel);

	NPXChannelsEntry = new NumericEntry("NPXChannelsEntry", "0");
	NPXChannelsEntry->setBounds(95,80,40,20);
	NPXChannelsEntry->setEditable(false, true);
	NPXChannelsEntry->setColour(Label::backgroundColourId, Colours::grey);
	NPXChannelsEntry->setColour(Label::backgroundWhenEditingColourId, Colours::white);
	NPXChannelsEntry->setJustificationType(Justification::centredRight);
	NPXChannelsEntry->setText(String(384), juce::NotificationType::sendNotification);
	NPXChannelsEntry->addListener(this);
	addAndMakeVisible(NPXChannelsEntry);

	NPXQuantityEntry = new NumericEntry("NPXQuantityEntry", "0");
	NPXQuantityEntry->setBounds(137,80,30,20);
	NPXQuantityEntry->setEditable(false, true);
	NPXQuantityEntry->setColour(Label::backgroundColourId, Colours::grey);
	NPXQuantityEntry->setColour(Label::backgroundWhenEditingColourId, Colours::white);
	NPXQuantityEntry->setJustificationType(Justification::centredRight);
	NPXQuantityEntry->setText(String(1), juce::NotificationType::sendNotification);
	NPXQuantityEntry->addListener(this);
	addAndMakeVisible(NPXQuantityEntry);

	NIDAQDeviceLabel = new Label("NIDAQ", "NIDAQ-Sim");
	NIDAQDeviceLabel->setBounds(5,105,120,20);
	addAndMakeVisible(NIDAQDeviceLabel);

	NIDAQChannelsEntry = new NumericEntry("NIDAQChannelsEntry", "0");
	NIDAQChannelsEntry->setBounds(95,105,40,20);
	NIDAQChannelsEntry->setEditable(false, true);
	NIDAQChannelsEntry->setColour(Label::backgroundColourId, Colours::grey);
	NIDAQChannelsEntry->setColour(Label::backgroundWhenEditingColourId, Colours::white);
	NIDAQChannelsEntry->setJustificationType(Justification::centredRight);
	NIDAQChannelsEntry->setText(String(16), juce::NotificationType::sendNotification);
	NIDAQChannelsEntry->addListener(this);
	addAndMakeVisible(NIDAQChannelsEntry);

	NIDAQQuantityEntry = new NumericEntry("NIDAQQuantityEntry", "0");
	NIDAQQuantityEntry->setBounds(137,105,30,20);
	NIDAQQuantityEntry->setEditable(false, true);
	NIDAQQuantityEntry->setColour(Label::backgroundColourId, Colours::grey);
	NIDAQQuantityEntry->setColour(Label::backgroundWhenEditingColourId, Colours::white);
	NIDAQQuantityEntry->setJustificationType(Justification::centredRight);
	NIDAQQuantityEntry->setText(String(1), juce::NotificationType::sendNotification);
	NIDAQQuantityEntry->addListener(this);
	addAndMakeVisible(NIDAQQuantityEntry);


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
	
	if (label == NPXChannelsEntry)
	{
		int channels = NPXChannelsEntry->getText().getIntValue();
		
		if (channels < 0 || channels > 384)
		{
		    channels = 384;
            NPXChannelsEntry->setText(String(channels), juce::NotificationType::sendNotification);
		}
	}
	else if (label == NPXQuantityEntry)
	{
		int numProbes = NPXQuantityEntry->getText().getIntValue();

		if (numProbes < 0 || numProbes > 20)
		{
		    numProbes = 1;
            NPXQuantityEntry->setText(String(numProbes), juce::NotificationType::sendNotification);
		}

	}
	else if (label == NIDAQChannelsEntry)
	{
		int channels = NIDAQChannelsEntry->getText().getIntValue();
		if (channels < 0 || channels > 32)
		{
		    channels = 8;
            NIDAQChannelsEntry->setText(String(channels), juce::NotificationType::sendNotification);
		}

	}
	else if (label == NIDAQQuantityEntry)
	{
		int numDevices = NIDAQQuantityEntry->getText().getIntValue();
		if (numDevices < 0 || numDevices > 20)
		{
		    numDevices = 1;
            NIDAQQuantityEntry->setText(String(numDevices), juce::NotificationType::sendNotification);
		}

	}
	
    CoreServices::updateSignalChain(this);	
	
}

void SourceSimEditor::getSettings(PluginSettingsObject& settings)
{
	settings.numProbes = NPXQuantityEntry->getText().getIntValue();
	settings.channelsPerProbe = NPXChannelsEntry->getText().getIntValue();
	settings.numNIDAQ = NIDAQQuantityEntry->getText().getIntValue();
	settings.channelsPerNIDAQ = NIDAQChannelsEntry->getText().getIntValue();
}

void SourceSimEditor::startAcquisition()
{
	NPXChannelsEntry->setEnabled(false);
	NPXQuantityEntry->setEnabled(false);
	NIDAQChannelsEntry->setEnabled(false);
	NIDAQQuantityEntry->setEnabled(false);
}

void SourceSimEditor::stopAcquisition()
{
	NPXChannelsEntry->setEnabled(true);
	NPXQuantityEntry->setEnabled(true);
	NIDAQChannelsEntry->setEnabled(true);
	NIDAQQuantityEntry->setEnabled(true);
}


void SourceSimEditor::saveCustomParametersToXml(XmlElement* xml)
{

	xml->setAttribute("npx_channels", NPXChannelsEntry->getText());
	xml->setAttribute("npx_quantity", NPXQuantityEntry->getText());
	xml->setAttribute("nidaq_channels", NIDAQChannelsEntry->getText());
	xml->setAttribute("nidaq_quantity", NIDAQQuantityEntry->getText());

}

void SourceSimEditor::loadCustomParametersFromXml(XmlElement* xml)
{
    
	NPXChannelsEntry->setText(xml->getStringAttribute("npx_channels", "384"), dontSendNotification);
	NPXQuantityEntry->setText(xml->getStringAttribute("npx_quantity", "1"), dontSendNotification);
	NIDAQChannelsEntry->setText(xml->getStringAttribute("nidaq_channels", "16"), dontSendNotification);
	NIDAQQuantityEntry->setText(xml->getStringAttribute("nidaq_quantity", "1"), dontSendNotification);
    
}
