/*
* The MIT License (MIT)
*
* Copyright (c) 2017 urShadow
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#include "RakNet/BitStream.h"
#include "urmem/urmem.hpp"
#include "crtp_singleton/crtp_singleton.hpp"
#include "cpptoml/include/cpptoml.h"

#include <list>
#include <array>
#include <string>
#include <regex>

#include "Pawn.RakNet.inc"

#include "Logger.h"
#include "Settings.h"
#include "Utils.h"
#include "Scripts.h"
#include "Addresses.h"
#include "Functions.h"
#include "Hooks.h"
#include "Natives.h"

extern void *pAMXFunctions;

namespace Plugin {
    bool Load(void **ppData) {
        pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

        Logger::instance()->Init(ppData[PLUGIN_DATA_LOGPRINTF]);

        Settings::Read(Settings::kConfigFile);

        if (!Hooks::Init(ppData[PLUGIN_DATA_LOGPRINTF])) {
            Logger::instance()->Write("[%s] %s: RakServer address not found", Settings::kPluginName, __FUNCTION__);

            return false;
        }

        Logger::instance()->Write("%s plugin v%s by urShadow loaded", Settings::kPluginName, Settings::kPluginVersion);

        return true;
    }

    void Unload(void) {
        Logger::instance()->Write("%s plugin v%s by urShadow unloaded", Settings::kPluginName, Settings::kPluginVersion);
    }

    void AmxLoad(AMX *amx) {
        cell include_version{}, is_gamemode{};

        const bool exists = Utils::get_public_var(amx, Settings::kPublicVarNameVersion, include_version) &&
            Utils::get_public_var(amx, Settings::kPublicVarNameIsGamemode, is_gamemode);

        if (exists) {
            if (include_version != PAWNRAKNET_INCLUDE_VERSION) {
                Logger::instance()->Write("[%s] %s: Please update Pawn.RakNet.inc file to the latest version", Settings::kPluginName, __FUNCTION__);

                return;
            }

            Natives::Register(amx);

            Scripts::Load(amx, is_gamemode == 1);
        }
    }

    void AmxUnload(AMX *amx) {
        Scripts::Unload(amx);
    }
};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
    return Plugin::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    Plugin::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL AmxLoad(AMX *amx) {
    Plugin::AmxLoad(amx);
}

PLUGIN_EXPORT void PLUGIN_CALL AmxUnload(AMX *amx) {
    Plugin::AmxUnload(amx);
}
