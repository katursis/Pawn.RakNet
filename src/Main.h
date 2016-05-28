#ifndef RAKNETMANAGER_MAIN_H_
#define RAKNETMANAGER_MAIN_H_

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include "RakNet/BitStream.h"
#include "urmem.hpp"

#include <unordered_map>
#include <array>

#include "Addresses.h"
#include "Callbacks.h"
#include "Hooks.h"
#include "Natives.h"

using logprintf_t = void(*)(char* format, ...);

extern logprintf_t logprintf;

#endif // RAKNETMANAGER_MAIN_H_