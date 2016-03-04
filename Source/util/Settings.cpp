#include "Settings.h"

Settings::Settings()
{
	PropertiesFile::Options options;
	options.applicationName = ProjectInfo::projectName;
	options.folderName = ProjectInfo::projectName;
	options.doNotSave = false;
	options.osxLibrarySubFolder = "Application Support";
	_settings.setStorageParameters(options);

	_settingsFile = _settings.getCommonSettings(true);
}

Settings& Settings::instance()
{
	static Settings inst;
	return inst;
}

Settings::~Settings()
{
	_settingsFile = nullptr;
	_settings.saveIfNeeded();
}

var Settings::value(const String& key, var defaultValue /*= var()*/) const
{
	std::lock_guard<std::mutex> guard(_mutex);
	return _settingsFile ? _settingsFile->getValue(key, defaultValue) : defaultValue;
}

void Settings::setValue(const String& key, var value)
{
	std::lock_guard<std::mutex> guard(_mutex);
	if (_settingsFile)
		_settingsFile->setValue(key, value);
}
