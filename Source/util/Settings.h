#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <mutex>

class Settings
{
public:
	static Settings& instance();

	~Settings();

	var value(const String& key, var defaultValue = var()) const;
	void setValue(const String& key, var value);

private:
	Settings();

private:
	ApplicationProperties _settings;
	PropertiesFile * _settingsFile = nullptr;

	mutable std::mutex _mutex;
};