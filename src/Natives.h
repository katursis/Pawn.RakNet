#ifndef RAKNETMANAGER_NATIVES_H_
#define RAKNETMANAGER_NATIVES_H_

class Natives
{
public:

	static int Register(AMX *amx);

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

	static cell AMX_NATIVE_CALL SendRPC(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL SendPacket(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL BitStream__New(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__Delete(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL BitStream__Reset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__ResetReadPointer(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__ResetWritePointer(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__IgnoreBits(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL BitStream__SetWriteOffset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__GetWriteOffset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__SetReadOffset(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__GetReadOffset(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL BitStream__GetNumberOfBitsUsed(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__GetNumberOfBytesUsed(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__GetNumberOfUnreadBits(AMX *amx, cell *params);

	static cell AMX_NATIVE_CALL BitStream__WriteValue(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL BitStream__ReadValue(AMX *amx, cell *params);	
	
	inline static bool check_params(const char *native, const int count, const cell *params);
	inline static int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max);
};

#endif // RAKNETMANAGER_NATIVES_H_
