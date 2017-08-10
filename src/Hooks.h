#ifndef HOOKS_H_
#define HOOKS_H_

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall

enum RakServerOffsets {
    SEND = 7,
    RPC = 32,
    RECEIVE = 10,
    REGISTER_AS_REMOTE_PROCEDURE_CALL = 29,
    DEALLOCATE_PACKET = 12,
    GET_INDEX_FROM_PLAYER_ID = 57,
    GET_PLAYER_ID_FROM_INDEX = 58
};
#else
#define THISCALL

enum RakServerOffsets {
    SEND = 9,
    RPC = 35,
    RECEIVE = 11,
    REGISTER_AS_REMOTE_PROCEDURE_CALL = 30,
    DEALLOCATE_PACKET = 13,
    GET_INDEX_FROM_PLAYER_ID = 58,
    GET_PLAYER_ID_FROM_INDEX = 59
};
#endif

namespace Addresses {
    urmem::address_t
        PTR_RAKSERVER,
        FUNC_RAKSERVER__SEND,
        FUNC_RAKSERVER__RPC,
        FUNC_RAKSERVER__RECEIVE,
        FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
        FUNC_RAKSERVER__DEALLOCATE_PACKET,
        FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID,
        FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX;

    urmem::address_t * Init(urmem::address_t rakserver) {
        if (const auto vmt = urmem::pointer(PTR_RAKSERVER = rakserver).field<urmem::address_t *>(0)) {
            FUNC_RAKSERVER__SEND = vmt[RakServerOffsets::SEND];
            FUNC_RAKSERVER__RPC = vmt[RakServerOffsets::RPC];
            FUNC_RAKSERVER__RECEIVE = vmt[RakServerOffsets::RECEIVE];
            FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL = vmt[RakServerOffsets::REGISTER_AS_REMOTE_PROCEDURE_CALL];
            FUNC_RAKSERVER__DEALLOCATE_PACKET = vmt[RakServerOffsets::DEALLOCATE_PACKET];
            FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID = vmt[RakServerOffsets::GET_INDEX_FROM_PLAYER_ID];
            FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX = vmt[RakServerOffsets::GET_PLAYER_ID_FROM_INDEX];

            return vmt;
        }

        return nullptr;
    }
};

namespace Functions {
    int GetIndexFromPlayerID(const PlayerID &id) {
        return urmem::call_function<urmem::calling_convention::thiscall, int>(
            Addresses::FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID,
            Addresses::PTR_RAKSERVER,
            id
            );
    }

    PlayerID GetPlayerIDFromIndex(int index) {
        return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
            Addresses::FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX,
            Addresses::PTR_RAKSERVER,
            index
            );
    }

    void DeallocatePacket(Packet *p) {
        urmem::call_function<urmem::calling_convention::thiscall>(
            Addresses::FUNC_RAKSERVER__DEALLOCATE_PACKET,
            Addresses::PTR_RAKSERVER,
            p
            );
    }

    bool SendPacket(int player_id, RakNet::BitStream *bs, int priority, int reliability) {
        return urmem::call_function<urmem::calling_convention::thiscall, bool>(
            Addresses::FUNC_RAKSERVER__SEND,
            Addresses::PTR_RAKSERVER,
            bs,
            priority,
            reliability,
            '\0',
            GetPlayerIDFromIndex(player_id),
            player_id == -1
            );
    }

    bool SendRPC(int player_id, int rpc_id, RakNet::BitStream *bs, int priority, int reliability) {
        static RPCIndex id{};

        id = static_cast<RPCIndex>(rpc_id);

        return urmem::call_function<urmem::calling_convention::thiscall, bool>(
            Addresses::FUNC_RAKSERVER__RPC,
            Addresses::PTR_RAKSERVER,
            &id,
            bs,
            priority,
            reliability,
            '\0',
            GetPlayerIDFromIndex(player_id),
            player_id == -1,
            false
            );
    }
};

namespace Hooks {
    std::shared_ptr<urmem::hook> hook_get_rak_server_interface;

    std::array<RPCFunction, MAX_RPC_MAP_SIZE>
        original_rpc,
        custom_rpc;

    class InternalHooks {
    public:
        static bool THISCALL RakServer__Send(
            void *_this,
            RakNet::BitStream *bitStream,
            int priority,
            int reliability,
            char orderingChannel,
            PlayerID playerId,
            bool broadcast
        ) {
            if (bitStream) {
                const int
                    read_offset = bitStream->GetReadOffset(),
                    write_offset = bitStream->GetWriteOffset(),
                    packet_id = static_cast<int>(bitStream->GetData()[0]);

                if (!Scripts::OnOutcomingPacket(Functions::GetIndexFromPlayerID(playerId), packet_id, bitStream)) {
                    return false;
                }

                bitStream->SetReadOffset(read_offset);

                bitStream->SetWriteOffset(write_offset);
            }

            return urmem::call_function<urmem::calling_convention::thiscall, bool>(
                Addresses::FUNC_RAKSERVER__SEND,
                _this,
                bitStream,
                priority,
                reliability,
                orderingChannel,
                playerId,
                broadcast
                );
        }

        static bool THISCALL RakServer__RPC(
            void *_this,
            RPCIndex *uniqueID,
            RakNet::BitStream *bitStream,
            int priority,
            int reliability,
            char orderingChannel,
            PlayerID playerId,
            bool broadcast,
            bool shiftTimestamp
        ) {
            if (uniqueID && bitStream) {
                const int
                    read_offset = bitStream->GetReadOffset(),
                    write_offset = bitStream->GetWriteOffset(),
                    rpc_id = static_cast<int>(*uniqueID);

                if (!Scripts::OnOutcomingRPC(Functions::GetIndexFromPlayerID(playerId), rpc_id, bitStream)) {
                    return false;
                }

                bitStream->SetReadOffset(read_offset);

                bitStream->SetWriteOffset(write_offset);
            }

            return urmem::call_function<urmem::calling_convention::thiscall, bool>(
                Addresses::FUNC_RAKSERVER__RPC,
                _this,
                uniqueID,
                bitStream,
                priority,
                reliability,
                orderingChannel,
                playerId,
                broadcast,
                shiftTimestamp
                );
        }

        static  Packet * THISCALL RakServer__Receive(void *_this) {
            Packet *packet = urmem::call_function<urmem::calling_convention::thiscall, Packet *>(
                Addresses::FUNC_RAKSERVER__RECEIVE,
                _this
                );

            if (packet && packet->data) {
                RakNet::BitStream bitstream(packet->data, packet->length, false);

                const int
                    player_id = static_cast<int>(packet->playerIndex),
                    packet_id = static_cast<int>(packet->data[0]);

                if (!Scripts::OnIncomingPacket(player_id, packet_id, &bitstream)) {
                    Functions::DeallocatePacket(packet);

                    return nullptr;
                }
            }

            return packet;
        }

        static void * THISCALL RakServer__RegisterAsRemoteProcedureCall(
            void *_this,
            RPCIndex *uniqueID,
            RPCFunction functionPointer
        ) {
            if (uniqueID && functionPointer) {
                const int rpc_id = static_cast<int>(*uniqueID);

                original_rpc[rpc_id] = functionPointer;

                return urmem::call_function<urmem::calling_convention::thiscall, void *>(
                    Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                    _this,
                    uniqueID,
                    custom_rpc[rpc_id]
                    );
            }

            return urmem::call_function<urmem::calling_convention::thiscall, void *>(
                Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                _this,
                uniqueID,
                functionPointer
                );
        }

        static void ReceiveRPC(int rpc_id, RPCParameters *p) {
            if (!p) {
                return;
            }

            const int player_id = Functions::GetIndexFromPlayerID(p->sender);

            std::unique_ptr<RakNet::BitStream> bs;

            if (p->input) {
                bs = std::unique_ptr<RakNet::BitStream>(
                    new RakNet::BitStream{ p->input, BITS_TO_BYTES(p->numberOfBitsOfData), false }
                );
            }

            if (!Scripts::OnIncomingRPC(player_id, rpc_id, bs.get())) {
                return;
            }

            if (original_rpc[rpc_id]) {
                original_rpc[rpc_id](p);
            } else {
                Logger::instance()->Write("[%s] %s: unknown rpc_id (%d)", Settings::kPluginName, __FUNCTION__, rpc_id);
            }
        }

        static void * GetRakServerInterface(void) {
            const urmem::hook::raii scope(*hook_get_rak_server_interface);

            const auto rakserver = urmem::call_function<urmem::calling_convention::cdeclcall, void *>(
                hook_get_rak_server_interface->get_original_addr()
                );

            if (const auto vmt = Addresses::Init(reinterpret_cast<urmem::address_t>(rakserver))) {
                urmem::unprotect_scope scope{
                    reinterpret_cast<urmem::address_t>(&vmt[0]),
                    RakServerOffsets::REGISTER_AS_REMOTE_PROCEDURE_CALL * sizeof(urmem::address_t)
                };

                vmt[RakServerOffsets::SEND] = urmem::get_func_addr(&RakServer__Send);
                vmt[RakServerOffsets::RPC] = urmem::get_func_addr(&RakServer__RPC);
                vmt[RakServerOffsets::RECEIVE] = urmem::get_func_addr(&RakServer__Receive);
                vmt[RakServerOffsets::REGISTER_AS_REMOTE_PROCEDURE_CALL] = urmem::get_func_addr(&RakServer__RegisterAsRemoteProcedureCall);

                original_rpc.fill(nullptr);
                custom_rpc.fill(nullptr);

                RPCHandle::Generate();

                Logger::instance()->Write("[%s] Initialized", Settings::kPluginName);
            }

            return rakserver;
        }

        struct RPCHandle {
            static void Generate(void) {
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

    bool Init(void *addr_in_server) {
        urmem::sig_scanner scanner;

        if (scanner.init(addr_in_server)) {
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

#endif // HOOKS_H_
