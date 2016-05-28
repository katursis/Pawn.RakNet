#ifndef RAKNETMANAGER_NATIVES_H_
#define RAKNETMANAGER_NATIVES_H_

class Natives
{
public:

	static void Register(AMX *amx);

private:

	enum class BS_ValueType
	{
		INT8,
		INT16,
		INT32,
		UINT8,
		UINT16,
		UINT32,
		FLOAT,
		BOOL,
		STRING,

		// compressed values
		CINT8,
		CINT16,
		CINT32,
		CUINT8,
		CUINT16,
		CUINT32,
		CFLOAT,
		CBOOL,
	};

	static cell AMX_NATIVE_CALL n_BS_RPC(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_Send(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL n_BS_New(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_Delete(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL n_BS_Reset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_ResetReadPointer(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_ResetWritePointer(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_IgnoreBits(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL n_BS_SetWriteOffset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_GetWriteOffset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_SetReadOffset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_GetReadOffset(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL n_BS_GetNumberOfBitsUsed(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_GetNumberOfBytesUsed(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_GetNumberOfUnreadBits(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL n_BS_WriteValue(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL n_BS_ReadValue(AMX *amx, cell *params);

	static bool check_params(const char *native, int count, cell *params);
	static int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max);
};

#endif // RAKNETMANAGER_NATIVES_H_