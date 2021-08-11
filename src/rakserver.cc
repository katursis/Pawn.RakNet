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

RakServer::RakServer(urmem::address_t addr_rakserver)
    : addr_rakserver_{addr_rakserver} {
  addr_rakserver_vmt_ =
      urmem::pointer(addr_rakserver_).field<urmem::address_t>(0);

  addr_rakserver_send_ = GetMethodAddrFromTable(MethodIndex::kSend);
  addr_rakserver_rpc_ = GetMethodAddrFromTable(MethodIndex::kRPC);
  addr_rakserver_receive_ = GetMethodAddrFromTable(MethodIndex::kReceive);
  addr_rakserver_register_as_remote_procedure_call_ =
      GetMethodAddrFromTable(MethodIndex::kRegisterAsRemoteProcedureCall);
  addr_rakserver_deallocate_packet_ =
      GetMethodAddrFromTable(MethodIndex::kDeallocatePacket);
  addr_rakserver_attach_plugin_ =
      GetMethodAddrFromTable(MethodIndex::kAttachPlugin);
  addr_rakserver_get_index_from_player_id_ =
      GetMethodAddrFromTable(MethodIndex::kGetIndexFromPlayerID);
  addr_rakserver_get_player_id_from_index_ =
      GetMethodAddrFromTable(MethodIndex::kGetPlayerIDFromIndex);
}

bool RakServer::Send(BitStream *bitStream, int priority, int reliability,
                     char orderingChannel, PlayerID playerId, bool broadcast) {
  return urmem::call_function<urmem::calling_convention::thiscall, bool>(
      addr_rakserver_send_, addr_rakserver_, bitStream, priority, reliability,
      orderingChannel, playerId, broadcast);
}

bool RakServer::RPC(RPCIndex *uniqueID, BitStream *bitStream, int priority,
                    int reliability, char orderingChannel, PlayerID playerId,
                    bool broadcast, bool shiftTimestamp) {
  return urmem::call_function<urmem::calling_convention::thiscall, bool>(
      addr_rakserver_rpc_, addr_rakserver_, uniqueID, bitStream, priority,
      reliability, orderingChannel, playerId, broadcast, shiftTimestamp);
}

Packet *RakServer::Receive() {
  return urmem::call_function<urmem::calling_convention::thiscall, Packet *>(
      addr_rakserver_receive_, addr_rakserver_);
}

void *RakServer::RegisterAsRemoteProcedureCall(RPCIndex *uniqueID,
                                               RPCFunction functionPointer) {
  return urmem::call_function<urmem::calling_convention::thiscall, void *>(
      addr_rakserver_register_as_remote_procedure_call_, addr_rakserver_,
      uniqueID, functionPointer);
}

void RakServer::DeallocatePacket(Packet *packet) {
  urmem::call_function<urmem::calling_convention::thiscall>(
      addr_rakserver_deallocate_packet_, addr_rakserver_, packet);
}

void RakServer::AttachPlugin(PluginInterface *messageHandler) {
  urmem::call_function<urmem::calling_convention::thiscall>(
      addr_rakserver_attach_plugin_, addr_rakserver_, messageHandler);
}

int RakServer::GetIndexFromPlayerID(const PlayerID &playerId) {
  return urmem::call_function<urmem::calling_convention::thiscall, int>(
      addr_rakserver_get_index_from_player_id_, addr_rakserver_, playerId);
}

const PlayerID RakServer::GetPlayerIDFromIndex(int index) {
  return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
      addr_rakserver_get_player_id_from_index_, addr_rakserver_, index);
}

urmem::address_t &RakServer::GetMethodAddrFromTable(MethodIndex index) {
  return reinterpret_cast<urmem::address_t *>(
      addr_rakserver_vmt_)[static_cast<std::size_t>(index)];
}
