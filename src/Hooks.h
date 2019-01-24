#ifndef HOOKS_H_
#define HOOKS_H_

namespace Hooks {
    std::shared_ptr<urmem::hook>
        hook_get_rak_server_interface,
        hook_amx_cleanup;

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
            if (!bitStream) {
                return false;
            }

            if (bitStream->GetNumberOfBytesUsed() <= 0) {
                return false;
            }

            const int
                read_offset = bitStream->GetReadOffset(),
                write_offset = bitStream->GetWriteOffset(),
                packet_id = static_cast<int>(bitStream->GetData()[0]);

            if (!Scripts::OnOutcomingPacket(Functions::GetIndexFromPlayerID(playerId), packet_id, bitStream)) {
                return false;
            }

            bitStream->SetReadOffset(read_offset);
            bitStream->SetWriteOffset(write_offset);

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
            if (!uniqueID) {
                return false;
            }

            const int rpc_id = static_cast<int>(*uniqueID);

            if (bitStream) {
                const int
                    read_offset = bitStream->GetReadOffset(),
                    write_offset = bitStream->GetWriteOffset();

                if (!Scripts::OnOutcomingRPC(Functions::GetIndexFromPlayerID(playerId), rpc_id, bitStream)) {
                    return false;
                }

                bitStream->SetReadOffset(read_offset);
                bitStream->SetWriteOffset(write_offset);
            } else {
                if (!Scripts::OnOutcomingRPC(Functions::GetIndexFromPlayerID(playerId), rpc_id, nullptr)) {
                    return false;
                }
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
            Packet *packet{};

            while (packet = urmem::call_function<urmem::calling_convention::thiscall, Packet *>(Addresses::FUNC_RAKSERVER__RECEIVE, _this)) {
                RakNet::BitStream bitstream(packet->data, packet->length, false);

                const int
                    player_id = static_cast<int>(packet->playerIndex),
                    packet_id = static_cast<int>(packet->data[0]);

                if (Scripts::OnIncomingPacket(player_id, packet_id, &bitstream)) {
                    break;
                }

                Functions::DeallocatePacket(packet);
            }

            return packet;
        }

        static void * THISCALL RakServer__RegisterAsRemoteProcedureCall(
            void *_this,
            RPCIndex *uniqueID,
            RPCFunction functionPointer
        ) {
            if (!uniqueID) {
                return nullptr;
            }

            if (!functionPointer) {
                return nullptr;
            }

            const int rpc_id = static_cast<int>(*uniqueID);

            original_rpc[rpc_id] = functionPointer;

            functionPointer = custom_rpc[rpc_id];

            return urmem::call_function<urmem::calling_convention::thiscall, void *>(
                Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                _this,
                uniqueID,
                functionPointer
            );
        }

        static bool ReceiveRPC(int rpc_id, RPCParameters *p) {
            if (!p) {
                return false;
            }

            const int player_id = Functions::GetIndexFromPlayerID(p->sender);

            std::unique_ptr<RakNet::BitStream> bs;

            if (p->input) {
                bs = std::unique_ptr<RakNet::BitStream>(
                    new RakNet::BitStream{ p->input, BITS_TO_BYTES(p->numberOfBitsOfData), false }
                );
            }

            return Scripts::OnIncomingRPC(player_id, rpc_id, bs.get());
        }

        static void * GetRakServerInterface() {
            const urmem::hook::raii scope(*hook_get_rak_server_interface);

            const auto rakserver = urmem::call_function<urmem::calling_convention::cdeclcall, void *>(
                hook_get_rak_server_interface->get_original_addr()
            );

            if (const auto vmt = Addresses::Init(reinterpret_cast<urmem::address_t>(rakserver))) {
                const auto install_hook = [vmt](RakServerOffsets offset, urmem::address_t dest) {
                    urmem::unprotect_scope scope{
                        reinterpret_cast<urmem::address_t>(&vmt[offset]),
                        sizeof(urmem::address_t)
                    };

                    vmt[offset] = dest;
                };

                if (Settings::intercept_outcoming_packet) {
                    install_hook(RakServerOffsets::SEND, urmem::get_func_addr(&RakServer__Send));
                }

                if (Settings::intercept_outcoming_rpc) {
                    install_hook(RakServerOffsets::RPC, urmem::get_func_addr(&RakServer__RPC));
                }

                if (Settings::intercept_incoming_packet) {
                    install_hook(RakServerOffsets::RECEIVE, urmem::get_func_addr(&RakServer__Receive));
                }

                if (Settings::intercept_incoming_rpc) {
                    install_hook(
                        RakServerOffsets::REGISTER_AS_REMOTE_PROCEDURE_CALL,
                        urmem::get_func_addr(&RakServer__RegisterAsRemoteProcedureCall)
                    );

                    original_rpc.fill(nullptr);
                    custom_rpc.fill(nullptr);

                    RPCHandle::Generate();
                }
            } else {
                Logger::instance()->Write("[%s] Invalid RakServer VMT", Settings::kPluginName);
            }

            return rakserver;
        }

        static int AMXAPI amx_Cleanup(AMX *amx) {
            const urmem::hook::raii scope(*hook_amx_cleanup);

            Scripts::Unload(amx);

            const auto amx_err_code = urmem::call_function<urmem::calling_convention::cdeclcall, int>(
                hook_amx_cleanup->get_original_addr(),
                amx
            );

            return amx_err_code;
        }

        struct RPCHandle {
            static void Generate() {
                Generator<0>::Run();
            }

            template<size_t ID>
            struct Generator {
                static void Handle(RPCParameters *p) {
                    if (ReceiveRPC(ID, p)) {
                        original_rpc[ID](p);
                    }
                }

                static void Run() {
                    custom_rpc[ID] = reinterpret_cast<RPCFunction>(&Handle);

                    Generator<ID + 1>::Run();
                }
            };
        };
    };

    template<>
    struct InternalHooks::RPCHandle::Generator<MAX_RPC_MAP_SIZE> {
        static void Run() {}
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

                hook_amx_cleanup = std::make_shared<urmem::hook>(
                    reinterpret_cast<urmem::address_t *>(pAMXFunctions)[PLUGIN_AMX_EXPORT_Cleanup],
                    urmem::get_func_addr(&InternalHooks::amx_Cleanup)
                );

                return true;
            }
        }

        return false;
    }
};

#endif // HOOKS_H_
