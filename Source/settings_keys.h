#pragma once

#include "util/util.h"

#define SETTINGS_KEY_DELAY_ON(n) (String("delay_on") + (n))
#define SETTINGS_DEFAULT_DELAY_ON false

#define SETTINGS_KEY_FRONT_CHANNEL_DELAY_VALUE(n) (String("front_channel_delay_value") + (n))
#define SETTINGS_DEFAULT_FRONT_CHANNEL_DELAY_VALUE 0.0