#include "Main.h"

void Natives::Register(AMX *amx) {
    const std::vector<AMX_NATIVE_INFO> natives{
        { "BS_RPC", n_BS_RPC },
        { "BS_Send", n_BS_Send },

        { "BS_New", n_BS_New },
        { "BS_Delete", n_BS_Delete },

        { "BS_Reset", n_BS_Reset },
        { "BS_ResetReadPointer", n_BS_ResetReadPointer },
        { "BS_ResetWritePointer", n_BS_ResetWritePointer },
        { "BS_IgnoreBits", n_BS_IgnoreBits },

        { "BS_SetWriteOffset", n_BS_SetWriteOffset },
        { "BS_GetWriteOffset", n_BS_GetWriteOffset },
        { "BS_SetReadOffset", n_BS_SetReadOffset },
        { "BS_GetReadOffset", n_BS_GetReadOffset },

        { "BS_GetNumberOfBitsUsed", n_BS_GetNumberOfBitsUsed },
        { "BS_GetNumberOfBytesUsed", n_BS_GetNumberOfBytesUsed },
        { "BS_GetNumberOfUnreadBits", n_BS_GetNumberOfUnreadBits },

        { "BS_WriteValue", n_BS_WriteValue },
        { "BS_ReadValue", n_BS_ReadValue }
    };

    amx_Register(amx, natives.data(), natives.size());
}

// native BS_RPC(BitStream:bs, playerid, rpcid, PacketPriority:priority = HIGH_PRIORITY, PacketReliability:reliability = RELIABLE_ORDERED);
cell AMX_NATIVE_CALL Natives::n_BS_RPC(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 5, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        Hooks::SendRPC(
            static_cast<int>(params[2]),
            static_cast<int>(params[3]),
            bs,
            static_cast<int>(params[4]),
            static_cast<int>(params[5]));
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_Send(BitStream:bs, playerid, PacketPriority:priority = HIGH_PRIORITY, PacketReliability:reliability = RELIABLE_ORDERED);
cell AMX_NATIVE_CALL Natives::n_BS_Send(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 4, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        Hooks::SendPacket(
            static_cast<int>(params[2]),
            bs,
            static_cast<int>(params[3]),
            static_cast<int>(params[4]));
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BitStream:BS_New();
cell AMX_NATIVE_CALL Natives::n_BS_New(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 0, params)) {
        return 0;
    }

    return reinterpret_cast<cell>(new RakNet::BitStream);
}

// native BS_Delete(&BitStream:bs);
cell AMX_NATIVE_CALL Natives::n_BS_Delete(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 1, params)) {
        return 0;
    }

    cell *cptr{}; amx_GetAddr(amx, params[1], &cptr);

    if (*cptr) {
        delete reinterpret_cast<RakNet::BitStream *>(*cptr);

        *cptr = 0;
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_Reset(BitStream:bs);
cell AMX_NATIVE_CALL Natives::n_BS_Reset(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 1, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        bs->Reset();
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_ResetReadPointer(BitStream:bs);
cell AMX_NATIVE_CALL Natives::n_BS_ResetReadPointer(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 1, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        bs->ResetReadPointer();
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_ResetWritePointer(BitStream:bs);
cell AMX_NATIVE_CALL Natives::n_BS_ResetWritePointer(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 1, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        bs->ResetWritePointer();
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_IgnoreBits(BitStream:bs, number_of_bits);
cell AMX_NATIVE_CALL Natives::n_BS_IgnoreBits(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        bs->IgnoreBits(static_cast<int>(params[2]));
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_SetWriteOffset(BitStream:bs, offset);
cell AMX_NATIVE_CALL Natives::n_BS_SetWriteOffset(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        bs->SetWriteOffset(static_cast<int>(params[2]));
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_GetWriteOffset(BitStream:bs, &offset);
cell AMX_NATIVE_CALL Natives::n_BS_GetWriteOffset(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

        *cptr = static_cast<cell>(bs->GetWriteOffset());
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_SetReadOffset(BitStream:bs, offset);
cell AMX_NATIVE_CALL Natives::n_BS_SetReadOffset(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        bs->SetReadOffset(static_cast<int>(params[2]));
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_GetReadOffset(BitStream:bs, &offset);
cell AMX_NATIVE_CALL Natives::n_BS_GetReadOffset(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

        *cptr = static_cast<cell>(bs->GetReadOffset());
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
cell AMX_NATIVE_CALL Natives::n_BS_GetNumberOfBitsUsed(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

        *cptr = static_cast<cell>(bs->GetNumberOfBitsUsed());
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
cell AMX_NATIVE_CALL Natives::n_BS_GetNumberOfBytesUsed(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

        *cptr = static_cast<cell>(bs->GetNumberOfBytesUsed());
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_GetNumberOfUnreadBits(BitStream:bs, &number);
cell AMX_NATIVE_CALL Natives::n_BS_GetNumberOfUnreadBits(AMX *amx, cell *params) {
    if (!check_params(__FUNCTION__, 2, params)) {
        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

        *cptr = static_cast<cell>(bs->GetNumberOfUnreadBits());
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_WriteValue(BitStream:bs, {Float,_}:...);
cell AMX_NATIVE_CALL Natives::n_BS_WriteValue(AMX *amx, cell *params) {
    if (params[0] < (3 * sizeof(cell))) {
        logprintf("[RNM] %s: invalid number of parameters. Should be at least 3", __FUNCTION__);

        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr_type{}, *cptr_value{};

        for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
            amx_GetAddr(amx, params[i + 1], &cptr_type);
            amx_GetAddr(amx, params[i + 2], &cptr_value);

            auto type = static_cast<BS_ValueType>(*cptr_type);

            switch (type) {
                case BS_ValueType::INT8:
                    bs->Write(static_cast<char>(*cptr_value));
                    break;
                case BS_ValueType::INT16:
                    bs->Write(static_cast<short>(*cptr_value));
                    break;
                case BS_ValueType::INT32:
                    bs->Write(static_cast<int>(*cptr_value));
                    break;
                case BS_ValueType::UINT8:
                    bs->Write(static_cast<unsigned char>(*cptr_value));
                    break;
                case BS_ValueType::UINT16:
                    bs->Write(static_cast<unsigned short>(*cptr_value));
                    break;
                case BS_ValueType::UINT32:
                    bs->Write(static_cast<unsigned int>(*cptr_value));
                    break;
                case BS_ValueType::FLOAT:
                    bs->Write(amx_ctof(*cptr_value));
                    break;
                case BS_ValueType::BOOL:
                    bs->Write(!!(*cptr_value));
                    break;
                case BS_ValueType::STRING:
                {
                    int size{}; amx_StrLen(cptr_value, &size);

                    char *str = new char[size + 1]{};

                    amx_GetString(str, cptr_value, 0, size + 1);

                    bs->Write(str, size);

                    delete[] str;

                    break;
                }
                case BS_ValueType::CINT8:
                    bs->WriteCompressed(static_cast<char>(*cptr_value));
                    break;
                case BS_ValueType::CINT16:
                    bs->WriteCompressed(static_cast<short>(*cptr_value));
                    break;
                case BS_ValueType::CINT32:
                    bs->WriteCompressed(static_cast<int>(*cptr_value));
                    break;
                case BS_ValueType::CUINT8:
                    bs->WriteCompressed(static_cast<unsigned char>(*cptr_value));
                    break;
                case BS_ValueType::CUINT16:
                    bs->WriteCompressed(static_cast<unsigned short>(*cptr_value));
                    break;
                case BS_ValueType::CUINT32:
                    bs->WriteCompressed(static_cast<unsigned int>(*cptr_value));
                    break;
                case BS_ValueType::CFLOAT:
                    bs->WriteCompressed(amx_ctof(*cptr_value));
                    break;
                case BS_ValueType::CBOOL:
                    bs->WriteCompressed(!!(*cptr_value));
                    break;
                default:
                    logprintf("[RNM] %s: invalid type of value", __FUNCTION__);

                    return 0;
            }
        }
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

// native BS_ReadValue(BitStream:bs, {Float,_}:...);
cell AMX_NATIVE_CALL Natives::n_BS_ReadValue(AMX *amx, cell *params) {
    if (params[0] < 3 * sizeof(cell)) {
        logprintf("[RNM] %s: invalid number of parameters. Should be at least 3", __FUNCTION__);

        return 0;
    }

    auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

    if (bs) {
        cell *cptr_type{}, *cptr_value{};

        for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
            amx_GetAddr(amx, params[i + 1], &cptr_type);
            amx_GetAddr(amx, params[i + 2], &cptr_value);

            auto type = static_cast<BS_ValueType>(*cptr_type);

            switch (type) {
                case BS_ValueType::INT8:
                {
                    char value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::INT16:
                {
                    short value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::INT32:
                {
                    int value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::UINT8:
                {
                    unsigned char value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::UINT16:
                {
                    unsigned short value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::UINT32:
                {
                    unsigned int value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::FLOAT:
                {
                    float value{};

                    bs->Read(value);

                    *cptr_value = amx_ftoc(value);

                    break;
                }
                case BS_ValueType::BOOL:
                {
                    bool value{};

                    bs->Read(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::STRING:
                {
                    cell *cptr_size{}; amx_GetAddr(amx, params[i + 3], &cptr_size);

                    std::size_t size = *cptr_size;

                    char *str = new char[size + 1]{};

                    bs->Read(str, size);

                    set_amxstring(amx, params[i + 2], str, size);

                    delete[] str;

                    ++i;

                    break;
                }
                case BS_ValueType::CINT8:
                {
                    char value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::CINT16:
                {
                    short value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::CINT32:
                {
                    int value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::CUINT8:
                {
                    unsigned char value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::CUINT16:
                {
                    unsigned short value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::CUINT32:
                {
                    unsigned int value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                case BS_ValueType::CFLOAT:
                {
                    float value{};

                    bs->ReadCompressed(value);

                    *cptr_value = amx_ftoc(value);

                    break;
                }
                case BS_ValueType::CBOOL:
                {
                    bool value{};

                    bs->ReadCompressed(value);

                    *cptr_value = static_cast<cell>(value);

                    break;
                }
                default:
                    logprintf("[RNM] %s: invalid type of value", __FUNCTION__);

                    return 0;
            }
        }
    } else {
        logprintf("[RNM] %s: invalid BitStream handle", __FUNCTION__);

        return 0;
    }

    return 1;
}

bool Natives::check_params(const char *native, int count, cell *params) {
    if (params[0] != (count * sizeof(cell))) {
        logprintf("[RNM] %s: invalid number of parameters. Should be %d", native, count);

        return false;
    }

    return true;
}

int Natives::set_amxstring(AMX *amx, cell amx_addr, const char *source, int max) {
    cell *dest = reinterpret_cast<cell *>(
        amx->base + static_cast<int>(reinterpret_cast<AMX_HEADER *>(amx->base)->dat + amx_addr)
        );

    cell *start = dest;

    while (max-- && *source) {
        *dest++ = static_cast<cell>(*source++);
    }

    *dest = 0;

    return dest - start;
}
