#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "util/Settings.h"
#include "settings_keys.h"
#include "util/util.h"

DelayEditor::DelayEditor(AudioProcessorWithDelays& processor, int channelId) : _processor(processor), _channelId(channelId)
{
	_onOffSwitch.setComponentID("onOffSwitch");
	_onOffSwitch.setButtonText("On");
	_onOffSwitch.addListener(this);
	_onOffSwitch.setToggleState(Settings::instance().value(SETTINGS_KEY_DELAY_ON(channelId), SETTINGS_DEFAULT_DELAY_ON), juce::sendNotificationSync);
	addAndMakeVisible(_onOffSwitch);
	_onOffSwitch.setBounds("parent.left + 10, parent.top + 20, left + 50, top + 20");

	_delaySlider.setSliderStyle(Slider::LinearBar);
	_delaySlider.setRange(0.0, 15.0, 0.01);
	_delaySlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	_delaySlider.setPopupDisplayEnabled(true, this);
	_delaySlider.setTextValueSuffix(" ms");
	_delaySlider.setValue(Settings::instance().value(SETTINGS_KEY_FRONT_CHANNEL_DELAY_VALUE(channelId), SETTINGS_DEFAULT_FRONT_CHANNEL_DELAY_VALUE));
	_delaySlider.addListener(this);
	_delaySlider.setComponentID("delaySlider");

	const int textEditWidth = 50;

	addAndMakeVisible(_delaySlider);
	_delaySlider.setBounds(String("onOffSwitch.right + 10, parent.top + 20, parent.right - 40 - 10 - ") + String(textEditWidth) + ", top + 20");

	_editor.setMultiLine(false);
	_editor.setComponentID("delayEditor");
	_editor.addListener(this);
	_editor.setText(String(_delaySlider.getValue()));
	addAndMakeVisible(_editor);
	_editor.setBounds(String("parent.right - 20 - ") + String(textEditWidth) + ", parent.top + 20, parent.right - 20, top + 20");

	const int verticalMargin = 20;
	setSize(getWidth(), _delaySlider.getHeight() + 2*verticalMargin);
}

void DelayEditor::sliderValueChanged(Slider* slider)
{
	if (slider == &_delaySlider)
	{
		Settings::instance().setValue(SETTINGS_KEY_FRONT_CHANNEL_DELAY_VALUE(_channelId), _delaySlider.getValue());
		_editor.setText(String(_delaySlider.getValue()));
		_processor.onDelayChanged(_delaySlider.getValue(), _channelId);
	}
}

void DelayEditor::textEditorReturnKeyPressed(TextEditor & editor)
{
	if (editor.getComponentID() == "delayEditor")
		_delaySlider.setValue(editor.getText().getDoubleValue());
}

void DelayEditor::buttonClicked(Button* button)
{
	if (button->getComponentID() == "onOffSwitch")
	{
		Settings::instance().setValue(SETTINGS_KEY_DELAY_ON(_channelId), button->getToggleState());
		_processor.setEnabled(button->getToggleState(), _channelId);
	}
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
	const int previousEditorBottom = _editors.empty() ? 0 : _editors.back().getBottom();
	const String previousEditorID = _editors.empty() ? String() : _editors.back().getComponentID();

	_editors.emplace_back(processor, channelId);
	DelayEditor& newEditor = _editors.back();
	newEditor.setComponentID(String("Editor_") + channelId);
	addAndMakeVisible(newEditor);

//	newEditor.setSize(getWidth(), newEditor.getHeight());

// 	auto rect = newEditor.getBoundsInParent();
	String expression = String("parent.left, ") + (previousEditorID.isEmpty() ? "parent.top" : (previousEditorID + ".bottom")) + ", parent.right, top + " + newEditor.getHeight();
 	newEditor.setBounds(expression);
// //	newEditor.setTopLeftPosition(0, previousEditorBottom);
// 	rect = newEditor.getBoundsInParent();
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
