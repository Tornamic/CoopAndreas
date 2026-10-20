// Server stand-in for plugin-sdk PluginBase.h. The real one pulls injector (windows.h, x86 only),
// this is just enough for the data-only plugin-sdk headers (CDamageManager.h, CPedClothesDesc.h, ...)
// to compile on any platform.
#pragma once

#include <cstdint>
#include <cstring>

#define PLUGIN_API
#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)
