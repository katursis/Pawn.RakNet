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

#ifndef PAWNRAKNET_HOOKS_H_
#define PAWNRAKNET_HOOKS_H_

class MessageHandler : public PluginInterface {
 public:
  PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet);
};

class Hooks {
 public:
  static bool THISCALL RakServer__Send(void *_this, BitStream *bs, int priority,
                                       int reliability, char orderingChannel,
                                       PlayerID playerId, bool broadcast);

  static bool THISCALL RakServer__RPC(void *_this, RPCIndex *uniqueID,
                                      BitStream *bs, int priority,
                                      int reliability, char orderingChannel,
                                      PlayerID playerId, bool broadcast,
                                      bool shiftTimestamp);

  static Packet *THISCALL RakServer__Receive(void *_this);

  static void *THISCALL RakServer__RegisterAsRemoteProcedureCall(
      void *_this, RPCIndex *uniqueID, RPCFunction functionPointer);

  struct ReceiveRPC {
    static void Init() { Handler<0>::Init(); }

    template <std::size_t ID>
    struct Handler {
      static void Interlayer(RPCParameters *p) { HandleRPC(ID, p); }

      static void Init() {
        Plugin::Get().SetFakeRPCHandler(
            ID, reinterpret_cast<RPCFunction>(&Interlayer));

        Handler<ID + 1>::Init();
      }
    };
  };

  static void HandleRPC(int rpc_id, RPCParameters *p);

  static urmem::address_t GetRakServerInterface();

  static int AMXAPI amx_Cleanup(AMX *amx);
};

template <>
struct Hooks::ReceiveRPC::Handler<PR_MAX_HANDLERS> {
  static void Init() {}
};

#endif  // PAWNRAKNET_HOOKS_H_
