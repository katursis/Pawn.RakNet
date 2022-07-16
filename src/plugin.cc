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

#include "main.h"

bool Plugin::OnLoad() {
  config_ = std::make_shared<Config>("components/pawnraknet.cfg");

  config_->Read();

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
      "    | %s %s | omp-beta7 | 2016 - %s"
      "\n"
      "    |--------------------------------------------"
      "\n"
      "    | Author and maintainer: katursis"
      "\n\n\n"
      "    | Compiled: %s at %s"
      "\n"
      "    |--------------------------------------------------------------"
      "\n"
      "    | Repository: https://github.com/katursis/%s/tree/omp"
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

  Log("plugin unloaded");
}

void Plugin::OnProcessTick() {}

void Plugin::InstallPreHooks() {
  hook_amx_cleanup_ = urmem::hook::make(
      reinterpret_cast<urmem::address_t *>(
          plugin_data_[PLUGIN_DATA_AMX_EXPORTS])[PLUGIN_AMX_EXPORT_Cleanup],
      &Hooks::amx_Cleanup);
}

void Plugin::SetCustomRPC(RPCIndex rpc_id) { custom_rpc_[rpc_id] = true; }

bool Plugin::IsCustomRPC(RPCIndex rpc_id) { return custom_rpc_[rpc_id]; }

const std::shared_ptr<urmem::hook> &Plugin::GetHookAmxCleanup() {
  return hook_amx_cleanup_;
}

const std::shared_ptr<Config> &Plugin::GetConfig() { return config_; }
