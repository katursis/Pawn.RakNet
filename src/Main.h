#ifndef RAKNETMANAGER_MAIN_H_
#define RAKNETMANAGER_MAIN_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <string.h>
#include <sys/mman.h>
#endif

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include "RakNet/BitStream.h"
#include "urmem.hpp"

#include <set>
#include <array>
#include <initializer_list>
#include <string>
#include <iostream>

#include "Addresses.h"
#include "Callbacks.h"
#include "Hooks.h"
#include "Natives.h"

using logprintf_t = void(*)(char* format, ...);

extern logprintf_t logprintf;

#endif // RAKNETMANAGER_MAIN_H_
