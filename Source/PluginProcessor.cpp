#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <assert.h>
#include <string>

const double bufferLengthMs = 10.0;

//==============================================================================
AudioProcessorWithDelays::AudioProcessorWithDelays()
{
}

AudioProcessorWithDelays::~AudioProcessorWithDelays()
{
}

void AudioProcessorWithDelays::setUiUpdateRequiredCallback(const std::function<void()>& callback)
{
	assert(callback);
	_uiUpdateRequiredCallback = callback;
}


//==============================================================================
const String AudioProcessorWithDelays::getName() const
{
	return JucePlugin_Name;
}

bool AudioProcessorWithDelays::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool AudioProcessorWithDelays::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool AudioProcessorWithDelays::silenceInProducesSilenceOut() const
{
	return false;
}

double AudioProcessorWithDelays::getTailLengthSeconds() const
{
	return bufferLengthMs * 1e-3;
}

int AudioProcessorWithDelays::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int AudioProcessorWithDelays::getCurrentProgram()
{
	return 0;
}

void AudioProcessorWithDelays::setCurrentProgram(int /*index*/)
{
}

const String AudioProcessorWithDelays::getProgramName(int /*index*/)
{
	return "Default program";
}

void AudioProcessorWithDelays::changeProgramName(int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void AudioProcessorWithDelays::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
	// prepareToPlay() may be called more than once with the same parameters.
	// The catch is that the second call is not followed by setStateInformation() so we cannot easily recreate the state.
	// So we want to keep the previously restored (via setStateInformation) state , if possible.
	const size_t numChannels = (size_t)getTotalNumOutputChannels();
	if (sampleRate == _currentSampleRate && _processors.size() == numChannels)
		return;

	_currentSampleRate = sampleRate;

	// We should only arrive here if it's the first prepareToPlay() call, or if the sample rate has changed. The latter is not uncommon.
	if (_processors.size() != numChannels)
	{
		assert(_processors.empty());
		for (int i = 0; i < getTotalNumOutputChannels(); ++i)
			_processors.emplace_back(i);
	}

	for (size_t i = 0; i < numChannels; ++i)
	{
		ChannelProcessor& p = _processors[i];
		p._delayBuffer.resize((size_t)(sampleRate * bufferLengthMs * 1e-3), 0.0f); // 10 ms buffer

		setDelay(delay(i), i); // This will update the ChannelProcessor properly in case this prepareToPlay call occurred after the settings had been loaded
	}
}

void AudioProcessorWithDelays::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

void AudioProcessorWithDelays::processBlock(AudioSampleBuffer& buffer, MidiBuffer& /*midiMessages*/)
{
	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// I've added this to avoid people getting screaming feedback
	// when they first compile the plugin, but obviously you don't need to
	// this code if your algorithm already fills all the output channels.
	for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	std::lock_guard<std::mutex> guard(m_mutex);

	for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
	{
		ChannelProcessor& processor = processorByChannelId(channel);
		if (!processor._enabled)
			continue;

		float* channelData = buffer.getWritePointer(channel);
		const int numSamples = buffer.getNumSamples();

		// Delay implementation
		assert(processor._delayBuffer.size() > (size_t)numSamples);
		assert(processor._delayBuffer.size() - numSamples > processor._delayNumSamples);

		memmove(processor._delayBuffer.data(), processor._delayBuffer.data() + numSamples, (processor._delayBuffer.size() - numSamples) * sizeof(float));
		float* currentBlockStartInBuffer = processor._delayBuffer.data() + processor._delayBuffer.size() - numSamples;
		memcpy(currentBlockStartInBuffer, channelData, numSamples * sizeof(float));
		memcpy(channelData, currentBlockStartInBuffer - processor._delayNumSamples, numSamples * sizeof(float));
	}
}

//==============================================================================
bool AudioProcessorWithDelays::hasEditor() const
{
	return true;
}

AudioProcessorEditor* AudioProcessorWithDelays::createEditor()
{
	return new AudioProcessorAudioProcessorEditor(*this);
}

//==============================================================================
void AudioProcessorWithDelays::getStateInformation(MemoryBlock& destData)
{
	XmlElement root("Root");
	for (int i = 0; i < getTotalNumOutputChannels(); ++i)
	{
		XmlElement *el = root.createNewChildElement("Channel");
		el->setAttribute("id", i);

		const double delayMs = delay(i);
		el->setAttribute("delay", delayMs);

		const bool enabled = isEnabled(i);
		el->setAttribute("enabled", enabled ? 1 : 0);
	}
	
	copyXmlToBinary(root, destData);
}

void AudioProcessorWithDelays::setStateInformation(const void* data, int sizeInBytes)
{
	XmlElement* pRoot = getXmlFromBinary(data, sizeInBytes);
	assert(pRoot);
	if (!pRoot)
		return;

	forEachXmlChildElement(*pRoot, pChild)
	{
		if (pChild && pChild->hasTagName("Channel"))
		{
			const int id = pChild->getIntAttribute("id");
			const double delayMs = pChild->getDoubleAttribute("delay");
			setDelay(delayMs, id);

			const int enabled = pChild->getIntAttribute("enabled");
			setEnabled(enabled == 0 ? false : true, id);
		}
	}

	delete pRoot;
}

void AudioProcessorWithDelays::setDelay(double delay, int channelId)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	ChannelProcessor& p = processorByChannelId(channelId);
	p._delayMs = (float)delay;
	const double sampleRate = getSampleRate();
	p._delayNumSamples = (uint32_t)(p._delayMs * sampleRate * 1e-3 + 0.5);

	_uiUpdateRequiredCallback();
}

double AudioProcessorWithDelays::delay(int channelId) const
{
	std::lock_guard<std::mutex> guard(m_mutex);
	const ChannelProcessor& p = processorByChannelId(channelId);

	return p._delayMs;
}

void AudioProcessorWithDelays::setEnabled(bool enabled, int channelId)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	ChannelProcessor& p = processorByChannelId(channelId);
	p._enabled = enabled;

	_uiUpdateRequiredCallback();
}

bool AudioProcessorWithDelays::isEnabled(int channelId) const
{
	std::lock_guard<std::mutex> guard(m_mutex);
	const ChannelProcessor& p = processorByChannelId(channelId);
	return p._enabled;
}

ChannelProcessor& AudioProcessorWithDelays::processorByChannelId(int id)
{
	auto it = std::find_if(_processors.begin(), _processors.end(), [id](const ChannelProcessor& p) {
		return p._channelId == id;
	});

	if (it != _processors.end())
		return *it;
	else
		throw std::runtime_error((std::string("Processor not found for channel") + std::to_string(id)).c_str());
}

const ChannelProcessor& AudioProcessorWithDelays::processorByChannelId(int id) const
{
	auto it = std::find_if(_processors.begin(), _processors.end(), [id](const ChannelProcessor& p) {
		return p._channelId == id;
	});

	if (it != _processors.end())
		return *it;
	else
		throw std::runtime_error((std::string("Processor not found for channel") + std::to_string(id)).c_str());
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new AudioProcessorWithDelays();
}
