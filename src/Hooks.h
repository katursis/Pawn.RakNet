#ifndef HOOKS_H_
#define HOOKS_H_

namespace Hooks {
    std::shared_ptr<urmem::hook>
        hook_get_rakserver_interface,
        hook_amx_cleanup;

    std::array<RPCFunction, PR_MAX_HANDLERS>
        original_rpc,
        custom_rpc;

    std::queue<Packet *> emulating_packets;

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

            const std::size_t length = bitStream->GetNumberOfBytesUsed();

            if (!length) {
                return false;
            }

            if (!bitStream->GetData()) {
                return false;
            }

            RakNet::BitStream bs{bitStream->GetData(), length, false};

            if (!Scripts::OnOutcomingPacket(broadcast ? -1 : Functions::RakServer::GetIndexFromPlayerID(playerId), *bs.GetData(), &bs)) {
                return false;
            }

            return Functions::RakServer::Send(bitStream, priority, reliability, orderingChannel, playerId, broadcast);
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

            const int rpc_id = *uniqueID;

            RakNet::BitStream bs;

            if (bitStream) {
                bs.SetData(bitStream->GetData());
                bs.SetNumberOfBitsAllocated(bitStream->GetNumberOfBitsAllocated());
                bs.SetWriteOffset(bitStream->GetWriteOffset());
            }

            if (!Scripts::OnOutcomingRPC(broadcast ? -1 : Functions::RakServer::GetIndexFromPlayerID(playerId), rpc_id, &bs)) {
                return false;
            }

            return Functions::RakServer::RPC(uniqueID, bitStream, priority, reliability, orderingChannel, playerId, broadcast, shiftTimestamp);
        }

        static  Packet * THISCALL RakServer__Receive(void *_this) {
            Packet *packet{};

            if (!emulating_packets.empty()) {
                packet = emulating_packets.front();

                emulating_packets.pop();

                return packet;
            }

            while (packet = Functions::RakServer::Receive()) {
                if (packet->playerIndex == static_cast<PlayerIndex>(-1)) {
                    break;
                }

                RakNet::BitStream bs{packet->data, packet->length, false};

                if (Scripts::OnIncomingPacket(packet->playerIndex, Functions::RakServer::GetPacketId(packet), &bs)) {
                    break;
                }

                Functions::RakServer::DeallocatePacket(packet);
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

            const int rpc_id = *uniqueID;

            original_rpc[rpc_id] = functionPointer;

            return Functions::RakServer::RegisterAsRemoteProcedureCall(uniqueID, custom_rpc[rpc_id]);
        }

        struct ReceiveRPC {
            static void Init() {
                Handler<0>::Init();
            }

            template<std::size_t ID>
            struct Handler {
                static void Interlayer(RPCParameters *p) {
                    const int player_id = Functions::RakServer::GetIndexFromPlayerID(p->sender);

                    if (player_id != -1) {
                        RakNet::BitStream bs;

                        if (p->input) {
                            bs.SetData(p->input);
                            bs.SetNumberOfBitsAllocated(p->numberOfBitsOfData);
                            bs.SetWriteOffset(p->numberOfBitsOfData);
                        }

                        if (!Scripts::OnIncomingRPC(player_id, ID, &bs)) {
                            return;
                        }
                    }

                    original_rpc[ID](p);
                }

                static void Init() {
                    custom_rpc[ID] = reinterpret_cast<RPCFunction>(&Interlayer);

                    Handler<ID + 1>::Init();
                }
            };
        };

        static void * GetRakServerInterface() {
            const auto rakserver = hook_get_rakserver_interface->call<urmem::calling_convention::cdeclcall, void *>();

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

                    ReceiveRPC::Init();
                }
            } else {
                Logger::instance()->Write("[%s] Invalid RakServer VMT", Settings::kPluginName);
            }

            return rakserver;
        }

        static int AMXAPI amx_Cleanup(AMX *amx) {
            Scripts::Unload(amx);

            return hook_amx_cleanup->call<urmem::calling_convention::cdeclcall, int>(amx);
        }
    };

    template<>
    struct InternalHooks::ReceiveRPC::Handler<PR_MAX_HANDLERS> {
        static void Init() {}
    };

    void Init(void *addr_in_server) {
        urmem::sig_scanner scanner;
        urmem::address_t get_rakserver_interface_addr{};

        if (!scanner.init(reinterpret_cast<urmem::address_t>(addr_in_server))) {
            throw std::runtime_error{"signature scanner init error"};
        }

        if (
            !scanner.find(Settings::kGetRakServerInterfacePattern, Settings::kGetRakServerInterfaceMask, get_rakserver_interface_addr) ||
            !get_rakserver_interface_addr
        ) {
            throw std::runtime_error{"GetRakServerInterface not found"};
        }

        if (
            !scanner.find(Settings::kGetPacketIdPattern, Settings::kGetPacketIdMask, Addresses::FUNC_GET_PACKET_ID) ||
            !Addresses::FUNC_GET_PACKET_ID
        ) {
            throw std::runtime_error{"GetPacketId not found"};
        }

        hook_get_rakserver_interface = std::make_shared<urmem::hook>(
            get_rakserver_interface_addr,
            urmem::get_func_addr(&InternalHooks::GetRakServerInterface)
        );

        hook_amx_cleanup = std::make_shared<urmem::hook>(
            reinterpret_cast<urmem::address_t *>(pAMXFunctions)[PLUGIN_AMX_EXPORT_Cleanup],
            urmem::get_func_addr(&InternalHooks::amx_Cleanup)
        );
    }
};

#endif // HOOKS_H_
