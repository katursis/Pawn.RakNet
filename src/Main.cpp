#include "Main.h"

logprintf_t logprintf;

extern void *pAMXFunctions;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
  logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

  if (Hooks::Init()) {
    logprintf("[RNM] Loading...");

    return true;
  }

  logprintf("[RNM] RakServer address not found. Stopping...");

  return false;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
  logprintf("[RNM] Unloaded");
}

PLUGIN_EXPORT void PLUGIN_CALL AmxLoad(AMX *amx) {
  Callbacks::OnAmxLoad(amx);

  Natives::Register(amx);
}

PLUGIN_EXPORT void PLUGIN_CALL AmxUnload(AMX *amx) {
  Callbacks::OnAmxUnload(amx);
}
