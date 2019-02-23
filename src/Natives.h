/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2019 urShadow
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

#ifndef NATIVES_H_
#define NATIVES_H_

namespace Natives {
    // native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
    cell AMX_NATIVE_CALL n_BS_Send(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(4, params);

            return Functions::RakServer::Send(
                Functions::GetAmxBitStream(params[1]),
                params[3],
                params[4],
                '\0',
                Functions::RakServer::GetPlayerIDFromIndex(params[2]),
                params[2] == -1
            );
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
    cell AMX_NATIVE_CALL n_BS_RPC(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(5, params);

            return Functions::RakServer::RPC(
                reinterpret_cast<RPCIndex *>(&params[3]),
                Functions::GetAmxBitStream(params[1]),
                params[4],
                params[5],
                '\0',
                Functions::RakServer::GetPlayerIDFromIndex(params[2]),
                params[2] == -1,
                false
            );
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native BS_EmulateIncomingPacket(BitStream:bs, playerid);
    cell AMX_NATIVE_CALL n_BS_EmulateIncomingPacket(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(2, params);

            const auto bs = Functions::GetAmxBitStream(params[1]);

            Hooks::emulating_packets.push(Functions::RakServer::NewPacket(params[2], *bs));

            return 1;
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

            const auto &handler = Hooks::original_rpc.at(params[3]);

            if (!handler) {
                throw std::runtime_error{"invalid rpcid"};
            }

            RPCParameters RPCParams;
            RPCParams.numberOfBitsOfData = bs->GetNumberOfBitsUsed();
            RPCParams.sender = Functions::RakServer::GetPlayerIDFromIndex(params[2]);

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

            return Scripts::GetScript(amx).NewBitStream();
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

            Scripts::GetScript(amx).DeleteBitStream(ref);

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

            Functions::GetAmxBitStream(params[1])->Reset();

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

            Functions::GetAmxBitStream(params[1])->ResetReadPointer();

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

            Functions::GetAmxBitStream(params[1])->ResetWritePointer();

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

            Functions::GetAmxBitStream(params[1])->IgnoreBits(params[2]);

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

            Functions::GetAmxBitStream(params[1])->SetWriteOffset(params[2]);

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

            Functions::GetAmxParamRef(amx, params[2]) = Functions::GetAmxBitStream(params[1])->GetWriteOffset();

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

            Functions::GetAmxBitStream(params[1])->SetReadOffset(params[2]);

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

            Functions::GetAmxParamRef(amx, params[2]) = Functions::GetAmxBitStream(params[1])->GetReadOffset();

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

            Functions::GetAmxParamRef(amx, params[2]) = Functions::GetAmxBitStream(params[1])->GetNumberOfBitsUsed();

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

            Functions::GetAmxParamRef(amx, params[2]) = Functions::GetAmxBitStream(params[1])->GetNumberOfBytesUsed();

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

            Functions::GetAmxParamRef(amx, params[2]) = Functions::GetAmxBitStream(params[1])->GetNumberOfUnreadBits();

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

            Functions::GetAmxParamRef(amx, params[2]) = Functions::GetAmxBitStream(params[1])->GetNumberOfBitsAllocated();

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
                const auto &value = Functions::GetAmxParamRef(amx, params[i + 2]);

                switch (type) {
                    case PR_STRING:
                    case PR_CSTRING: {
                        auto str = Functions::GetAmxString(amx, params[i + 2]);

                        if (type == PR_STRING) {
                            bs->Write(str.c_str(), str.size());
                        } else {
                            stringCompressor->EncodeString(str.c_str(), str.size() + 1, bs);
                        }

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
                    case PR_FLOAT3:
                    case PR_FLOAT4: {
                        const std::size_t arr_size = (type == PR_FLOAT3 ? 3 : 4);
                        const auto arr = &value;

                        for (std::size_t index{}; index < arr_size; ++index) {
                            bs->Write(amx_ctof(arr[index]));
                        }

                        break;
                    }
                    case PR_VECTOR:
                    case PR_NORM_QUAT: {
                        const auto arr = reinterpret_cast<const float *>(&value);

                        if (type == PR_VECTOR) {
                            bs->WriteVector(arr[0], arr[1], arr[2]);
                        } else {
                            bs->WriteNormQuat(arr[0], arr[1], arr[2], arr[3]);
                        }

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
                    case PR_STRING:
                    case PR_CSTRING: {
                        const auto size = Functions::GetAmxParamRef(amx, params[i + 3]);

                        std::unique_ptr<char[]> str{new char[size + 1]{}};

                        if (type == PR_STRING) {
                            bs->Read(str.get(), size);
                        } else {
                            stringCompressor->DecodeString(str.get(), size, bs);
                        }

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
                    case PR_FLOAT3:
                    case PR_FLOAT4: {
                        const std::size_t arr_size = (type == PR_FLOAT3 ? 3 : 4);
                        auto arr = &value;

                        for (std::size_t index{}; index < arr_size; ++index) {
                            arr[index] = Value<float>::Read(bs);
                        }

                        break;
                    }
                    case PR_VECTOR:
                    case PR_NORM_QUAT: {
                        auto arr = reinterpret_cast<float *>(&value);

                        if (type == PR_VECTOR) {
                            bs->ReadVector(arr[0], arr[1], arr[2]);
                        } else {
                            bs->ReadNormQuat(arr[0], arr[1], arr[2], arr[3]);
                        }

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

    // native PR_RegHandler(id, const publicname[], PR_EventType:type);
    cell AMX_NATIVE_CALL n_PR_RegHandler(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(3, params);

            Scripts::GetScript(amx).RegisterHandler(
                params[1],
                Functions::GetAmxString(amx, params[2]),
                static_cast<PR_EventType>(params[3])
            );

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    // native PR_Init(bool:is_gamemode);
    cell AMX_NATIVE_CALL n_PR_Init(AMX *amx, cell *params) {
        try {
            Functions::AssertParams(1, params);

            Scripts::Load(amx, params[1] == 1);

            return 1;
        } catch (const std::exception &e) {
            Logger::instance()->Write("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
        }

        return 0;
    }

    void Register(AMX *amx) {
        const std::vector<AMX_NATIVE_INFO> natives{
            {"BS_Send", n_BS_Send},
            {"BS_RPC", n_BS_RPC},

            {"BS_EmulateIncomingPacket", n_BS_EmulateIncomingPacket},
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

            {"PR_RegHandler", n_PR_RegHandler},
            {"PR_Init", n_PR_Init}
        };

        amx_Register(amx, natives.data(), natives.size());
    }
};

#endif // NATIVES_H_
