/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 katursis
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

  RakNet::BitStream bs{packet->data, packet->length, false};

  if (!Plugin::OnEvent(PR_INCOMING_RAW_PACKET, player_id,
                       Plugin::Get().GetPacketId(packet), &bs)) {
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

bool THISCALL Hooks::RakServer__Send(void *_this, RakNet::BitStream *bs,
                                     int priority, int reliability,
                                     char orderingChannel, PlayerID playerId,
                                     bool broadcast) {
  if (!bs || !bs->GetData()) {
    return false;
  }

  if (!Plugin::OnEvent(
          PR_OUTCOMING_PACKET,
          broadcast
              ? -1
              : Plugin::Get().GetRakServer()->GetIndexFromPlayerID(playerId),
          *bs->GetData(), bs)) {
    return false;
  }

  return Plugin::Get().GetRakServer()->Send(
      bs, priority, reliability, orderingChannel, playerId, broadcast);
}

bool THISCALL Hooks::RakServer__RPC(void *_this, RPCIndex *uniqueID,
                                    RakNet::BitStream *bs, int priority,
                                    int reliability, char orderingChannel,
                                    PlayerID playerId, bool broadcast,
                                    bool shiftTimestamp) {
  if (!uniqueID) {
    return false;
  }

  const int rpc_id = *uniqueID;

  RakNet::BitStream empty_bs;
  if (!bs) {
    bs = &empty_bs;
  }

  if (!Plugin::OnEvent(
          PR_OUTCOMING_RPC,
          broadcast
              ? -1
              : Plugin::Get().GetRakServer()->GetIndexFromPlayerID(playerId),
          rpc_id, bs)) {
    return false;
  }

  return Plugin::Get().GetRakServer()->RPC(uniqueID, bs, priority, reliability,
                                           orderingChannel, playerId, broadcast,
                                           shiftTimestamp);
}

Packet *THISCALL Hooks::RakServer__Receive(void *_this) {
  auto packet = Plugin::Get().GetNextPacketToEmulate();
  if (packet) {
    return packet;
  }

  while (packet = Plugin::Get().GetRakServer()->Receive()) {
    const auto player_id = packet->playerIndex;
    if (player_id == static_cast<PlayerIndex>(-1)) {
      break;
    }

    RakNet::BitStream bs{packet->data, packet->length, false};
    if (Plugin::OnEvent(PR_INCOMING_PACKET, player_id,
                        Plugin::Get().GetPacketId(packet), &bs)) {
      if (packet->data != bs.GetData()) {
        Plugin::Get().GetRakServer()->DeallocatePacket(packet);

        packet = Plugin::Get().NewPacket(player_id, bs);
      }

      break;
    }

    Plugin::Get().GetRakServer()->DeallocatePacket(packet);
  }

  return packet;
}

void *THISCALL Hooks::RakServer__RegisterAsRemoteProcedureCall(
    void *_this, RPCIndex *uniqueID, RPCFunction functionPointer) {
  if (!uniqueID || !functionPointer) {
    return nullptr;
  }

  const int rpc_id = *uniqueID;

  Plugin::Get().SetOriginalRPCHandler(rpc_id, functionPointer);

  return Plugin::Get().GetRakServer()->RegisterAsRemoteProcedureCall(
      uniqueID, Plugin::Get().GetFakeRPCHandler(rpc_id));
}

void Hooks::HandleRPC(int rpc_id, RPCParameters *p) {
  const int player_id =
      Plugin::Get().GetRakServer()->GetIndexFromPlayerID(p->sender);

  RakNet::BitStream bs;

  if (player_id != -1) {
    if (p->input) {
      bs.SetData(p->input);
      bs.SetNumberOfBitsAllocated(p->numberOfBitsOfData);
      bs.SetWriteOffset(p->numberOfBitsOfData);
    }

    if (!Plugin::OnEvent(PR_INCOMING_RPC, player_id, rpc_id, &bs)) {
      return;
    }

    const auto numberOfBitsUsed = bs.GetNumberOfBitsUsed();
    if (p->numberOfBitsOfData != numberOfBitsUsed) {
      p->input = numberOfBitsUsed > 0 ? bs.GetData() : nullptr;
      p->numberOfBitsOfData = numberOfBitsUsed;
    }
  }

  Plugin::Get().GetOriginalRPCHandler(rpc_id)(p);
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
