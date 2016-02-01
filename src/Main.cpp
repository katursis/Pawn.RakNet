#include "Main.h"

logprintf_t logprintf;

extern void *pAMXFunctions;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{	
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

	if (Hooks::Init())
	{	
		logprintf("");
		logprintf(">> RakNet Manager plugin v1.0.0 by urShadow loaded");
		logprintf("");		
	}

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	logprintf("");
	logprintf(">> RakNet Manager plugin unloaded");
	logprintf("");	
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	Callbacks::OnAmxLoad(amx);
	
	return Natives::Register(amx);	
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{	
	Callbacks::OnAmxUnload(amx);

	return AMX_ERR_NONE;
}