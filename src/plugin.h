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

  void InstallPreHooks();

  void InstallRakServerHooks(urmem::address_t addr_rakserver);

  int GetPacketId(Packet *packet);

  Packet *NewPacket(PlayerIndex index, const BitStream &bs);

  void PushPacketToEmulate(Packet *packet);

  Packet *GetNextPacketToEmulate();

  void SetOriginalRPCHandler(int rpc_id, RPCFunction handler);

  RPCFunction GetOriginalRPCHandler(int rpc_id);

  void SetFakeRPCHandler(int rpc_id, RPCFunction handler);

  RPCFunction GetFakeRPCHandler(int rpc_id);

  const std::shared_ptr<urmem::hook> &GetHookGetRakServerInterface();

  const std::shared_ptr<urmem::hook> &GetHookAmxCleanup();

  const std::shared_ptr<Config> &GetConfig();

  const std::shared_ptr<RakServer> &GetRakServer();

  template <PR_EventType event_type>
  static bool OnEvent(int player_id, int id, BitStream *bs) {
    return EveryScript([=](const std::shared_ptr<Script> &script) {
      return script->OnEvent<event_type>(player_id, id, bs);
    });
  }

  static Plugin &Get() { return Instance(); }

 private:
#ifdef _WIN32
  const char *get_rakserver_interface_pattern_ =
      "\x6A\xFF\x68\x5B\xA4\x4A\x00\x64\xA1\x00\x00"
      "\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51"
      "\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83"
      "\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x24\xFF"
      "\x00\x00\x00\x00\x74\x16";
  const char *get_rakserver_interface_mask_ =
      "???????xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx";

  const char *get_packet_id_pattern_ =
      "\x8B\x44\x24\x04\x85\xC0\x75\x03\x0C\xFF\xC3\x8B\x48\x10\x8A\x01"
      "\x3C\xFF\x75\x03\x8A\x41\x05\xC3";
  const char *get_packet_id_mask_ = "?????xxxxxxxxxxxx?xxxxxx";
#else
  const char *get_rakserver_interface_pattern_ =
      "\x55\x89\xE5\x83\xEC\x18\xC7\x04\x24\xFF\xFF"
      "\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF"
      "\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF"
      "\xFF\x89\xF0\x8B\x5D\xFF\x8B\x75\xFF\x89\xEC"
      "\x5D\xC3";
  const char *get_rakserver_interface_mask_ =
      "?????xxxx????xx?xx?x????xxxxxx????xxxx?xx?xxxx";

  const char *get_packet_id_pattern_ =
      "\x55\xB8\xFF\x00\x00\x00\x89\xE5\x8B\x55\x08\x85\xD2\x74\x0D\x8B"
      "\x52\x10\x0F\xB6\x02\x3C\xFF\x74\x07\x0F\xB6\xC0\x5D\xC3\x66\x90"
      "\x0F\xB6\x42\x05\x5D\xC3";
  const char *get_packet_id_mask_ = "?????xxxxxxxxxxxxxxxxx?xxxxxxxxxxxxxxx";
#endif

  std::shared_ptr<Config> config_;

  urmem::address_t addr_get_packet_id_{};

  std::shared_ptr<RakServer> rakserver_;

  std::shared_ptr<urmem::hook> hook_get_rakserver_interface_;
  std::shared_ptr<urmem::hook> hook_amx_cleanup_;

  std::shared_ptr<PluginInterface> message_handler_;

  std::array<RPCFunction, PR_MAX_HANDLERS> original_rpc_;
  std::array<RPCFunction, PR_MAX_HANDLERS> fake_rpc_;

  std::queue<Packet *> emulating_packets_;
};

#endif  // PAWNRAKNET_PLUGIN_H_
