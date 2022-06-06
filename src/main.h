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

#ifndef PAWNRAKNET_MAIN_H_
#define PAWNRAKNET_MAIN_H_

#define _GLIBCXX_USE_CXX11_ABI 1

#include "sdk.hpp"
#include "Server/Components/Pawn/pawn.hpp"
#include "Impl/network_impl.hpp"
#include "samp-ptl/ptl.h"
#include "RakNet/bitstream.hpp"
#include "RakNet/Encoding/str_compress.hpp"
#include "urmem/urmem.hpp"
#include "cpptoml/include/cpptoml.h"

#include <unordered_set>
#include <set>
#include <limits>
#include <list>
#include <array>
#include <string>
#include <regex>
#include <queue>
#include <thread>
#include <atomic>
#include <vector>
#include <cstdarg>

#include "Pawn.RakNet.inc"

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall
#else
#define THISCALL
#endif

using BitStream = NetworkBitStream;
using RPCIndex = unsigned char;

#include "config.h"
#include "bitstream_pool.h"
#include "script.h"
#include "native_param.h"
#include "plugin.h"
#include "hooks.h"

class PluginComponent final : public IComponent,
                              public PawnEventHandler,
                              public CoreEventHandler,
                              public NetworkInEventHandler,
                              public NetworkOutEventHandler {
 public:
  PROVIDE_UID(0x4a8b15c16d23e42f);

  StringView componentName() const override;

  SemanticVersion componentVersion() const override;

  void onLoad(ICore *c) override;

  void onInit(IComponentList *components) override;

  void onAmxLoad(void *amx) override;

  void onAmxUnload(void *amx) override;

  void onTick(Microseconds elapsed, TimePoint now) override;

  bool onReceivePacket(IPlayer &peer, int id, NetworkBitStream &bs) override;

  bool onReceiveRPC(IPlayer &peer, int id, NetworkBitStream &bs) override;

  bool onSendPacket(IPlayer *peer, int id, NetworkBitStream &bs) override;

  bool onSendRPC(IPlayer *peer, int id, NetworkBitStream &bs) override;

  void onFree(IComponent *component) override;

  void reset() override;

  void free() override;

  static void PluginLogprintf(const char *fmt, ...);

  static ICore *&getCore();

  static PluginComponent *&get();

 private:
  ICore *core_{};
  IPawnComponent *pawn_component_{};

  void *plugin_data_[MAX_PLUGIN_DATA]{};
};

#endif  // PAWNRAKNET_MAIN_H_
