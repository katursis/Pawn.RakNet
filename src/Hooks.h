#ifndef HOOKS_H_
#define HOOKS_H_

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall
#else
#define THISCALL
#endif

namespace Hooks {
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

                Logger::instance()->Write("[%s] Addresses found", Settings::kPluginName);

                return true;
            }

            Logger::instance()->Write("[%s] Addresses not found", Settings::kPluginName);

            return false;
        }
    }

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
            Addresses::FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID,
            Addresses::RAKSERVER,
            id
            );
    }

    PlayerID GetPlayerIDFromIndex(int index) {
        return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
            Addresses::FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX,
            Addresses::RAKSERVER,
            index
            );
    }

    void DeallocatePacket(Packet *p) {
        urmem::call_function<urmem::calling_convention::thiscall>(
            Addresses::FUNC_RAKSERVER__DEALLOCATE_PACKET,
            Addresses::RAKSERVER,
            p
            );
    }

    bool SendPacket(int player_id, RakNet::BitStream *bs, int priority, int reliability) {
        const bool enabled = hook_rakserver__send->is_enabled();

        if (enabled) {
            hook_rakserver__send->disable();
        }

        const auto result = urmem::call_function<urmem::calling_convention::thiscall, bool>(
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
        const bool enabled = hook_rakserver__rpc->is_enabled();

        if (enabled) {
            hook_rakserver__rpc->disable();
        }

        static RPCIndex id{};

        id = static_cast<RPCIndex>(rpc_id);

        const auto result = urmem::call_function<urmem::calling_convention::thiscall, bool>(
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
        static bool THISCALL RakServer__Send(
            void *_this,
            RakNet::BitStream *bitStream,
            int priority,
            int reliability,
            char orderingChannel,
            PlayerID playerId,
            bool broadcast
        ) {
            const urmem::hook::raii scope(*hook_rakserver__send);

            if (bitStream) {
                const int
                    read_offset = bitStream->GetReadOffset(),
                    write_offset = bitStream->GetWriteOffset(),
                    packet_id = static_cast<int>(bitStream->GetData()[0]);

                if (!Scripts::OnOutcomingPacket(GetIndexFromPlayerID(playerId), packet_id, bitStream)) {
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
            const urmem::hook::raii scope(*hook_rakserver__rpc);

            if (uniqueID && bitStream) {
                const int
                    read_offset = bitStream->GetReadOffset(),
                    write_offset = bitStream->GetWriteOffset(),
                    rpc_id = static_cast<int>(*uniqueID);

                if (!Scripts::OnOutcomingRPC(GetIndexFromPlayerID(playerId), rpc_id, bitStream)) {
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
            const urmem::hook::raii scope(*hook_rakserver__receive);

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
                    DeallocatePacket(packet);

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
            const urmem::hook::raii scope(*hook_rakserver__register_as_remote_procedure_call);

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
            if (p) {
                const int player_id = GetIndexFromPlayerID(p->sender);

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
        }

        static void * GetRakServerInterface(void) {
            const urmem::hook::raii scope(*hook_get_rak_server_interface);

            const auto rakserver = urmem::call_function<urmem::calling_convention::cdeclcall, void *>(
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

                Logger::instance()->Write("[%s] Initialized", Settings::kPluginName);
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
