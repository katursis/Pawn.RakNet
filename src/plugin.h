/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2022 katursis
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

  bool LogAmxErrors();

  void OnUnload();

  void OnProcessTick();

  void InstallPreHooks();

  void SetCustomRPC(RPCIndex rpc_id);

  bool IsCustomRPC(RPCIndex rpc_id);

  const std::shared_ptr<urmem::hook> &GetHookAmxCleanup();

  const std::shared_ptr<Config> &GetConfig();

  template <PR_EventType event_type>
  static bool OnEvent(int player_id, unsigned char event_id, BitStream *bs) {
    return EveryScript([=](const std::shared_ptr<Script> &script) {
      return script->OnEvent<event_type>(player_id, event_id, bs);
    });
  }

  static Plugin &Get() { return Instance(); }

 private:
  std::shared_ptr<Config> config_;

  std::shared_ptr<urmem::hook> hook_amx_cleanup_;

  std::array<bool, PR_MAX_HANDLERS> custom_rpc_{};
};

#endif  // PAWNRAKNET_PLUGIN_H_
