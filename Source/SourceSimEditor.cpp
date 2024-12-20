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

SourceSimEditor::SourceSimEditor(GenericProcessor* parentNode, SourceSimThread* t)
	: GenericEditor(parentNode)
{
	desiredWidth = 180;
    thread = t;

	clockLabel = new Label("clkFreqLabel", "CLK");
	clockLabel->setBounds(5,30,50,20);
	addAndMakeVisible(clockLabel);

	addTextBoxParameterEditor(Parameter::PROCESSOR_SCOPE, "clk_hz", 95, 30);
	getParameterEditor("clk_hz")->setLayout(ParameterEditor::nameHidden);
	getParameterEditor("clk_hz")->setSize(40, 20);

	freqLabel = new Label("Freq:", "Hz");
	freqLabel->setBounds(140,30,40,20);
	addAndMakeVisible(freqLabel);

	//Add title labels
	deviceLabel = new Label("Dev:", "Device");
	deviceLabel->setBounds(5,55,120,20);
	addAndMakeVisible(deviceLabel);

	channelsLabel = new Label("CH:", "CH");
	channelsLabel->setBounds(95,55,30,20);
	addAndMakeVisible(channelsLabel);

	quantityLabel = new Label("QTY:", "QTY");
	quantityLabel->setBounds(138,55,40,20);
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

SourceSimEditor::~SourceSimEditor() {}