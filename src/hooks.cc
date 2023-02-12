/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 katursis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "main.h"

PluginReceiveResult MessageHandler::OnReceive(RakPeerInterface *peer,
                                              Packet *packet) {
  const auto player_id = packet->playerIndex;

  if (player_id == static_cast<PlayerIndex>(-1)) {
    return PluginReceiveResult::RR_CONTINUE_PROCESSING;
  }

  auto &plugin = Plugin::Get();

  BitStream bs{packet->data, packet->length, false};

  if (!Plugin::OnEvent<PR_INCOMING_RAW_PACKET>(
          player_id, plugin.GetPacketId(packet), &bs)) {
    return PluginReceiveResult::RR_STOP_PROCESSING_AND_DEALLOCATE;
  }

  if (packet->data != bs.GetData()) {
    if (packet->deleteData) {
      delete packet->data;
    }

    const auto number_of_bits_used = bs.CopyData(&packet->data);
    packet->length = BITS_TO_BYTES(number_of_bits_used);
    packet->bitSize = number_of_bits_used;
    packet->deleteData = true;
  }

  return PluginReceiveResult::RR_CONTINUE_PROCESSING;
}

void MessageHandler::OnInternalPacket(InternalPacket *internalPacket,
                                      unsigned frameNumber,
                                      PlayerID remoteSystemID, RakNetTime time,
                                      bool isSend) {
  auto &plugin = Plugin::Get();
  auto &config = plugin.GetConfig();
  auto &ch = plugin.GetInternalPacketChannel();

  if (!internalPacket || !internalPacket->data || !ch || ch->IsClosed() ||
      (isSend && !config->InterceptOutgoingInternalPacket()) ||
      (!isSend && !config->InterceptIncomingInternalPacket()) ||
      !config->IsWhiteListedInternalPacket(internalPacket->data[0])) {
    return;
  }

  ch->PushPacket(internalPacket, remoteSystemID, isSend);

  if (!ch->PopResult()) {
    internalPacket->data[0] = 0;
  }
}

void MessageHandler::OnInitialize(RakPeerInterface *peer) {
  auto &ch = Plugin::Get().GetInternalPacketChannel();
  if (ch) {
    ch->Open();
  }
}

void MessageHandler::OnDisconnect(RakPeerInterface *peer) {
  auto &ch = Plugin::Get().GetInternalPacketChannel();
  if (ch) {
    ch->Close();
  }
}

bool THISCALL Hooks::RakServer__Send(void *_this, BitStream *bs, int priority,
                                     int reliability, char orderingChannel,
                                     PlayerID playerId, bool broadcast) {
  if (!bs || !bs->GetData()) {
    return false;
  }

  auto &rakserver = Plugin::Get().GetRakServer();

  if (!Plugin::OnEvent<PR_OUTGOING_PACKET>(
          broadcast ? -1 : rakserver->GetIndexFromPlayerID(playerId),
          *bs->GetData(), bs)) {
    return false;
  }

  return rakserver->Send(bs, priority, reliability, orderingChannel, playerId,
                         broadcast);
}

bool THISCALL Hooks::RakServer__RPC(void *_this, RPCIndex *uniqueID,
                                    BitStream *bs, int priority,
                                    int reliability, char orderingChannel,
                                    PlayerID playerId, bool broadcast,
                                    bool shiftTimestamp) {
  if (!uniqueID) {
    return false;
  }

  const auto rpc_id = *uniqueID;

  BitStream empty_bs;
  if (!bs) {
    bs = &empty_bs;
  }

  auto &rakserver = Plugin::Get().GetRakServer();

  if (!Plugin::OnEvent<PR_OUTGOING_RPC>(
          broadcast ? -1 : rakserver->GetIndexFromPlayerID(playerId), rpc_id,
          bs)) {
    return false;
  }

  return rakserver->RPC(uniqueID, bs, priority, reliability, orderingChannel,
                        playerId, broadcast, shiftTimestamp);
}

Packet *THISCALL Hooks::RakServer__Receive(void *_this) {
  auto &plugin = Plugin::Get();
  auto &rakserver = plugin.GetRakServer();

  auto packet = plugin.GetNextPacketToEmulate();
  if (packet) {
    return packet;
  }

  while (packet = rakserver->Receive()) {
    const auto player_id = packet->playerIndex;
    if (player_id == static_cast<PlayerIndex>(-1)) {
      break;
    }

    BitStream bs{packet->data, packet->length, false};

    if (Plugin::OnEvent<PR_INCOMING_PACKET>(player_id,
                                            plugin.GetPacketId(packet), &bs)) {
      if (packet->data != bs.GetData()) {
        rakserver->DeallocatePacket(packet);

        packet = plugin.NewPacket(player_id, bs);
      }

      break;
    }

    rakserver->DeallocatePacket(packet);
  }

  return packet;
}

void *THISCALL Hooks::RakServer__RegisterAsRemoteProcedureCall(
    void *_this, RPCIndex *uniqueID, RPCFunction functionPointer) {
  if (!uniqueID || !functionPointer) {
    return nullptr;
  }

  auto &plugin = Plugin::Get();
  auto &rakserver = plugin.GetRakServer();
  auto &config = plugin.GetConfig();

  const auto rpc_id = *uniqueID;

  plugin.SetOriginalRPCHandler(rpc_id, functionPointer);

  if (config->InterceptIncomingRPC()) {
    functionPointer = plugin.GetFakeRPCHandler(rpc_id);
  }

  return rakserver->RegisterAsRemoteProcedureCall(uniqueID, functionPointer);
}

void Hooks::HandleRPC(RPCIndex rpc_id, RPCParameters *p) {
  auto &plugin = Plugin::Get();
  auto &rakserver = plugin.GetRakServer();

  const int player_id = rakserver->GetIndexFromPlayerID(p->sender);
  if (player_id == -1) {
    return;
  }

  BitStream bs;
  if (p->input) {
    bs.SetData(p->input);
    bs.SetNumberOfBitsAllocated(p->numberOfBitsOfData);
    bs.SetWriteOffset(p->numberOfBitsOfData);
  }

  const auto original_handler = plugin.GetOriginalRPCHandler(rpc_id);

  const auto on_event = original_handler
                            ? Plugin::OnEvent<PR_INCOMING_RPC>
                            : Plugin::OnEvent<PR_INCOMING_CUSTOM_RPC>;
  if (!on_event(player_id, rpc_id, &bs)) {
    return;
  }

  if (original_handler) {
    const auto numberOfBitsUsed = bs.GetNumberOfBitsUsed();
    if (p->numberOfBitsOfData != numberOfBitsUsed) {
      p->input = numberOfBitsUsed > 0 ? bs.GetData() : nullptr;
      p->numberOfBitsOfData = numberOfBitsUsed;
    }

    original_handler(p);
  }
}

urmem::address_t Hooks::GetRakServerInterface() {
  auto &plugin = Plugin::Get();

  const auto addr_rakserver =
      plugin.GetHookGetRakServerInterface()
          ->call<urmem::calling_convention::cdeclcall, urmem::address_t>();

  plugin.InstallRakServerHooks(addr_rakserver);

  return addr_rakserver;
}

int AMXAPI Hooks::amx_Cleanup(AMX *amx) {
  auto &plugin = Plugin::Get();

  Plugin::DoAmxUnload(amx);

  return plugin.GetHookAmxCleanup()
      ->call<urmem::calling_convention::cdeclcall, int>(amx);
}
