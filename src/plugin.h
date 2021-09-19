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

#ifndef PAWNRAKNET_PLUGIN_H_
#define PAWNRAKNET_PLUGIN_H_

class Plugin : public ptl::AbstractPlugin<Plugin, Script, NativeParam> {
 public:
  const char *Name() { return "Pawn.RakNet"; }

  int Version() { return PAWNRAKNET_VERSION; }

  bool OnLoad();

  void OnUnload();

  void OnProcessTick();

  void InstallPreHooks();

  void InstallRakServerHooks(urmem::address_t addr_rakserver);

  unsigned char GetPacketId(Packet *packet);

  Packet *NewPacket(PlayerIndex index, const BitStream &bs);

  void PushPacketToEmulate(Packet *packet);

  Packet *GetNextPacketToEmulate();

  void SetOriginalRPCHandler(RPCIndex rpc_id, RPCFunction handler);

  RPCFunction GetOriginalRPCHandler(RPCIndex rpc_id);

  void SetFakeRPCHandler(RPCIndex rpc_id, RPCFunction handler);

  RPCFunction GetFakeRPCHandler(RPCIndex rpc_id);

  const std::shared_ptr<urmem::hook> &GetHookAmxCleanup();

  const std::shared_ptr<Config> &GetConfig();

  const std::shared_ptr<RakServer> &GetRakServer();

  const std::shared_ptr<InternalPacketChannel> &GetInternalPacketChannel();

  void ProcessInternalPackets();

  template <PR_EventType event_type>
  static bool OnEvent(int player_id, unsigned char event_id, BitStream *bs) {
    return EveryScript([=](const std::shared_ptr<Script> &script) {
      return script->OnEvent<event_type>(player_id, event_id, bs);
    });
  }

  static Plugin &Get() { return Instance(); }

 private:

 private:
  bool initialized_hooks_ = false;

  std::shared_ptr<Config> config_;

  std::shared_ptr<RakServer> rakserver_;

  std::shared_ptr<urmem::hook> hook_amx_cleanup_;

  std::shared_ptr<PluginInterface> message_handler_;

  std::shared_ptr<InternalPacketChannel> internal_packet_channel_;

  std::array<RPCFunction, PR_MAX_HANDLERS> original_rpc_;
  std::array<RPCFunction, PR_MAX_HANDLERS> fake_rpc_;

  std::queue<Packet *> emulating_packets_;
};

#endif  // PAWNRAKNET_PLUGIN_H_
