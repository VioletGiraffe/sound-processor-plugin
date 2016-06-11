#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "util/util.h"

DelayEditor::DelayEditor(AudioProcessorWithDelays& processor, int channelId) : _processor(processor), _channelId(channelId)
{
	_onOffSwitch.setButtonText("On");
	_onOffSwitch.addListener(this);
	_onOffSwitch.setToggleState(_processor.isEnabled(channelId), juce::sendNotificationSync);
	addAndMakeVisible(_onOffSwitch);

	_delaySlider.setSliderStyle(Slider::LinearBar);
	_delaySlider.setRange(0.0, 15.0, 0.01);
	_delaySlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	_delaySlider.setPopupDisplayEnabled(true, this);
	_delaySlider.setTextValueSuffix(" ms");
	_delaySlider.setValue(_processor.delay(channelId));
	_delaySlider.addListener(this);
	addAndMakeVisible(_delaySlider);

	_editor.setMultiLine(false);
	_editor.addListener(this);
	_editor.setText(String(_delaySlider.getValue()));
	addAndMakeVisible(_editor);
}

void DelayEditor::sliderValueChanged(Slider* slider)
{
	if (slider == &_delaySlider && _delaySlider.getValue() != _processor.delay(_channelId))
	{
		_editor.setText(String(_delaySlider.getValue()));
		_processor.setDelay(_delaySlider.getValue(), _channelId);
	}
}

void DelayEditor::textEditorReturnKeyPressed(TextEditor & editor)
{
	_delaySlider.setValue(editor.getText().getDoubleValue());
}

void DelayEditor::buttonClicked(Button* button)
{
	const bool enabled = button->getToggleState();
	if (enabled != _processor.isEnabled(_channelId))
		_processor.setEnabled(enabled, _channelId);
}

void DelayEditor::resized()
{
	auto lBounds = getLocalBounds();
	lBounds.removeFromLeft(10);
	lBounds.removeFromTop(20);
	_onOffSwitch.setBounds(Rectangle<int>(lBounds.getX(), lBounds.getY(), 50, 20));
	lBounds.removeFromLeft(50);

	const int textEditWidth = 50;
	_delaySlider.setBounds(Rectangle<int>(lBounds.getX(), lBounds.getY(), lBounds.getWidth() - 10 - textEditWidth, 20));

	lBounds.removeFromLeft(lBounds.getWidth() - 10 - textEditWidth);
	_editor.setBounds(Rectangle<int>(lBounds.getX(), lBounds.getY(), lBounds.getWidth(), 20));
}


//==============================================================================
AudioProcessorAudioProcessorEditor::AudioProcessorAudioProcessorEditor(AudioProcessorWithDelays& p)
	: AudioProcessorEditor(&p), processor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(500, 400);

	for (int i = 0; i < processor.getTotalNumOutputChannels(); ++i)
		createEditor(i);
}

AudioProcessorAudioProcessorEditor::~AudioProcessorAudioProcessorEditor()
{
}

void AudioProcessorAudioProcessorEditor::createEditor(const int channelId)
{
	_editors.emplace_back(processor, channelId);
	addAndMakeVisible(_editors.back());
	resized();
}

//==============================================================================
void AudioProcessorAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(Colour(251, 251, 251));
// 
// 	g.setColour(Colours::black);
// 	g.setFont(15.0f);
// 	g.drawFittedText("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void AudioProcessorAudioProcessorEditor::resized()
{
	auto lBounds = getLocalBounds();
	for (size_t i = 0; i < _editors.size(); ++i)
	{
		if (i > 0)
			lBounds.removeFromTop(20 + 2*20);
		_editors[i].setBounds(lBounds);
		_editors[i].resized();
	}
}
