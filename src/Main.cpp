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

#include <list>
#include <array>
#include <string>

#include "Pawn.RakNet.inc"

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall
#else
#define THISCALL
#endif

using logprintf_t = void(*)(const char *format, ...);

logprintf_t logprintf;

extern void *pAMXFunctions;

namespace Settings {
    constexpr char
        *kPluginName = "Pawn.RakNet",
        *kPluginVersion = "1.0",
        *kPublicVarNameVersion = "_pawnraknet_version",
        *kPublicVarNameIsGamemode = "_pawnraknet_is_gamemode",
#ifdef _WIN32
        *kPattern = "\x6A\xFF\x68\x5B\xA4\x4A\x00\x64\xA1\x00\x00" \
        "\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51" \
        "\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83" \
        "\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x24\xFF" \
        "\x00\x00\x00\x00\x74\x16",
        *kMask = "xxx????xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx";
#else
        *kPattern =
        "\x55\x89\xE5\x83\xEC\x18\xC7\x04\x24\xFF\xFF" \
        "\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF" \
        "\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF" \
        "\xFF\x89\xF0\x8B\x5D\xFF\x8B\x75\xFF\x89\xEC" \
        "\x5D\xC3",
        *kMask = "xxxxxxxxx????xx?xx?x????xxxxxx????xxxx?xx?xxxx";
#endif
}

namespace Utils {
    inline bool check_params(const char *native, int count, cell *params) {
        if (params[0] != (count * sizeof(cell))) {
            logprintf("[%s] %s: invalid number of parameters. Should be %d", Settings::kPluginName, native, count);

            return false;
        }

        return true;
    }

    inline int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max) {
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

    inline bool get_public_var(AMX *amx, const char *name, cell &out) {
        cell addr{},
            *phys_addr{};

        if ((amx_FindPubVar(amx, name, &addr) == AMX_ERR_NONE) &&
            (amx_GetAddr(amx, addr, &phys_addr) == AMX_ERR_NONE)) {
            out = *phys_addr;

            return true;
        }

        return false;
    }
}

namespace Addresses {
    urmem::address_t
        RAKSERVER,
        FUNC_RAKSERVER__SEND,
        FUNC_RAKSERVER__RPC,
        FUNC_RAKSERVER__RECEIVE,
        FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
        FUNC_RAKSERVER__DEALLOCATE_PACKET,
        FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID,
        FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX;

    bool Init(urmem::address_t rakserver) {
        if (const auto vmt = urmem::pointer(RAKSERVER = rakserver).field<urmem::address_t *>(0)) {
#ifdef _WIN32
            FUNC_RAKSERVER__SEND = vmt[7];
            FUNC_RAKSERVER__RPC = vmt[32];
            FUNC_RAKSERVER__RECEIVE = vmt[10];
            FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL = vmt[29];
            FUNC_RAKSERVER__DEALLOCATE_PACKET = vmt[12];
            FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID = vmt[57];
            FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX = vmt[58];
#else
            FUNC_RAKSERVER__SEND = vmt[9];
            FUNC_RAKSERVER__RPC = vmt[35];
            FUNC_RAKSERVER__RECEIVE = vmt[11];
            FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL = vmt[30];
            FUNC_RAKSERVER__DEALLOCATE_PACKET = vmt[13];
            FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID = vmt[58];
            FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX = vmt[59];
#endif

            logprintf("[%s] Addresses found", Settings::kPluginName);

            return true;
        }

        logprintf("[%s] Addresses not found", Settings::kPluginName);

        return false;
    }
};

namespace Callbacks {
    class Public {
    public:
        explicit Public(const std::string &name, AMX *amx) : _name{ name }, _amx{ amx } {
            _exists = (amx_FindPublic(amx, name.c_str(), &_index) == AMX_ERR_NONE);
        }

        inline bool exists() const {
            return _exists;
        }

        inline int get_index() const {
            return _index;
        }

    private:
        std::string _name;
        int _index;
        bool _exists;
        AMX *_amx;
    };

    class Script {
    public:
        enum Publics {
            ON_INCOMING_PACKET,
            ON_INCOMING_RPC,
            ON_OUTCOMING_PACKET,
            ON_OUTCOMIMG_RPC,
            NUMBER_OF_PUBLICS
        };

        explicit Script(AMX *amx) : _amx(amx) {
            const auto make_public = [amx](const std::string &name) {
                return std::make_unique<Public>(name, amx);
            };

            _publics[ON_INCOMING_PACKET] = make_public("OnIncomingPacket");
            _publics[ON_INCOMING_RPC] = make_public("OnIncomingRPC");
            _publics[ON_OUTCOMING_PACKET] = make_public("OnOutcomingPacket");
            _publics[ON_OUTCOMIMG_RPC] = make_public("OnOutcomingRPC");
        }

        // forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        inline bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[ON_INCOMING_PACKET];

            if (!pub->exists()) {
                return true;
            }

            if (bs) {
                bs->ResetReadPointer();
            }

            amx_Push(_amx, reinterpret_cast<cell>(bs));
            amx_Push(_amx, static_cast<cell>(packet_id));
            amx_Push(_amx, static_cast<cell>(player_id));

            cell retval{};

            amx_Exec(_amx, &retval, pub->get_index());

            return retval == 1;
        }

        // forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        inline bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[ON_INCOMING_RPC];

            if (!pub->exists()) {
                return true;
            }

            if (bs) {
                bs->ResetReadPointer();
            }

            amx_Push(_amx, reinterpret_cast<cell>(bs));
            amx_Push(_amx, static_cast<cell>(rpc_id));
            amx_Push(_amx, static_cast<cell>(player_id));

            cell retval{};

            amx_Exec(_amx, &retval, pub->get_index());

            return retval == 1;
        }

        // forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        inline bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[ON_OUTCOMING_PACKET];

            if (!pub->exists()) {
                return true;
            }

            if (bs) {
                bs->ResetReadPointer();
            }

            amx_Push(_amx, reinterpret_cast<cell>(bs));
            amx_Push(_amx, static_cast<cell>(packet_id));
            amx_Push(_amx, static_cast<cell>(player_id));

            cell retval{};

            amx_Exec(_amx, &retval, pub->get_index());

            return retval == 1;
        }

        // forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
        bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[ON_OUTCOMIMG_RPC];

            if (!pub->exists()) {
                return true;
            }

            if (bs) {
                bs->ResetReadPointer();
            }

            amx_Push(_amx, reinterpret_cast<cell>(bs));
            amx_Push(_amx, static_cast<cell>(rpc_id));
            amx_Push(_amx, static_cast<cell>(player_id));

            cell retval{};

            amx_Exec(_amx, &retval, pub->get_index());

            return retval == 1;
        }

        inline AMX *get_amx() const {
            return _amx;
        }

    private:
        AMX *_amx;
        std::array<std::unique_ptr<Public>, NUMBER_OF_PUBLICS> _publics{};
    };

    class Scripts {
    public:
        void load(AMX *amx, bool is_gamemode) {
            auto script = std::make_unique<Script>(amx);

            if (is_gamemode) {
                _scripts.push_back(std::move(script));
            } else {
                _scripts.push_front(std::move(script));
            }
        }

        void unload(AMX *amx) {
            const auto script = std::find_if(_scripts.begin(), _scripts.end(), [amx](const std::unique_ptr<Script> &script) {
                return script->get_amx() == amx;
            });

            if (script != _scripts.end()) {
                _scripts.erase(script);
            }
        }

        bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
                return script->OnIncomingPacket(player_id, packet_id, bs);
            });
        }

        bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
                return script->OnIncomingRPC(player_id, rpc_id, bs);
            });
        }

        bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
                return script->OnOutcomingPacket(player_id, packet_id, bs);
            });
        }

        bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
                return script->OnOutcomingRPC(player_id, rpc_id, bs);
            });
        }

    private:
        std::list<std::unique_ptr<Script>> _scripts;
    };

    Scripts scripts;

    bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return scripts.OnIncomingPacket(player_id, packet_id, bs);
    }

    bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return scripts.OnIncomingRPC(player_id, rpc_id, bs);
    }

    bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return scripts.OnOutcomingPacket(player_id, packet_id, bs);
    }

    bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return scripts.OnOutcomingRPC(player_id, rpc_id, bs);
    }

    void OnAmxLoad(AMX *amx, bool is_gamemode) {
        scripts.load(amx, is_gamemode);
    }

    void OnAmxUnload(AMX *amx) {
        scripts.unload(amx);
    }
};

namespace Hooks {
    std::shared_ptr<urmem::hook>
        hook_get_rak_server_interface,
        hook_rakserver__send,
        hook_rakserver__rpc,
        hook_rakserver__receive,
        hook_rakserver__register_as_remote_procedure_call;

    std::array<RPCFunction, MAX_RPC_MAP_SIZE>
        original_rpc,
        custom_rpc;

    int GetIndexFromPlayerID(const PlayerID &id) {
        return urmem::call_function<urmem::calling_convention::thiscall, int>(
            Addresses::FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID, Addresses::RAKSERVER, id
            );
    }

    PlayerID GetPlayerIDFromIndex(int index) {
        return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
            Addresses::FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX, Addresses::RAKSERVER, index
            );
    }

    void DeallocatePacket(Packet *p) {
        urmem::call_function<urmem::calling_convention::thiscall>(
            Addresses::FUNC_RAKSERVER__DEALLOCATE_PACKET, Addresses::RAKSERVER, p
            );
    }

    bool SendPacket(int player_id, RakNet::BitStream *bs, int priority, int reliability) {
        bool enabled = hook_rakserver__send->is_enabled();

        if (enabled) {
            hook_rakserver__send->disable();
        }

        auto result = urmem::call_function<urmem::calling_convention::thiscall, bool>(
            Addresses::FUNC_RAKSERVER__SEND,
            Addresses::RAKSERVER,
            bs,
            priority,
            reliability,
            '\0',
            GetPlayerIDFromIndex(player_id),
            player_id == -1
            );

        if (enabled) {
            hook_rakserver__send->enable();
        }

        return result;
    }

    bool SendRPC(int player_id, int rpc_id, RakNet::BitStream *bs, int priority, int reliability) {
        bool enabled = hook_rakserver__rpc->is_enabled();

        if (enabled) {
            hook_rakserver__rpc->disable();
        }

        static RPCIndex id{};

        id = static_cast<RPCIndex>(rpc_id);

        auto result = urmem::call_function<urmem::calling_convention::thiscall, bool>(
            Addresses::FUNC_RAKSERVER__RPC,
            Addresses::RAKSERVER,
            &id,
            bs,
            priority,
            reliability,
            '\0',
            GetPlayerIDFromIndex(player_id),
            player_id == -1,
            false
            );

        if (enabled) {
            hook_rakserver__rpc->enable();
        }

        return result;
    }

    class InternalHooks {
    public:
        static bool THISCALL RakServer__Send(void *_this, RakNet::BitStream *bitStream,
                                             int priority, int reliability,
                                             char orderingChannel, PlayerID playerId,
                                             bool broadcast) {
            urmem::hook::raii scope(*hook_rakserver__send);

            if (bitStream) {
                const auto read_offset = bitStream->GetReadOffset(), write_offset = bitStream->GetWriteOffset();

                if (!Callbacks::OnOutcomingPacket(GetIndexFromPlayerID(playerId), bitStream->GetData()[0], bitStream)) {
                    return false;
                }

                bitStream->SetReadOffset(read_offset), bitStream->SetWriteOffset(write_offset);
            }

            return urmem::call_function<urmem::calling_convention::thiscall, bool>(
                Addresses::FUNC_RAKSERVER__SEND, _this, bitStream, priority, reliability,
                orderingChannel, playerId, broadcast
                );
        }

        static bool THISCALL RakServer__RPC(void *_this, RPCIndex *uniqueID,
                                            RakNet::BitStream *bitStream, int priority,
                                            int reliability, char orderingChannel,
                                            PlayerID playerId, bool broadcast,
                                            bool shiftTimestamp) {
            urmem::hook::raii scope(*hook_rakserver__rpc);

            if (uniqueID && bitStream) {
                const auto
                    read_offset = bitStream->GetReadOffset(),
                    write_offset = bitStream->GetWriteOffset();

                if (!Callbacks::OnOutcomingRPC(GetIndexFromPlayerID(playerId), *uniqueID, bitStream)) {
                    return false;
                }

                bitStream->SetReadOffset(read_offset);

                bitStream->SetWriteOffset(write_offset);
            }

            return urmem::call_function<urmem::calling_convention::thiscall, bool>(
                Addresses::FUNC_RAKSERVER__RPC, _this, uniqueID, bitStream, priority, reliability,
                orderingChannel, playerId, broadcast, shiftTimestamp
                );
        }

        static  Packet * THISCALL RakServer__Receive(void *_this) {
            urmem::hook::raii scope(*hook_rakserver__receive);

            Packet *packet = urmem::call_function<urmem::calling_convention::thiscall, Packet *>(
                Addresses::FUNC_RAKSERVER__RECEIVE, _this
                );

            if (packet && packet->data) {
                RakNet::BitStream bitstream(packet->data, packet->length, false);

                if (!Callbacks::OnIncomingPacket(packet->playerIndex, packet->data[0], &bitstream)) {
                    DeallocatePacket(packet);

                    return nullptr;
                }
            }

            return packet;
        }

        static void * THISCALL RakServer__RegisterAsRemoteProcedureCall(void *_this, RPCIndex *uniqueID, RPCFunction functionPointer) {
            urmem::hook::raii scope(*hook_rakserver__register_as_remote_procedure_call);

            if (uniqueID && functionPointer) {
                original_rpc[*uniqueID] = functionPointer;

                return urmem::call_function<urmem::calling_convention::thiscall, void *>(
                    Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                    _this, uniqueID, custom_rpc[*uniqueID]
                    );
            }

            return urmem::call_function<urmem::calling_convention::thiscall, void *>(
                Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                _this, uniqueID, functionPointer
                );
        }

        static void ReceiveRPC(int rpc_id, RPCParameters *p) {
            if (p) {
                int player_id = GetIndexFromPlayerID(p->sender);

                std::shared_ptr<RakNet::BitStream> bs;

                if (p->input) {
                    bs = std::make_shared<RakNet::BitStream>(p->input, BITS_TO_BYTES(p->numberOfBitsOfData), false);
                }

                if (!Callbacks::OnIncomingRPC(player_id, rpc_id, bs.get())) {
                    return;
                }

                try {
                    original_rpc.at(rpc_id)(p);
                } catch (const std::exception &e) {
                    logprintf("[%s] %s: %s", Settings::kPluginName, __FUNCTION__, e.what());
                }
            }
        }

        static void * GetRakServerInterface(void) {
            urmem::hook::raii scope(*hook_get_rak_server_interface);

            auto rakserver = urmem::call_function<urmem::calling_convention::cdeclcall, void *>(
                hook_get_rak_server_interface->get_original_addr()
                );

            if (Addresses::Init(reinterpret_cast<urmem::address_t>(rakserver))) {
                hook_rakserver__send = std::make_shared<urmem::hook>(
                    Addresses::FUNC_RAKSERVER__SEND,
                    urmem::get_func_addr(&RakServer__Send)
                    );

                hook_rakserver__rpc = std::make_shared<urmem::hook>(
                    Addresses::FUNC_RAKSERVER__RPC,
                    urmem::get_func_addr(&RakServer__RPC)
                    );

                hook_rakserver__receive = std::make_shared<urmem::hook>(
                    Addresses::FUNC_RAKSERVER__RECEIVE,
                    urmem::get_func_addr(&RakServer__Receive)
                    );

                hook_rakserver__register_as_remote_procedure_call = std::make_shared<urmem::hook>(
                    Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                    urmem::get_func_addr(&RakServer__RegisterAsRemoteProcedureCall)
                    );

                original_rpc.fill(nullptr);
                custom_rpc.fill(nullptr);

                RPCHandle::Create();

                logprintf("[%s] Initialized", Settings::kPluginName);
            }

            return rakserver;
        }

        struct RPCHandle {
            static void Create(void) {
                Generator<0>::Run();
            }

            template<size_t ID>
            struct Generator {
                static void Handle(RPCParameters *p) {
                    ReceiveRPC(ID, p);
                }

                static void Run(void) {
                    custom_rpc[ID] = reinterpret_cast<RPCFunction>(&Handle);

                    Generator<ID + 1>::Run();
                }
            };
        };
    };

    template<>
    struct InternalHooks::RPCHandle::Generator<MAX_RPC_MAP_SIZE> {
        static void Run(void) {}
    };

    bool Init(void) {
        urmem::sig_scanner scanner;

        if (scanner.init(urmem::get_func_addr(logprintf))) {
            urmem::address_t addr{};

            if (scanner.find(Settings::kPattern, Settings::kMask, addr)) {
                hook_get_rak_server_interface = std::make_shared<urmem::hook>(
                    addr,
                    urmem::get_func_addr(&InternalHooks::GetRakServerInterface)
                    );

                return true;
            }
        }

        return false;
    }
};

namespace Natives {
    // native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
    cell AMX_NATIVE_CALL n_BS_RPC(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 5, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            Hooks::SendRPC(
                static_cast<int>(params[2]),
                static_cast<int>(params[3]),
                bs,
                static_cast<int>(params[4]),
                static_cast<int>(params[5])
            );
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
    cell AMX_NATIVE_CALL n_BS_Send(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 4, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            Hooks::SendPacket(
                static_cast<int>(params[2]),
                bs,
                static_cast<int>(params[3]),
                static_cast<int>(params[4])
            );
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BitStream:BS_New();
    cell AMX_NATIVE_CALL n_BS_New(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 0, params)) {
            return 0;
        }

        return reinterpret_cast<cell>(new RakNet::BitStream);
    }

    // native BS_Delete(&BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_Delete(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 1, params)) {
            return 0;
        }

        cell *cptr{}; amx_GetAddr(amx, params[1], &cptr);

        if (*cptr) {
            delete reinterpret_cast<RakNet::BitStream *>(*cptr);

            *cptr = 0;
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_Reset(BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_Reset(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 1, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            bs->Reset();
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_ResetReadPointer(BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_ResetReadPointer(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 1, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            bs->ResetReadPointer();
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_ResetWritePointer(BitStream:bs);
    cell AMX_NATIVE_CALL n_BS_ResetWritePointer(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 1, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            bs->ResetWritePointer();
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_IgnoreBits(BitStream:bs, number_of_bits);
    cell AMX_NATIVE_CALL n_BS_IgnoreBits(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            bs->IgnoreBits(static_cast<int>(params[2]));
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_SetWriteOffset(BitStream:bs, offset);
    cell AMX_NATIVE_CALL n_BS_SetWriteOffset(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            bs->SetWriteOffset(static_cast<int>(params[2]));
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_GetWriteOffset(BitStream:bs, &offset);
    cell AMX_NATIVE_CALL n_BS_GetWriteOffset(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

            *cptr = static_cast<cell>(bs->GetWriteOffset());
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_SetReadOffset(BitStream:bs, offset);
    cell AMX_NATIVE_CALL n_BS_SetReadOffset(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            bs->SetReadOffset(static_cast<int>(params[2]));
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_GetReadOffset(BitStream:bs, &offset);
    cell AMX_NATIVE_CALL n_BS_GetReadOffset(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

            *cptr = static_cast<cell>(bs->GetReadOffset());
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfBitsUsed(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

            *cptr = static_cast<cell>(bs->GetNumberOfBitsUsed());
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfBytesUsed(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

            *cptr = static_cast<cell>(bs->GetNumberOfBytesUsed());
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_GetNumberOfUnreadBits(BitStream:bs, &number);
    cell AMX_NATIVE_CALL n_BS_GetNumberOfUnreadBits(AMX *amx, cell *params) {
        if (!Utils::check_params(__FUNCTION__, 2, params)) {
            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr{}; amx_GetAddr(amx, params[2], &cptr);

            *cptr = static_cast<cell>(bs->GetNumberOfUnreadBits());
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_WriteValue(BitStream:bs, {Float,_}:...);
    cell AMX_NATIVE_CALL n_BS_WriteValue(AMX *amx, cell *params) {
        if (params[0] < (3 * sizeof(cell))) {
            logprintf("[%s] %s: invalid number of parameters. Should be at least 3", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr_type{}, *cptr_value{};

            for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
                amx_GetAddr(amx, params[i + 1], &cptr_type);
                amx_GetAddr(amx, params[i + 2], &cptr_value);

                auto type = static_cast<PR_ValueType>(*cptr_type);

                switch (type) {
                    case PR_INT8:
                        bs->Write(static_cast<char>(*cptr_value));
                        break;
                    case PR_INT16:
                        bs->Write(static_cast<short>(*cptr_value));
                        break;
                    case PR_INT32:
                        bs->Write(static_cast<int>(*cptr_value));
                        break;
                    case PR_UINT8:
                        bs->Write(static_cast<unsigned char>(*cptr_value));
                        break;
                    case PR_UINT16:
                        bs->Write(static_cast<unsigned short>(*cptr_value));
                        break;
                    case PR_UINT32:
                        bs->Write(static_cast<unsigned int>(*cptr_value));
                        break;
                    case PR_FLOAT:
                        bs->Write(amx_ctof(*cptr_value));
                        break;
                    case PR_BOOL:
                        bs->Write(!!(*cptr_value));
                        break;
                    case PR_STRING:
                    {
                        int size{}; amx_StrLen(cptr_value, &size);

                        char *str = new char[size + 1]{};

                        amx_GetString(str, cptr_value, 0, size + 1);

                        bs->Write(str, size);

                        delete[] str;

                        break;
                    }
                    case PR_CINT8:
                        bs->WriteCompressed(static_cast<char>(*cptr_value));
                        break;
                    case PR_CINT16:
                        bs->WriteCompressed(static_cast<short>(*cptr_value));
                        break;
                    case PR_CINT32:
                        bs->WriteCompressed(static_cast<int>(*cptr_value));
                        break;
                    case PR_CUINT8:
                        bs->WriteCompressed(static_cast<unsigned char>(*cptr_value));
                        break;
                    case PR_CUINT16:
                        bs->WriteCompressed(static_cast<unsigned short>(*cptr_value));
                        break;
                    case PR_CUINT32:
                        bs->WriteCompressed(static_cast<unsigned int>(*cptr_value));
                        break;
                    case PR_CFLOAT:
                        bs->WriteCompressed(amx_ctof(*cptr_value));
                        break;
                    case PR_CBOOL:
                        bs->WriteCompressed(!!(*cptr_value));
                        break;
                    default:
                        logprintf("[%s] %s: invalid type of value", Settings::kPluginName, __FUNCTION__);

                        return 0;
                }
            }
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    // native BS_ReadValue(BitStream:bs, {Float,_}:...);
    cell AMX_NATIVE_CALL n_BS_ReadValue(AMX *amx, cell *params) {
        if (params[0] < 3 * sizeof(cell)) {
            logprintf("[%s] %s: invalid number of parameters. Should be at least 3", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        auto bs = reinterpret_cast<RakNet::BitStream *>(params[1]);

        if (bs) {
            cell *cptr_type{}, *cptr_value{};

            for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
                amx_GetAddr(amx, params[i + 1], &cptr_type);
                amx_GetAddr(amx, params[i + 2], &cptr_value);

                auto type = static_cast<PR_ValueType>(*cptr_type);

                switch (type) {
                    case PR_INT8:
                    {
                        char value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_INT16:
                    {
                        short value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_INT32:
                    {
                        int value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_UINT8:
                    {
                        unsigned char value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_UINT16:
                    {
                        unsigned short value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_UINT32:
                    {
                        unsigned int value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_FLOAT:
                    {
                        float value{};

                        bs->Read(value);

                        *cptr_value = amx_ftoc(value);

                        break;
                    }
                    case PR_BOOL:
                    {
                        bool value{};

                        bs->Read(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_STRING:
                    {
                        cell *cptr_size{}; amx_GetAddr(amx, params[i + 3], &cptr_size);

                        std::size_t size = *cptr_size;

                        char *str = new char[size + 1]{};

                        bs->Read(str, size);

                        Utils::set_amxstring(amx, params[i + 2], str, size);

                        delete[] str;

                        ++i;

                        break;
                    }
                    case PR_CINT8:
                    {
                        char value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_CINT16:
                    {
                        short value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_CINT32:
                    {
                        int value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_CUINT8:
                    {
                        unsigned char value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_CUINT16:
                    {
                        unsigned short value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_CUINT32:
                    {
                        unsigned int value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    case PR_CFLOAT:
                    {
                        float value{};

                        bs->ReadCompressed(value);

                        *cptr_value = amx_ftoc(value);

                        break;
                    }
                    case PR_CBOOL:
                    {
                        bool value{};

                        bs->ReadCompressed(value);

                        *cptr_value = static_cast<cell>(value);

                        break;
                    }
                    default:
                        logprintf("[%s] %s: invalid type of value", Settings::kPluginName, __FUNCTION__);

                        return 0;
                }
            }
        } else {
            logprintf("[%s] %s: invalid BitStream handle", Settings::kPluginName, __FUNCTION__);

            return 0;
        }

        return 1;
    }

    void Register(AMX *amx) {
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
};

namespace Plugin {
    bool Load(void) {
        if (Hooks::Init()) {
            logprintf("%s plugin v%s by urShadow loaded", Settings::kPluginName, Settings::kPluginVersion);

            return true;
        }

        logprintf("[%s] %s: RakServer address not found", Settings::kPluginName, __FUNCTION__);

        return false;
    }

    void Unload(void) {
        logprintf("%s plugin v%s by urShadow unloaded", Settings::kPluginName, Settings::kPluginVersion);
    }

    void AmxLoad(AMX *amx) {
        cell include_version{}, is_gamemode{};

        bool exists = Utils::get_public_var(amx, Settings::kPublicVarNameVersion, include_version) &&
            Utils::get_public_var(amx, Settings::kPublicVarNameIsGamemode, is_gamemode);

        if (exists) {
            if (include_version != PAWNRAKNET_INCLUDE_VERSION) {
                logprintf("[%s] %s: Please update Pawn.RakNet.inc file to the latest version", Settings::kPluginName, __FUNCTION__);

                return;
            }

            Callbacks::OnAmxLoad(amx, is_gamemode == 1);

            Natives::Register(amx);
        }
    }

    void AmxUnload(AMX *amx) {
        Callbacks::OnAmxUnload(amx);
    }
};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

    logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

    return Plugin::Load();
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
