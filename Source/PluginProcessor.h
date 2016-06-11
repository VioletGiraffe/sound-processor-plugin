#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include <functional>
#include <mutex>
#include <vector>

struct ChannelProcessor
{
	inline ChannelProcessor(int channelId) : _channelId(channelId) {}
	ChannelProcessor& operator=(const ChannelProcessor& other) = delete;

	const int _channelId;
	float _delayMs = 0.0f;
	uint32_t _delayNumSamples = 0;
	bool _enabled = false;

	std::vector<float> _delayBuffer;
};


//==============================================================================
/**
*/
class AudioProcessorWithDelays : public AudioProcessor
{
public:
	//==============================================================================
	AudioProcessorWithDelays();
	~AudioProcessorWithDelays();

	void setUiUpdateRequiredCallback(const std::function<void ()>& callback);

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool silenceInProducesSilenceOut() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	//==============================================================================
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	void setDelay(double delay, int channelId);
	double delay(int channelId) const;
	void setEnabled(bool enabled, int channelId);
	bool isEnabled(int channelId) const;

private:
	ChannelProcessor& processorByChannelId(int id);
	const ChannelProcessor& processorByChannelId(int id) const;

private:
	mutable std::mutex m_mutex;

	std::vector<ChannelProcessor> _processors;
	double _currentSampleRate = 0.0;

	std::function<void ()> _uiUpdateRequiredCallback = [](){};

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorWithDelays)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
