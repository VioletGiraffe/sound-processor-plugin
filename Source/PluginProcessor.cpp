/*
  ==============================================================================

  This file was auto-generated by the Introjucer!

  It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
  */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioProcessorAudioProcessor::AudioProcessorAudioProcessor()
{
}

AudioProcessorAudioProcessor::~AudioProcessorAudioProcessor()
{
}

//==============================================================================
const String AudioProcessorAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool AudioProcessorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool AudioProcessorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool AudioProcessorAudioProcessor::silenceInProducesSilenceOut() const
{
	return false;
}

double AudioProcessorAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int AudioProcessorAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int AudioProcessorAudioProcessor::getCurrentProgram()
{
	return 0;
}

void AudioProcessorAudioProcessor::setCurrentProgram(int index)
{
}

const String AudioProcessorAudioProcessor::getProgramName(int index)
{
	return String();
}

void AudioProcessorAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void AudioProcessorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
}

void AudioProcessorAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

void AudioProcessorAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// I've added this to avoid people getting screaming feedback
	// when they first compile the plugin, but obviously you don't need to
	// this code if your algorithm already fills all the output channels.
	for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
	{
		float* channelData = buffer.getWritePointer(channel);

		// ..do something to the data...
	}
}

//==============================================================================
bool AudioProcessorAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AudioProcessorAudioProcessor::createEditor()
{
	return new AudioProcessorAudioProcessorEditor(*this);
}

//==============================================================================
void AudioProcessorAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void AudioProcessorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new AudioProcessorAudioProcessor();
}
