#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

inline String operator+(const String& str, int n)
{
	return str + String(n);
}