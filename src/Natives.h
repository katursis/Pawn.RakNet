#ifndef NATIVES_H_
#define NATIVES_H_

namespace Natives {
    // native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
    cell AMX_NATIVE_CALL n_BS_RPC(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(5, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            const int
                player_id = static_cast<int>(params[2]),
                rpc_id = static_cast<int>(params[3]),
                priority = static_cast<int>(params[4]),
                reliability = static_cast<int>(params[5]);

            return static_cast<cell>(Functions::SendRPC(player_id, rpc_id, bs, priority, reliability));
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
    cell AMX_NATIVE_CALL n_BS_Send(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(4, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            const int
                player_id = static_cast<int>(params[2]),
                priority = static_cast<int>(params[3]),
                reliability = static_cast<int>(params[4]);

            return static_cast<cell>(Functions::SendPacket(player_id, bs, priority, reliability));
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_EmulateIncomingRPC(BitStream:bs, playerid, rpcid);
    cell AMX_NATIVE_CALL n_BS_EmulateIncomingRPC(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(3, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            const int
                player_id = static_cast<int>(params[2]),
                rpc_id = static_cast<int>(params[3]);

            const auto &handler = Hooks::original_rpc.at(rpc_id);

            if (!handler) {
                throw std::runtime_error{"invalid rpcid"};
            }

            RPCParameters RPCParams;
            RPCParams.numberOfBitsOfData = bs->GetNumberOfBitsUsed();
            RPCParams.sender = Functions::GetPlayerIDFromIndex(player_id);

            if (RPCParams.numberOfBitsOfData) {
                RPCParams.input = bs->GetData();
            }

            handler(&RPCParams);

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BitStream:BS_New();
    cell AMX_NATIVE_CALL n_BS_New(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(0, params);

            return reinterpret_cast<cell>(new RakNet::BitStream{});
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_Delete(&BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_Delete(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(1, params);

            auto &ref = Functions::GetAmxParamRef(amx, params[1]);

            const auto bs = Functions::GetAmxBitStream(ref);

            delete bs;

            ref = 0;

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_Reset(BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_Reset(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(1, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            bs->Reset();

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_ResetReadPointer(BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_ResetReadPointer(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(1, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            bs->ResetReadPointer();

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_ResetWritePointer(BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_ResetWritePointer(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(1, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            bs->ResetWritePointer();

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_IgnoreBits(BitStream:bs, number_of_bits);
    cell AMX_NATIVE_CALL n_BS_IgnoreBits(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            const int number_of_bits = static_cast<int>(params[2]);

            bs->IgnoreBits(number_of_bits);

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_SetWriteOffset(BitStream:bs, offset);
    cell AMX_NATIVE_CALL n_BS_SetWriteOffset(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            const int offset = static_cast<int>(params[2]);

            bs->SetWriteOffset(offset);

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_GetWriteOffset(BitStream:bs, &offset);
    cell AMX_NATIVE_CALL n_BS_GetWriteOffset(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            auto &ref = Functions::GetAmxParamRef(amx, params[2]);

            ref = static_cast<cell>(bs->GetWriteOffset());

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_SetReadOffset(BitStream:bs, offset);
    cell AMX_NATIVE_CALL n_BS_SetReadOffset(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            const int offset = static_cast<int>(params[2]);

            bs->SetReadOffset(offset);

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_GetReadOffset(BitStream:bs, &offset);
    cell AMX_NATIVE_CALL n_BS_GetReadOffset(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            auto &ref = Functions::GetAmxParamRef(amx, params[2]);

            ref = static_cast<cell>(bs->GetReadOffset());

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfBitsUsed(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            auto &ref = Functions::GetAmxParamRef(amx, params[2]);

            ref = static_cast<cell>(bs->GetNumberOfBitsUsed());

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfBytesUsed(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            auto &ref = Functions::GetAmxParamRef(amx, params[2]);

            ref = static_cast<cell>(bs->GetNumberOfBytesUsed());

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_GetNumberOfUnreadBits(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfUnreadBits(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            auto &ref = Functions::GetAmxParamRef(amx, params[2]);

            ref = static_cast<cell>(bs->GetNumberOfUnreadBits());

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_GetNumberOfBitsAllocated(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfBitsAllocated(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            auto &ref = Functions::GetAmxParamRef(amx, params[2]);

            ref = static_cast<cell>(bs->GetNumberOfBitsAllocated());

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_WriteValue(BitStream:bs, {Float,_}:...);
    cell AMX_NATIVE_CALL n_BS_WriteValue(AMX *amx, cell *params) {
        try {
            if (params[0] < (sizeof(cell) * 3)) {
                throw std::runtime_error{"number of parameters must be >= 3"};
            }

            const auto bs = Functions::GetAmxBitStream(params[1]);

            for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
                const auto type = Functions::GetAmxParamRef(amx, params[i + 1]);
                const auto value = Functions::GetAmxParamRef(amx, params[i + 2]);

                switch (type) {
                    case PR_STRING: {
                        auto str = Functions::GetAmxString(amx, params[i + 2]);

                        bs->Write(str.c_str(), str.size());

                        break;
                    }
                    case PR_CSTRING: {
                        auto str = Functions::GetAmxString(amx, params[i + 2]);

                        stringCompressor->EncodeString(str.c_str(), str.size() + 1, bs);

                        break;
                    }
                    case PR_INT8:
                        bs->Write(static_cast<char>(value));
                        break;
                    case PR_INT16:
                        bs->Write(static_cast<short>(value));
                        break;
                    case PR_INT32:
                        bs->Write(static_cast<int>(value));
                        break;
                    case PR_UINT8:
                        bs->Write(static_cast<unsigned char>(value));
                        break;
                    case PR_UINT16:
                        bs->Write(static_cast<unsigned short>(value));
                        break;
                    case PR_UINT32:
                        bs->Write(static_cast<unsigned int>(value));
                        break;
                    case PR_FLOAT:
                        bs->Write(amx_ctof(value));
                        break;
                    case PR_BOOL:
                        bs->Write(!!value);
                        break;
                    case PR_CINT8:
                        bs->WriteCompressed(static_cast<char>(value));
                        break;
                    case PR_CINT16:
                        bs->WriteCompressed(static_cast<short>(value));
                        break;
                    case PR_CINT32:
                        bs->WriteCompressed(static_cast<int>(value));
                        break;
                    case PR_CUINT8:
                        bs->WriteCompressed(static_cast<unsigned char>(value));
                        break;
                    case PR_CUINT16:
                        bs->WriteCompressed(static_cast<unsigned short>(value));
                        break;
                    case PR_CUINT32:
                        bs->WriteCompressed(static_cast<unsigned int>(value));
                        break;
                    case PR_CFLOAT:
                        bs->WriteCompressed(amx_ctof(value));
                        break;
                    case PR_CBOOL:
                        bs->WriteCompressed(!!value);
                        break;
                    case PR_BITS: {
                        const auto number_of_bits = Functions::GetAmxParamRef(amx, params[i + 3]);

                        if (number_of_bits <= 0 || number_of_bits > (sizeof(cell) * 8)) {
                            throw std::runtime_error{"invalid number of bits"};
                        }

                        bs->WriteBits(reinterpret_cast<const unsigned char *>(&value), number_of_bits, true);

                        ++i;

                        break;
                    }
                    default: {
                        throw std::runtime_error{"invalid type of value"};
                    }
                }
            }

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    template<typename T>
    struct Value {
        static inline cell Read(RakNet::BitStream *bs) {
            T value{};

            bs->Read<T>(value);

            return static_cast<cell>(value);
        }

        static inline cell ReadCompressed(RakNet::BitStream *bs) {
            T value{};

            bs->ReadCompressed<T>(value);

            return static_cast<cell>(value);
        }
    };

    template<>
    struct Value<float> {
        static inline cell Read(RakNet::BitStream *bs) {
            float value{};

            bs->Read<float>(value);

            return amx_ftoc(value);
        }

        static inline cell ReadCompressed(RakNet::BitStream *bs) {
            float value{};

            bs->ReadCompressed<float>(value);

            return amx_ftoc(value);
        }
    };

    // native BS_ReadValue(BitStream:bs, {Float,_}:...);
    cell AMX_NATIVE_CALL n_BS_ReadValue(AMX *amx, cell *params) {
        try {
            if (params[0] < sizeof(cell) * 3) {
                throw std::runtime_error{"number of parameters must be >= 3"};
            }

            const auto bs = Functions::GetAmxBitStream(params[1]);

            for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
                const auto type = Functions::GetAmxParamRef(amx, params[i + 1]);
                auto &value = Functions::GetAmxParamRef(amx, params[i + 2]);

                switch (type) {
                    case PR_STRING: {
                        const auto size = Functions::GetAmxParamRef(amx, params[i + 3]);

                        std::unique_ptr<char[]> str{new char[size + 1]{}};

                        bs->Read(str.get(), size);

                        Functions::SetAmxString(amx, params[i + 2], str.get(), size);

                        ++i;

                        break;
                    }
                    case PR_CSTRING: {
                        const auto size = Functions::GetAmxParamRef(amx, params[i + 3]);

                        std::unique_ptr<char[]> str{new char[size + 1]{}};

                        stringCompressor->DecodeString(str.get(), size, bs);

                        Functions::SetAmxString(amx, params[i + 2], str.get(), size);

                        ++i;

                        break;
                    }
                    case PR_INT8:
                        value = Value<char>::Read(bs);
                        break;
                    case PR_INT16:
                        value = Value<short>::Read(bs);
                        break;
                    case PR_INT32:
                        value = Value<int>::Read(bs);
                        break;
                    case PR_UINT8:
                        value = Value<unsigned char>::Read(bs);
                        break;
                    case PR_UINT16:
                        value = Value<unsigned short>::Read(bs);
                        break;
                    case PR_UINT32:
                        value = Value<unsigned int>::Read(bs);
                        break;
                    case PR_FLOAT:
                        value = Value<float>::Read(bs);
                        break;
                    case PR_BOOL:
                        value = Value<bool>::Read(bs);
                        break;
                    case PR_CINT8:
                        value = Value<char>::ReadCompressed(bs);
                        break;
                    case PR_CINT16:
                        value = Value<short>::ReadCompressed(bs);
                        break;
                    case PR_CINT32:
                        value = Value<int>::ReadCompressed(bs);
                        break;
                    case PR_CUINT8:
                        value = Value<unsigned char>::ReadCompressed(bs);
                        break;
                    case PR_CUINT16:
                        value = Value<unsigned short>::ReadCompressed(bs);
                        break;
                    case PR_CUINT32:
                        value = Value<unsigned int>::ReadCompressed(bs);
                        break;
                    case PR_CFLOAT:
                        value = Value<float>::ReadCompressed(bs);
                        break;
                    case PR_CBOOL:
                        value = Value<bool>::ReadCompressed(bs);
                        break;
                    case PR_BITS: {
                        const auto number_of_bits = Functions::GetAmxParamRef(amx, params[i + 3]);

                        if (number_of_bits <= 0 || number_of_bits > (sizeof(cell) * 8)) {
                            throw std::runtime_error{"invalid number of bits"};
                        }

                        bs->ReadBits(reinterpret_cast<unsigned char *>(&value), number_of_bits, true);

                        ++i;

                        break;
                    }
                    default: {
                        throw std::runtime_error{"invalid type of value"};
                    }
                }
            }

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native PR_RegHandler(id, const publicname[], PR_HandlerType:type);
    cell AMX_NATIVE_CALL n_PR_RegHandler(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(3, params);

            Scripts::RegisterHandler(
                amx,
                static_cast<int>(params[1]),
                Functions::GetAmxString(amx, params[2]),
                static_cast<PR_HandlerType>(params[3])
            );

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    void Register(AMX *amx) {
        const std::vector<AMX_NATIVE_INFO> natives{
            {"BS_RPC", n_BS_RPC},
            {"BS_Send", n_BS_Send},

            {"BS_EmulateIncomingRPC", n_BS_EmulateIncomingRPC},

            {"BS_New", n_BS_New},
            {"BS_Delete", n_BS_Delete},

            {"BS_Reset", n_BS_Reset},
            {"BS_ResetReadPointer", n_BS_ResetReadPointer},
            {"BS_ResetWritePointer", n_BS_ResetWritePointer},
            {"BS_IgnoreBits", n_BS_IgnoreBits},

            {"BS_SetWriteOffset", n_BS_SetWriteOffset},
            {"BS_GetWriteOffset", n_BS_GetWriteOffset},
            {"BS_SetReadOffset", n_BS_SetReadOffset},
            {"BS_GetReadOffset", n_BS_GetReadOffset},

            {"BS_GetNumberOfBitsUsed", n_BS_GetNumberOfBitsUsed},
            {"BS_GetNumberOfBytesUsed", n_BS_GetNumberOfBytesUsed},
            {"BS_GetNumberOfUnreadBits", n_BS_GetNumberOfUnreadBits},
            {"BS_GetNumberOfBitsAllocated", n_BS_GetNumberOfBitsAllocated},

            {"BS_WriteValue", n_BS_WriteValue},
            {"BS_ReadValue", n_BS_ReadValue},

            {"PR_RegHandler", n_PR_RegHandler}
        };

        amx_Register(amx, natives.data(), natives.size());
    }
};

#endif // NATIVES_H_
