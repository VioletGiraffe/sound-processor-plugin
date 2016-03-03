/*
  ==============================================================================

  This file was auto-generated by the Introjucer!

  It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
  */

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class AudioProcessorAudioProcessorEditor : public AudioProcessorEditor,
	private Slider::Listener,
	private TextEditor::Listener,
	private ToggleButton::Listener
{
public:
	AudioProcessorAudioProcessorEditor(AudioProcessorWithDelays&);
	~AudioProcessorAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;

	void sliderValueChanged (Slider* slider) override;
	void textEditorReturnKeyPressed (TextEditor & editor) override;
	void buttonClicked (Button*  button) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	AudioProcessorWithDelays& processor;

	Slider _delaySlider;
	TextEditor _editor;
	ToggleButton _onOffSwitch;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
