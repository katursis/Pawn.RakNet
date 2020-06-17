/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020 urShadow
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
    std::shared_ptr<urmem::hook>
        hook_get_rakserver_interface,
        hook_amx_cleanup;

    std::shared_ptr<PluginInterface> messageHandler;

    std::array<RPCFunction, PR_MAX_HANDLERS>
        original_rpc,
        custom_rpc;

    std::queue<Packet *> emulating_packets;

    class MessageHandler : public PluginInterface {
        PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet) {
            const auto playerIndex = packet->playerIndex;

            if (playerIndex == static_cast<PlayerIndex>(-1)) {
                return PluginReceiveResult::RR_CONTINUE_PROCESSING;
            }

            RakNet::BitStream bs{packet->data, packet->length, false};

            if (!Scripts::OnEvent<PR_INCOMING_RAW_PACKET>(playerIndex, Functions::RakServer::GetPacketId(packet), &bs)) {
                return PluginReceiveResult::RR_STOP_PROCESSING_AND_DEALLOCATE;
            }

            if (packet->data != bs.GetData()) {
                if (packet->deleteData) {
                    delete packet->data;
                }

                const auto numberOfBitsUsed = bs.CopyData(&packet->data);
                packet->length = BITS_TO_BYTES(numberOfBitsUsed);
                packet->bitSize = numberOfBitsUsed;
                packet->deleteData = true;
            }

            return PluginReceiveResult::RR_CONTINUE_PROCESSING;
        }
    };

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
            if (!bitStream || !bitStream->GetData()) {
                return false;
            }

            if (!Scripts::OnEvent<PR_OUTCOMING_PACKET>(broadcast ? -1 : Functions::RakServer::GetIndexFromPlayerID(playerId), *bitStream->GetData(), bitStream)) {
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

            RakNet::BitStream emptyBitStream;

            if (!bitStream) {
                bitStream = &emptyBitStream;
            }

            if (!Scripts::OnEvent<PR_OUTCOMING_RPC>(broadcast ? -1 : Functions::RakServer::GetIndexFromPlayerID(playerId), rpc_id, bitStream)) {
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
                const auto playerIndex = packet->playerIndex;

                if (playerIndex == static_cast<PlayerIndex>(-1)) {
                    break;
                }

                RakNet::BitStream bs{packet->data, packet->length, false};

                if (Scripts::OnEvent<PR_INCOMING_PACKET>(playerIndex, Functions::RakServer::GetPacketId(packet), &bs)) {
                    if (packet->data != bs.GetData()) {
                        Functions::RakServer::DeallocatePacket(packet);

                        packet = Functions::RakServer::NewPacket(playerIndex, bs);
                    }

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
                    HandleRPC(ID, p);
                }

                static void Init() {
                    custom_rpc[ID] = reinterpret_cast<RPCFunction>(&Interlayer);

                    Handler<ID + 1>::Init();
                }
            };
        };

        static void HandleRPC(int rpc_id, RPCParameters *p) {
            const int player_id = Functions::RakServer::GetIndexFromPlayerID(p->sender);

            RakNet::BitStream bs;

            if (player_id != -1) {
                if (p->input) {
                    bs.SetData(p->input);
                    bs.SetNumberOfBitsAllocated(p->numberOfBitsOfData);
                    bs.SetWriteOffset(p->numberOfBitsOfData);
                }

                if (!Scripts::OnEvent<PR_INCOMING_RPC>(player_id, rpc_id, &bs)) {
                    return;
                }

                const auto numberOfBitsUsed = bs.GetNumberOfBitsUsed();

                if (p->numberOfBitsOfData != numberOfBitsUsed) {
                    p->input = numberOfBitsUsed > 0 ? bs.GetData() : nullptr;
                    p->numberOfBitsOfData = numberOfBitsUsed;
                }
            }

            original_rpc[rpc_id](p);
        }

        static urmem::address_t GetRakServerInterface() {
            const auto rakserver = hook_get_rakserver_interface->call<urmem::calling_convention::cdeclcall, urmem::address_t>();
            const auto vmt = urmem::pointer(rakserver).field<urmem::address_t *>(0);

            Addresses::InitRakServer(rakserver, vmt);

            const auto install_hook = [vmt](Addresses::RakServerOffset offset, urmem::address_t dest) {
                urmem::unprotect_scope scope{
                    reinterpret_cast<urmem::address_t>(&vmt[offset]),
                    sizeof(urmem::address_t)
                };

                vmt[offset] = dest;
            };

            if (Settings::intercept_incoming_packet) {
                install_hook(Addresses::RakServerOffset::RECEIVE, urmem::get_func_addr(&RakServer__Receive));
            }

            if (Settings::intercept_incoming_rpc) {
                install_hook(
                    Addresses::RakServerOffset::REGISTER_AS_REMOTE_PROCEDURE_CALL,
                    urmem::get_func_addr(&RakServer__RegisterAsRemoteProcedureCall)
                );

                ReceiveRPC::Init();
            }

            if (Settings::intercept_outcoming_packet) {
                install_hook(Addresses::RakServerOffset::SEND, urmem::get_func_addr(&RakServer__Send));
            }

            if (Settings::intercept_outcoming_rpc) {
                install_hook(Addresses::RakServerOffset::RPC, urmem::get_func_addr(&RakServer__RPC));
            }

            if (Settings::intercept_incoming_raw_packet) {
                messageHandler = std::make_shared<MessageHandler>();

                Functions::RakServer::AttachPlugin(messageHandler.get());
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
