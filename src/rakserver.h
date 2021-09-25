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

#ifndef PAWNRAKNET_RAKSERVER_H_
#define PAWNRAKNET_RAKSERVER_H_

class RakServer {
 public:
  enum class MethodIndex {
#ifdef _WIN32
    kSend = 7,
    kRPC = 32,
    kReceive = 10,
    kRegisterAsRemoteProcedureCall = 29,
    kDeallocatePacket = 12,
    kAttachPlugin = 41,
    kGetIndexFromPlayerID = 57,
    kGetPlayerIDFromIndex = 58
#else
    kSend = 9,
    kRPC = 35,
    kReceive = 11,
    kRegisterAsRemoteProcedureCall = 30,
    kDeallocatePacket = 13,
    kAttachPlugin = 42,
    kGetIndexFromPlayerID = 58,
    kGetPlayerIDFromIndex = 59
#endif
  };

  explicit RakServer(urmem::address_t addr_rakserver);

  bool Send(BitStream *bitStream, int priority, int reliability,
            char orderingChannel, PlayerID playerId, bool broadcast);

  bool RPC(RPCIndex *uniqueID, BitStream *bitStream, int priority,
           int reliability, char orderingChannel, PlayerID playerId,
           bool broadcast, bool shiftTimestamp);

  Packet *Receive();

  void *RegisterAsRemoteProcedureCall(RPCIndex *uniqueID,
                                      RPCFunction functionPointer);

  void DeallocatePacket(Packet *packet);

  void AttachPlugin(PluginInterface *messageHandler);

  int GetIndexFromPlayerID(const PlayerID &playerId);

  const PlayerID GetPlayerIDFromIndex(int index);

  template <typename T>
  void InstallHook(MethodIndex index, T handle) {
    auto &orig_addr = GetMethodAddrFromTable(index);

    urmem::unprotect_scope scope{reinterpret_cast<urmem::address_t>(&orig_addr),
                                 sizeof(urmem::address_t)};

    orig_addr = urmem::get_func_addr(handle);
  }

 private:
  urmem::address_t &GetMethodAddrFromTable(MethodIndex index);

  urmem::address_t addr_rakserver_{};
  urmem::address_t addr_rakserver_vmt_{};

  urmem::address_t addr_rakserver_send_{};
  urmem::address_t addr_rakserver_rpc_{};
  urmem::address_t addr_rakserver_receive_{};
  urmem::address_t addr_rakserver_register_as_remote_procedure_call_{};
  urmem::address_t addr_rakserver_deallocate_packet_{};
  urmem::address_t addr_rakserver_attach_plugin_{};
  urmem::address_t addr_rakserver_get_index_from_player_id_{};
  urmem::address_t addr_rakserver_get_player_id_from_index_{};
};

#endif  // PAWNRAKNET_RAKSERVER_H_
