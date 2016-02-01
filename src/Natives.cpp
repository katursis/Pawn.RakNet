#include "Main.h"

int Natives::Register(AMX *amx)
{
	static std::vector<AMX_NATIVE_INFO> vec_native = {

		{ "SendRPC", Natives::SendRPC },
		{ "SendPacket", Natives::SendPacket },

		{ "BS_New", Natives::BitStream__New },
		{ "BS_Delete", Natives::BitStream__Delete },

		{ "BS_Reset", Natives::BitStream__Reset },
		{ "BS_ResetReadPointer", Natives::BitStream__ResetReadPointer },
		{ "BS_ResetWritePointer", Natives::BitStream__ResetWritePointer },
		{ "BS_IgnoreBits", Natives::BitStream__IgnoreBits },

		{ "BS_SetWriteOffset", Natives::BitStream__SetWriteOffset },
		{ "BS_GetWriteOffset", Natives::BitStream__GetWriteOffset },
		{ "BS_SetReadOffset", Natives::BitStream__SetReadOffset },
		{ "BS_GetReadOffset", Natives::BitStream__GetReadOffset },

		{ "BS_GetNumberOfBitsUsed", Natives::BitStream__GetNumberOfBitsUsed },
		{ "BS_GetNumberOfBytesUsed", Natives::BitStream__GetNumberOfBytesUsed },
		{ "BS_GetNumberOfUnreadBits", Natives::BitStream__GetNumberOfUnreadBits },

		{ "BS_WriteValue", Natives::BitStream__WriteValue },
		{ "BS_ReadValue", Natives::BitStream__ReadValue }
	};
		
	return amx_Register(amx, vec_native.data(), vec_native.size());
}

// native SendRPC(playerid, rpcid, BitStream:bs);
cell AMX_NATIVE_CALL Natives::SendRPC(AMX *amx, cell *params)
{
	if (!check_params("SendRPC", 3, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[3]);

	if (bitstream)
	{
		Hooks::SendRPC(params[1], params[2], bitstream);
	}
	else
	{
		logprintf(">> RakNet Manager (SendRPC): BitStream handle error");

		return 0;
	}

	return 1;
}

// native SendPacket(playerid, BitStream:bs);
cell AMX_NATIVE_CALL Natives::SendPacket(AMX *amx, cell *params)
{
	if (!check_params("SendPacket", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[2]);

	if (bitstream)
	{
		Hooks::SendPacket(params[1], bitstream);
	}
	else
	{
		logprintf(">> RakNet Manager (SendPacket): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BitStream:BS_New();
cell AMX_NATIVE_CALL Natives::BitStream__New(AMX *amx, cell *params)
{
	if (!check_params("BitStream:BS_New", 0, params))
		return 0;

	return reinterpret_cast<cell>(new RakNet::BitStream);
}

// native BS_Delete(&BitStream:bs);
cell AMX_NATIVE_CALL Natives::BitStream__Delete(AMX *amx, cell *params)
{
	if (!check_params("BS_Delete", 1, params))
		return 0;

	cell *cptr{}; amx_GetAddr(amx, params[1], &cptr);

	if (*cptr)
	{
		delete reinterpret_cast<RakNet::BitStream *>(*cptr);
		*cptr = 0;
	}
	else
	{
		logprintf(">> RakNet Manager (BS_Delete): BitStream handle error");

		return 0;
	}	

	return 1;
}

// native BS_Reset(BitStream:bs);
cell AMX_NATIVE_CALL Natives::BitStream__Reset(AMX *amx, cell *params)
{
	if (!check_params("BS_Reset", 1, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		bitstream->Reset();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_Reset): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_ResetReadPointer(BitStream:bs);
cell AMX_NATIVE_CALL Natives::BitStream__ResetReadPointer(AMX *amx, cell *params)
{
	if (!check_params("BS_ResetReadPointer", 1, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		bitstream->ResetReadPointer();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_ResetReadPointer): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_ResetWritePointer(BitStream:bs);
cell AMX_NATIVE_CALL Natives::BitStream__ResetWritePointer(AMX *amx, cell *params)
{
	if (!check_params("BS_ResetWritePointer", 1, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		bitstream->ResetWritePointer();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_ResetWritePointer): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_IgnoreBits(BitStream:bs, bitcount);
cell AMX_NATIVE_CALL Natives::BitStream__IgnoreBits(AMX *amx, cell *params)
{
	if (!check_params("BS_IgnoreBits", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		bitstream->IgnoreBits(static_cast<int>(params[2]));
	}
	else
	{
		logprintf(">> RakNet Manager (BS_IgnoreBits): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_SetWriteOffset(BitStream:bs, offset);
cell AMX_NATIVE_CALL Natives::BitStream__SetWriteOffset(AMX *amx, cell *params)
{
	if (!check_params("BS_SetWriteOffset", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		bitstream->SetWriteOffset(static_cast<int>(params[2]));
	}
	else
	{
		logprintf(">> RakNet Manager (BS_SetWriteOffset): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_GetWriteOffset(BitStream:bs, &offset);
cell AMX_NATIVE_CALL Natives::BitStream__GetWriteOffset(AMX *amx, cell *params)
{
	if (!check_params("BS_GetWriteOffset", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

		*cptr = bitstream->GetWriteOffset();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_GetWriteOffset): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_SetReadOffset(BitStream:bs, offset);
cell AMX_NATIVE_CALL Natives::BitStream__SetReadOffset(AMX *amx, cell *params)
{
	if (!check_params("BS_SetReadOffset", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		bitstream->SetReadOffset(static_cast<int>(params[2]));
	}
	else
	{
		logprintf(">> RakNet Manager (BS_SetReadOffset): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_GetReadOffset(BitStream:bs, &offset);
cell AMX_NATIVE_CALL Natives::BitStream__GetReadOffset(AMX *amx, cell *params)
{
	if (!check_params("BS_GetReadOffset", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

		*cptr = bitstream->GetReadOffset();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_GetReadOffset): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
cell AMX_NATIVE_CALL Natives::BitStream__GetNumberOfBitsUsed(AMX *amx, cell *params)
{
	if (!check_params("BS_GetNumberOfBitsUsed", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

		*cptr = bitstream->GetNumberOfBitsUsed();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_GetNumberOfBitsUsed): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
cell AMX_NATIVE_CALL Natives::BitStream__GetNumberOfBytesUsed(AMX *amx, cell *params)
{
	if (!check_params("BS_GetNumberOfBytesUsed", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

		*cptr = bitstream->GetNumberOfBytesUsed();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_GetNumberOfBytesUsed): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_GetNumberOfUnreadBits(BitStream:bs, &number);
cell AMX_NATIVE_CALL Natives::BitStream__GetNumberOfUnreadBits(AMX *amx, cell *params)
{
	if (!check_params("BS_GetNumberOfUnreadBits", 2, params))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

		*cptr = bitstream->GetNumberOfUnreadBits();
	}
	else
	{
		logprintf(">> RakNet Manager (BS_GetNumberOfUnreadBits): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_WriteValue(BitStream:bs, {Float,_}:...);
cell AMX_NATIVE_CALL Natives::BitStream__WriteValue(AMX *amx, cell *params)
{
	if (params[0] < 3 * sizeof(cell))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr_type{}, *cptr_value{};
		
		for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i+=2)
		{
			amx_GetAddr(amx, params[i + 1], &cptr_type);
			amx_GetAddr(amx, params[i + 2], &cptr_value);

			auto type = static_cast<BS_ValueType>(*cptr_type);

			switch (type)
			{
				case BS_ValueType::INT8:
					bitstream->Write(static_cast<char>(*cptr_value));
					break;
				case BS_ValueType::INT16:
					bitstream->Write(static_cast<short>(*cptr_value));
					break;
				case BS_ValueType::INT32:
					bitstream->Write(static_cast<int>(*cptr_value));
					break;
				case BS_ValueType::UINT8:
					bitstream->Write(static_cast<unsigned char>(*cptr_value));
					break;
				case BS_ValueType::UINT16:
					bitstream->Write(static_cast<unsigned short>(*cptr_value));
					break;
				case BS_ValueType::UINT32:
					bitstream->Write(static_cast<unsigned int>(*cptr_value));
					break;
				case BS_ValueType::FLOAT:
					bitstream->Write(amx_ctof(*cptr_value));
					break;
				case BS_ValueType::BOOL:
					bitstream->Write(!!(*cptr_value));
					break;
				case BS_ValueType::STRING:
				{
					int size{}; amx_StrLen(cptr_value, &size);					

					char *str = new char[size + 1]{};

					amx_GetString(str, cptr_value, 0, size + 1);

					bitstream->Write(str, size);

					delete[] str;				

					break;
				}					
				case BS_ValueType::CINT8:
					bitstream->WriteCompressed(static_cast<char>(*cptr_value));
					break;
				case BS_ValueType::CINT16:
					bitstream->WriteCompressed(static_cast<short>(*cptr_value));
					break;
				case BS_ValueType::CINT32:
					bitstream->WriteCompressed(static_cast<int>(*cptr_value));
					break;
				case BS_ValueType::CUINT8:
					bitstream->WriteCompressed(static_cast<unsigned char>(*cptr_value));
					break;
				case BS_ValueType::CUINT16:
					bitstream->WriteCompressed(static_cast<unsigned short>(*cptr_value));
					break;
				case BS_ValueType::CUINT32:
					bitstream->WriteCompressed(static_cast<unsigned int>(*cptr_value));
					break;
				case BS_ValueType::CFLOAT:
					bitstream->WriteCompressed(amx_ctof(*cptr_value));
					break;
				case BS_ValueType::CBOOL:
					bitstream->WriteCompressed(!!(*cptr_value));
					break;
				default: 
					logprintf(">> RakNet Manager (BS_WriteValue): Error type of value");
					return 0;
			}
		}
	}
	else
	{
		logprintf(">> RakNet Manager (BS_WriteValue): BitStream handle error");

		return 0;
	}

	return 1;
}

// native BS_ReadValue(BitStream:bs, {Float,_}:...);
cell AMX_NATIVE_CALL Natives::BitStream__ReadValue(AMX *amx, cell *params)
{
	if (params[0] < 3 * sizeof(cell))
		return 0;

	auto bitstream = reinterpret_cast<RakNet::BitStream *>(params[1]);

	if (bitstream)
	{
		cell *cptr_type{}, *cptr_value{};

		for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2)
		{
			amx_GetAddr(amx, params[i + 1], &cptr_type);
			amx_GetAddr(amx, params[i + 2], &cptr_value);

			auto type = static_cast<BS_ValueType>(*cptr_type);

			switch (type)
			{
				case BS_ValueType::INT8: 
				{
					char value{}; 

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::INT16:
				{
					short value{};

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::INT32:
				{
					int value{};

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::UINT8:
				{
					unsigned char value{};

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::UINT16:
				{
					unsigned short value{};

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::UINT32:
				{
					unsigned int value{};

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::FLOAT:
				{
					float value{};

					bitstream->Read(value);

					*cptr_value = amx_ftoc(value);

					break;
				}
				case BS_ValueType::BOOL:
				{
					bool value{};

					bitstream->Read(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::STRING:
				{
					cell *cptr_size{}; amx_GetAddr(amx, params[i + 3], &cptr_size);
					
					std::size_t size = *cptr_size;

					char *str = new char[size + 1]{};

					bitstream->Read(str, size);									

					set_amxstring(amx, params[i + 2], str, size);

					delete[] str;

					++i;

					break;
				}
				case BS_ValueType::CINT8:
				{
					char value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::CINT16:
				{
					short value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::CINT32:
				{
					int value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::CUINT8:
				{
					unsigned char value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::CUINT16:
				{
					unsigned short value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::CUINT32:
				{
					unsigned int value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				case BS_ValueType::CFLOAT:
				{
					float value{};

					bitstream->ReadCompressed(value);

					*cptr_value = amx_ftoc(value);

					break;
				}
				case BS_ValueType::CBOOL:
				{
					bool value{};

					bitstream->ReadCompressed(value);

					*cptr_value = static_cast<cell>(value);

					break;
				}
				default:
					logprintf(">> RakNet Manager (BS_ReadValue): Error type of value");
					return 0;
			}
		}
	}
	else
	{
		logprintf(">> RakNet Manager (BS_ReadValue): BitStream handle error");

		return 0;
	}

	return 1;
}

inline bool Natives::check_params(const char *native, const int count, const cell *params)
{
	if (params[0] != (count * sizeof(cell)))
		return logprintf(">> RakNet Manager (%s): Bad parameter count (Count is %d, should be %d)",
			native, params[0] / sizeof(cell), count), false;

	return true;
}

inline int Natives::set_amxstring(AMX *amx, cell amx_addr, const char *source, int max)
{
	cell* dest = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
	
	cell* start = dest;

	while (max--&&*source)
		*dest++ = (cell)*source++;

	*dest = 0;

	return dest - start;
}