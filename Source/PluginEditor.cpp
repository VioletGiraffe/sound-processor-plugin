#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "util/util.h"

const double SpeedOfSound = 340.0; // m/s at 15C, normal pressure

const auto bgColor = Colour{48, 48, 48}, textColor = Colour{200, 200, 200};

inline double msToCm(double ms)
{
	static const double cmPerMs = SpeedOfSound * 100.0 /* cm in 1 m */ / 1000.0 /* ms in 1 s*/;
	return ms * cmPerMs;
}

inline double cmToMs(double cm)
{
	static const double msPerOneCm = 1.0 / msToCm(1.0);
	return cm * msPerOneCm;
}

DelayEditor::DelayEditor(AudioProcessorWithDelays& processor, int channelId) : _processor(processor), _channelId(channelId)
{
	_onOffSwitch.setButtonText("On");
	_onOffSwitch.addListener(this);
	_onOffSwitch.setToggleState(_processor.isEnabled(channelId), juce::sendNotificationSync);
	_onOffSwitch.setColour(ToggleButton::textColourId, textColor);
	_onOffSwitch.setColour(ToggleButton::tickColourId, Colour(239, 51, 64));
	addAndMakeVisible(_onOffSwitch);

	_delaySlider.setSliderStyle(Slider::LinearBar);
	_delaySlider.setRange(0.0, 200, 0.1);
	_delaySlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	_delaySlider.setPopupDisplayEnabled(true, this);
	_delaySlider.setTextValueSuffix(" cm");
	_delaySlider.setValue(msToCm(_processor.delay(channelId)));
	_delaySlider.addListener(this);
	addAndMakeVisible(_delaySlider);

	_editor.setMultiLine(false);
	_editor.addListener(this);
	_editor.setText(String(_delaySlider.getValue()));
	_editor.setColour(TextEditor::backgroundColourId, bgColor);
	_editor.setColour(TextEditor::textColourId, textColor);
	_editor.setColour(TextEditor::highlightColourId, _editor.findColour(TextEditor::focusedOutlineColourId));
	_editor.setColour(TextEditor::highlightColourId, _editor.findColour(TextEditor::focusedOutlineColourId));
	_editor.setColour(TextEditor::highlightedTextColourId, Colour(255, 255, 255));
	addAndMakeVisible(_editor);
}

void DelayEditor::sliderValueChanged(Slider* slider)
{
	const double currentDelayMs = _processor.delay(_channelId);
	const double newDelayMs = cmToMs(_delaySlider.getValue());
	if (slider == &_delaySlider && fabs(newDelayMs - currentDelayMs) >= cmToMs(0.1))
	{
		_editor.setText(String(_delaySlider.getValue()));
		_processor.setDelay(newDelayMs, _channelId);
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
	setSize(400, 200);

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
	g.fillAll(bgColor);
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
