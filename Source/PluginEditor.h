#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include <deque>

class DelayEditor : public Component,
	private Slider::Listener,
	private TextEditor::Listener,
	private ToggleButton::Listener
{
public:
	DelayEditor(AudioProcessorWithDelays& processor, int channelId);

	void sliderValueChanged(Slider* slider) override;
	void textEditorReturnKeyPressed(TextEditor & editor) override;
	void buttonClicked(Button*  button) override;

	DelayEditor& operator=(const DelayEditor& other) = delete;

	void resized() override;

private:
	AudioProcessorWithDelays& _processor;
	const int _channelId;

	Slider _delaySlider;
	TextEditor _editor;
	ToggleButton _onOffSwitch;
};

class AudioProcessorAudioProcessorEditor : public AudioProcessorEditor
{
public:
	AudioProcessorAudioProcessorEditor(AudioProcessorWithDelays&);
	~AudioProcessorAudioProcessorEditor();

	void createEditor(const int channelId);

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	AudioProcessorWithDelays& processor;

	std::deque<DelayEditor> _editors;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
