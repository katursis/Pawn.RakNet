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

bool Plugin::OnLoad() {
  config_ = Config::New("plugins/pawnraknet.cfg", true);

  StringCompressor::AddReference();

  InstallPreHooks();

  RegisterNative<&Script::PR_Init>("PR_Init");
  RegisterNative<&Script::PR_RegHandler>("PR_RegHandler");
  RegisterNative<&Script::PR_SendPacket>("PR_SendPacket");
  RegisterNative<&Script::PR_SendRPC>("PR_SendRPC");
  RegisterNative<&Script::PR_EmulateIncomingPacket>("PR_EmulateIncomingPacket");
  RegisterNative<&Script::PR_EmulateIncomingRPC>("PR_EmulateIncomingRPC");

  RegisterNative<&Script::BS_New>("BS_New");
  RegisterNative<&Script::BS_NewCopy>("BS_NewCopy");
  RegisterNative<&Script::BS_Delete>("BS_Delete");
  RegisterNative<&Script::BS_Reset>("BS_Reset");
  RegisterNative<&Script::BS_ResetReadPointer>("BS_ResetReadPointer");
  RegisterNative<&Script::BS_ResetWritePointer>("BS_ResetWritePointer");
  RegisterNative<&Script::BS_IgnoreBits>("BS_IgnoreBits");
  RegisterNative<&Script::BS_SetWriteOffset>("BS_SetWriteOffset");
  RegisterNative<&Script::BS_GetWriteOffset>("BS_GetWriteOffset");
  RegisterNative<&Script::BS_SetReadOffset>("BS_SetReadOffset");
  RegisterNative<&Script::BS_GetReadOffset>("BS_GetReadOffset");
  RegisterNative<&Script::BS_GetNumberOfBitsUsed>("BS_GetNumberOfBitsUsed");
  RegisterNative<&Script::BS_GetNumberOfBytesUsed>("BS_GetNumberOfBytesUsed");
  RegisterNative<&Script::BS_GetNumberOfUnreadBits>("BS_GetNumberOfUnreadBits");
  RegisterNative<&Script::BS_GetNumberOfBitsAllocated>(
      "BS_GetNumberOfBitsAllocated");
  RegisterNative<&Script::BS_WriteValue, false>("BS_WriteValue");
  RegisterNative<&Script::BS_ReadValue, false>("BS_ReadValue");

  Log("\n\n"
      "    | %s %s | 2016 - %s"
      "\n"
      "    |--------------------------------"
      "\n"
      "    | Author and maintainer: katursis"
      "\n\n\n"
      "    | Compiled: %s at %s"
      "\n"
      "    |--------------------------------------------------------------"
      "\n"
      "    | Repository: https://github.com/katursis/%s"
      "\n"
      "    |--------------------------------------------------------------"
      "\n"
      "    | Wiki: https://github.com/katursis/%s/wiki"
      "\n",
      Name(), VersionAsString().c_str(), &__DATE__[7], __DATE__, __TIME__,
      Name(), Name());

  return true;
}

void Plugin::OnUnload() {
  config_->Save();

  StringCompressor::RemoveReference();

  Log("plugin unloaded");
}

void Plugin::InstallPreHooks() {
  urmem::sig_scanner scanner;

  if (!scanner.init(reinterpret_cast<urmem::address_t>(*plugin_data_))) {
    throw std::runtime_error{"Sig scanner init error"};
  }

  urmem::address_t get_rakserver_interface_addr{};

  if (!scanner.find(get_rakserver_interface_pattern_,
                    get_rakserver_interface_mask_,
                    get_rakserver_interface_addr)) {
    throw std::runtime_error{"GetRakServerInterface not found"};
  }

  if (!scanner.find(get_packet_id_pattern_, get_packet_id_mask_,
                    addr_get_packet_id_)) {
    throw std::runtime_error{"GetPacketId not found"};
  }

  hook_get_rakserver_interface_ = urmem::hook::make(
      get_rakserver_interface_addr, &Hooks::GetRakServerInterface);

  hook_amx_cleanup_ = urmem::hook::make(
      reinterpret_cast<urmem::address_t *>(
          plugin_data_[PLUGIN_DATA_AMX_EXPORTS])[PLUGIN_AMX_EXPORT_Cleanup],
      &Hooks::amx_Cleanup);
}

void Plugin::InstallRakServerHooks(urmem::address_t addr_rakserver) {
  rakserver_ = std::make_shared<RakServer>(addr_rakserver);

  if (config_->InterceptIncomingPacket()) {
    rakserver_->InstallHook(RakServer::MethodIndex::kReceive,
                            &Hooks::RakServer__Receive);
  }

  if (config_->InterceptIncomingRPC()) {
    rakserver_->InstallHook(
        RakServer::MethodIndex::kRegisterAsRemoteProcedureCall,
        &Hooks::RakServer__RegisterAsRemoteProcedureCall);

    Hooks::ReceiveRPC::Init();
  }

  if (config_->InterceptOutcomingPacket()) {
    rakserver_->InstallHook(RakServer::MethodIndex::kSend,
                            &Hooks::RakServer__Send);
  }

  if (config_->InterceptOutcomingRPC()) {
    rakserver_->InstallHook(RakServer::MethodIndex::kRPC,
                            &Hooks::RakServer__RPC);
  }

  if (config_->InterceptIncomingRawPacket()) {
    message_handler_ = std::make_shared<MessageHandler>();

    rakserver_->AttachPlugin(message_handler_.get());
  }
}

int Plugin::GetPacketId(Packet *packet) {
  return urmem::call_function<urmem::calling_convention::cdeclcall,
                              unsigned char>(addr_get_packet_id_, packet);
}

Packet *Plugin::NewPacket(PlayerIndex index, const RakNet::BitStream &bs) {
  const std::size_t length = bs.GetNumberOfBytesUsed();
  if (!length) {
    throw std::runtime_error{"Data is empty"};
  }

  Packet *p = reinterpret_cast<Packet *>(malloc(sizeof(Packet) + length));

  p->playerIndex = index;
  p->playerId = GetRakServer()->GetPlayerIDFromIndex(index);
  p->length = length;
  p->bitSize = bs.GetNumberOfBitsUsed();
  p->data = reinterpret_cast<unsigned char *>(&p[1]);
  memcpy(p->data, bs.GetData(), length);
  p->deleteData = false;

  return p;
}

void Plugin::PushPacketToEmulate(Packet *packet) {
  emulating_packets_.push(packet);
}

Packet *Plugin::GetNextPacketToEmulate() {
  if (emulating_packets_.empty()) {
    return nullptr;
  }

  auto packet = emulating_packets_.front();

  emulating_packets_.pop();

  return packet;
}

void Plugin::SetOriginalRPCHandler(int rpc_id, RPCFunction handler) {
  original_rpc_.at(rpc_id) = handler;
}

RPCFunction Plugin::GetOriginalRPCHandler(int rpc_id) {
  return original_rpc_.at(rpc_id);
}

void Plugin::SetFakeRPCHandler(int rpc_id, RPCFunction handler) {
  fake_rpc_.at(rpc_id) = handler;
}

RPCFunction Plugin::GetFakeRPCHandler(int rpc_id) {
  return fake_rpc_.at(rpc_id);
}

const std::shared_ptr<urmem::hook> &Plugin::GetHookGetRakServerInterface() {
  return hook_get_rakserver_interface_;
}

const std::shared_ptr<urmem::hook> &Plugin::GetHookAmxCleanup() {
  return hook_amx_cleanup_;
}

const std::shared_ptr<Config> &Plugin::GetConfig() { return config_; }

const std::shared_ptr<RakServer> &Plugin::GetRakServer() { return rakserver_; }

bool Plugin::OnEvent(PR_EventType event_type, int player_id, int id,
                     RakNet::BitStream *bs) {
  return EveryScript([=](const std::shared_ptr<Script> &script) {
    return script->OnEvent(event_type, player_id, id, bs);
  });
}
