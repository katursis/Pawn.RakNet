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

// native PR_Init();
cell Script::PR_Init() {
  InitHandlers();

  return 1;
}

// native PR_RegHandler(id, const publicname[], PR_EventType:type);
cell Script::PR_RegHandler(int id, std::string public_name, PR_EventType type) {
  InitHandler(id, public_name, type);

  return 1;
}

// native PR_SendPacket(BitStream:bs, playerid, PR_PacketPriority:priority =
// PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED);
cell Script::PR_SendPacket(RakNet::BitStream *bs, int player_id,
                           PR_PacketPriority priority,
                           PR_PacketReliability reliability) {
  const bool broadcast = player_id == -1;

  return Plugin::Get().GetRakServer()->Send(
             bs, priority, reliability, '\0',
             broadcast ? UNASSIGNED_PLAYER_ID
                       : Plugin::Get().GetRakServer()->GetPlayerIDFromIndex(
                             player_id),
             broadcast)
             ? 1
             : 0;
}

// native PR_SendRPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority
// = PR_HIGH_PRIORITY, PR_PacketReliability:reliability =
// PR_RELIABLE_ORDERED);
cell Script::PR_SendRPC(RakNet::BitStream *bs, int player_id, int rpc_id,
                        PR_PacketPriority priority,
                        PR_PacketReliability reliability) {
  const bool broadcast = player_id == -1;

  return Plugin::Get().GetRakServer()->RPC(
             reinterpret_cast<RPCIndex *>(&rpc_id), bs, priority, reliability,
             '\0',
             broadcast ? UNASSIGNED_PLAYER_ID
                       : Plugin::Get().GetRakServer()->GetPlayerIDFromIndex(
                             player_id),
             broadcast, false)
             ? 1
             : 0;
}

// native PR_EmulateIncomingPacket(BitStream:bs, playerid);
cell Script::PR_EmulateIncomingPacket(RakNet::BitStream *bs, int player_id) {
  auto packet = Plugin::Get().NewPacket(player_id, *bs);

  Plugin::Get().PushPacketToEmulate(packet);

  return 1;
}

// native PR_EmulateIncomingRPC(BitStream:bs, playerid, rpcid);
cell Script::PR_EmulateIncomingRPC(RakNet::BitStream *bs, int player_id,
                                   int rpc_id) {
  const auto &handler = Plugin::Get().GetOriginalRPCHandler(rpc_id);
  if (!handler) {
    throw std::runtime_error{"Invalid rpcid"};
  }

  RPCParameters RPCParams{};
  RPCParams.numberOfBitsOfData = bs->GetNumberOfBitsUsed();
  RPCParams.sender =
      Plugin::Get().GetRakServer()->GetPlayerIDFromIndex(player_id);

  if (RPCParams.numberOfBitsOfData) {
    RPCParams.input = bs->GetData();
  }

  handler(&RPCParams);

  return 1;
}

// native BitStream:BS_New();
cell Script::BS_New() { return NewBitStream(); }

// native BitStream:BS_NewCopy(BitStream:bs);
cell Script::BS_NewCopy(RakNet::BitStream *bs) { return NewBitStreamCopy(bs); }

// native BS_Delete(&BitStream:bs);
cell Script::BS_Delete(cell *bs) {
  DeleteBitStream(*bs);

  *bs = 0;

  return 1;
}

// native BS_Reset(BitStream:bs);
cell Script::BS_Reset(RakNet::BitStream *bs) {
  bs->Reset();

  return 1;
}

// native BS_ResetReadPointer(BitStream:bs);
cell Script::BS_ResetReadPointer(RakNet::BitStream *bs) {
  bs->ResetReadPointer();

  return 1;
}

// native BS_ResetWritePointer(BitStream:bs);
cell Script::BS_ResetWritePointer(RakNet::BitStream *bs) {
  bs->ResetWritePointer();

  return 1;
}

// native BS_IgnoreBits(BitStream:bs, number_of_bits);
cell Script::BS_IgnoreBits(RakNet::BitStream *bs, int number_of_bits) {
  bs->IgnoreBits(number_of_bits);

  return 1;
}

// native BS_SetWriteOffset(BitStream:bs, offset);
cell Script::BS_SetWriteOffset(RakNet::BitStream *bs, int offset) {
  bs->SetWriteOffset(offset);

  return 1;
}

// native BS_GetWriteOffset(BitStream:bs, &offset);
cell Script::BS_GetWriteOffset(RakNet::BitStream *bs, cell *offset) {
  *offset = bs->GetWriteOffset();

  return 1;
}

// native BS_SetReadOffset(BitStream:bs, offset);
cell Script::BS_SetReadOffset(RakNet::BitStream *bs, int offset) {
  bs->SetReadOffset(offset);

  return 1;
}

// native BS_GetReadOffset(BitStream:bs, &offset);
cell Script::BS_GetReadOffset(RakNet::BitStream *bs, cell *offset) {
  *offset = bs->GetReadOffset();

  return 1;
}

// native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
cell Script::BS_GetNumberOfBitsUsed(RakNet::BitStream *bs, cell *number) {
  *number = bs->GetNumberOfBitsUsed();

  return 1;
}

// native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
cell Script::BS_GetNumberOfBytesUsed(RakNet::BitStream *bs, cell *number) {
  *number = bs->GetNumberOfBytesUsed();

  return 1;
}

// native BS_GetNumberOfUnreadBits(BitStream:bs, &number);
cell Script::BS_GetNumberOfUnreadBits(RakNet::BitStream *bs, cell *number) {
  *number = bs->GetNumberOfUnreadBits();

  return 1;
}

// native BS_GetNumberOfBitsAllocated(BitStream:bs, &number);
cell Script::BS_GetNumberOfBitsAllocated(RakNet::BitStream *bs, cell *number) {
  *number = bs->GetNumberOfBitsAllocated();

  return 1;
}

// native BS_WriteValue(BitStream:bs, {PR_ValueType, Float, _}:...);
cell Script::BS_WriteValue(cell *params) {
  AssertMinParams(3, params);

  const auto bs = GetBitStream(params[1]);

  for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
    const auto type = *GetPhysAddr(params[i + 1]);
    const auto &value = *GetPhysAddr(params[i + 2]);

    switch (type) {
      case PR_STRING:
      case PR_CSTRING: {
        auto str = GetString(params[i + 2]);

        if (type == PR_STRING) {
          bs->Write(str.c_str(), str.size());
        } else {
          stringCompressor->EncodeString(str.c_str(), str.size() + 1, bs);
        }

        break;
      }
      case PR_INT8:
        bs->Write(static_cast<char>(value));
        break;
      case PR_INT16:
        bs->Write(static_cast<short>(value));
        break;
      case PR_INT32:
        bs->Write(static_cast<int>(value));
        break;
      case PR_UINT8:
        bs->Write(static_cast<unsigned char>(value));
        break;
      case PR_UINT16:
        bs->Write(static_cast<unsigned short>(value));
        break;
      case PR_UINT32:
        bs->Write(static_cast<unsigned int>(value));
        break;
      case PR_FLOAT:
        bs->Write(amx_ctof(value));
        break;
      case PR_BOOL:
        bs->Write(!!value);
        break;
      case PR_CINT8:
        bs->WriteCompressed(static_cast<char>(value));
        break;
      case PR_CINT16:
        bs->WriteCompressed(static_cast<short>(value));
        break;
      case PR_CINT32:
        bs->WriteCompressed(static_cast<int>(value));
        break;
      case PR_CUINT8:
        bs->WriteCompressed(static_cast<unsigned char>(value));
        break;
      case PR_CUINT16:
        bs->WriteCompressed(static_cast<unsigned short>(value));
        break;
      case PR_CUINT32:
        bs->WriteCompressed(static_cast<unsigned int>(value));
        break;
      case PR_CFLOAT:
        bs->WriteCompressed(amx_ctof(value));
        break;
      case PR_CBOOL:
        bs->WriteCompressed(!!value);
        break;
      case PR_BITS: {
        const auto number_of_bits = *GetPhysAddr(params[i + 3]);
        if (number_of_bits <= 0 || number_of_bits > (sizeof(cell) * 8)) {
          throw std::runtime_error{"Invalid number of bits"};
        }

        bs->WriteBits(reinterpret_cast<const unsigned char *>(&value),
                      number_of_bits, true);

        ++i;

        break;
      }
      case PR_FLOAT3:
      case PR_FLOAT4: {
        const std::size_t arr_size = (type == PR_FLOAT3 ? 3 : 4);
        const auto arr = &value;

        for (std::size_t index{}; index < arr_size; ++index) {
          bs->Write(amx_ctof(arr[index]));
        }

        break;
      }
      case PR_VECTOR:
      case PR_NORM_QUAT: {
        const auto arr = reinterpret_cast<const float *>(&value);

        if (type == PR_VECTOR) {
          bs->WriteVector(arr[0], arr[1], arr[2]);
        } else {
          bs->WriteNormQuat(arr[0], arr[1], arr[2], arr[3]);
        }

        break;
      }
      case PR_STRING8:
      case PR_STRING32: {
        auto str = GetString(params[i + 2]);

        if (type == PR_STRING8) {
          bs->Write(static_cast<unsigned char>(str.size()));
        } else {
          bs->Write(static_cast<unsigned int>(str.size()));
        }

        bs->Write(str.c_str(), str.size());

        break;
      }
      case PR_IGNORE_BITS: {
        bs->SetWriteOffset(bs->GetWriteOffset() + value);
        break;
      }
      default: {
        throw std::runtime_error{"Invalid type of value"};
      }
    }
  }

  return 1;
}

template <typename T>
struct Value {
  static inline cell Read(RakNet::BitStream *bs) {
    T value{};

    bs->Read<T>(value);

    return static_cast<cell>(value);
  }

  static inline cell ReadCompressed(RakNet::BitStream *bs) {
    T value{};

    bs->ReadCompressed<T>(value);

    return static_cast<cell>(value);
  }
};

template <>
struct Value<float> {
  static inline cell Read(RakNet::BitStream *bs) {
    float value{};

    bs->Read<float>(value);

    return amx_ftoc(value);
  }

  static inline cell ReadCompressed(RakNet::BitStream *bs) {
    float value{};

    bs->ReadCompressed<float>(value);

    return amx_ftoc(value);
  }
};

// native BS_ReadValue(BitStream:bs, {PR_ValueType, Float, _}:...);
cell Script::BS_ReadValue(cell *params) {
  AssertMinParams(3, params);

  const auto bs = GetBitStream(params[1]);

  for (std::size_t i = 1; i < (params[0] / sizeof(cell)) - 1; i += 2) {
    const auto type = *GetPhysAddr(params[i + 1]);
    auto &value = *GetPhysAddr(params[i + 2]);

    switch (type) {
      case PR_STRING:
      case PR_CSTRING: {
        const auto size = *GetPhysAddr(params[i + 3]);

        std::unique_ptr<char[]> str{new char[size + 1]{}};

        if (type == PR_STRING) {
          bs->Read(str.get(), size);
        } else {
          stringCompressor->DecodeString(str.get(), size, bs);
        }

        SetString(&value, str.get(), size + 1);

        ++i;

        break;
      }
      case PR_INT8:
        value = Value<char>::Read(bs);
        break;
      case PR_INT16:
        value = Value<short>::Read(bs);
        break;
      case PR_INT32:
        value = Value<int>::Read(bs);
        break;
      case PR_UINT8:
        value = Value<unsigned char>::Read(bs);
        break;
      case PR_UINT16:
        value = Value<unsigned short>::Read(bs);
        break;
      case PR_UINT32:
        value = Value<unsigned int>::Read(bs);
        break;
      case PR_FLOAT:
        value = Value<float>::Read(bs);
        break;
      case PR_BOOL:
        value = Value<bool>::Read(bs);
        break;
      case PR_CINT8:
        value = Value<char>::ReadCompressed(bs);
        break;
      case PR_CINT16:
        value = Value<short>::ReadCompressed(bs);
        break;
      case PR_CINT32:
        value = Value<int>::ReadCompressed(bs);
        break;
      case PR_CUINT8:
        value = Value<unsigned char>::ReadCompressed(bs);
        break;
      case PR_CUINT16:
        value = Value<unsigned short>::ReadCompressed(bs);
        break;
      case PR_CUINT32:
        value = Value<unsigned int>::ReadCompressed(bs);
        break;
      case PR_CFLOAT:
        value = Value<float>::ReadCompressed(bs);
        break;
      case PR_CBOOL:
        value = Value<bool>::ReadCompressed(bs);
        break;
      case PR_BITS: {
        const auto number_of_bits = *GetPhysAddr(params[i + 3]);
        if (number_of_bits <= 0 || number_of_bits > (sizeof(cell) * 8)) {
          throw std::runtime_error{"Invalid number of bits"};
        }

        bs->ReadBits(reinterpret_cast<unsigned char *>(&value), number_of_bits,
                     true);

        ++i;

        break;
      }
      case PR_FLOAT3:
      case PR_FLOAT4: {
        const std::size_t arr_size = (type == PR_FLOAT3 ? 3 : 4);
        auto arr = &value;

        for (std::size_t index{}; index < arr_size; ++index) {
          arr[index] = Value<float>::Read(bs);
        }

        break;
      }
      case PR_VECTOR:
      case PR_NORM_QUAT: {
        auto arr = reinterpret_cast<float *>(&value);

        if (type == PR_VECTOR) {
          bs->ReadVector(arr[0], arr[1], arr[2]);
        } else {
          bs->ReadNormQuat(arr[0], arr[1], arr[2], arr[3]);
        }

        break;
      }
      case PR_STRING8:
      case PR_STRING32: {
        const auto max_size = *GetPhysAddr(params[i + 3]) - 1;

        cell size{};

        if (type == PR_STRING8) {
          size = Value<unsigned char>::Read(bs);
        } else {
          size = Value<unsigned int>::Read(bs);
        }

        if (size > 0) {
          if (size > max_size) {
            Log("%s: Warning! size (%d) > max_size (%d) "
                "(PR_STRING8/PR_STRING32)",
                __FUNCTION__, size, max_size);

            size = max_size;
          }

          std::unique_ptr<char[]> str{new char[size + 1]{}};

          bs->Read(str.get(), size);

          SetString(&value, str.get(), size + 1);
        }

        ++i;

        break;
      }
      case PR_IGNORE_BITS: {
        bs->IgnoreBits(value);
        break;
      }
      default: {
        throw std::runtime_error{"Invalid type of value"};
      }
    }
  }

  return 1;
}

bool Script::OnLoad() {
  int num_publics{};
  amx_->NumPublics(&num_publics);

  for (int index{}; index < num_publics; index++) {
    std::string public_name = GetPublicName(index);
    if (std::regex_match(public_name, regex_reg_handler_public_name_)) {
      publics_reg_handler_.push_back(MakePublic(public_name));
    } else if (public_name == "OnIncomingPacket") {
      InitPublic(PR_INCOMING_PACKET, public_name);
    } else if (public_name == "OnIncomingRPC") {
      InitPublic(PR_INCOMING_RPC, public_name);
    } else if (public_name == "OnOutcomingPacket") {
      InitPublic(PR_OUTCOMING_PACKET, public_name);
    } else if (public_name == "OnOutcomingRPC") {
      InitPublic(PR_OUTCOMING_RPC, public_name);
    } else if (public_name == "OnIncomingRawPacket") {
      InitPublic(PR_INCOMING_RAW_PACKET, public_name);
    }
  }

  return true;
}

bool Script::OnEvent(PR_EventType event_type, int player_id, int id,
                     RakNet::BitStream *bs) {
  const auto &pub = publics_[event_type];
  if (pub && pub->Exists()) {
    bs->ResetReadPointer();

    if (!pub->Exec(player_id, id, bs)) {
      return false;
    }
  }

  for (const auto &handler : handlers_[event_type][id]) {
    bs->ResetReadPointer();

    if (!handler->Exec(player_id, bs)) {
      return false;
    }
  }

  return true;
}

void Script::InitPublic(PR_EventType type, const std::string &public_name) {
  publics_.at(type) =
      MakePublic(public_name, Plugin::Get().GetConfig()->UseCaching());
}

void Script::InitHandler(int id, const std::string &public_name,
                         PR_EventType type) {
  auto pub = MakePublic(public_name, Plugin::Get().GetConfig()->UseCaching());
  if (!pub->Exists()) {
    throw std::runtime_error{"Public " + public_name + " does not exist"};
  }

  handlers_.at(type).at(id).push_back(pub);
}

void Script::InitHandlers() {
  for (const auto &pub : publics_reg_handler_) {
    if (pub && pub->Exists()) {
      pub->Exec();
    }
  }
}

cell Script::NewBitStream() {
  const auto bs = std::make_shared<RakNet::BitStream>();

  bitstreams_.insert(bs);

  return reinterpret_cast<cell>(bs.get());
}

cell Script::NewBitStreamCopy(RakNet::BitStream *src) {
  const auto bs = std::make_shared<RakNet::BitStream>(
      src->GetData(), src->GetNumberOfBytesUsed(), true);

  bitstreams_.insert(bs);

  return reinterpret_cast<cell>(bs.get());
}

RakNet::BitStream *Script::GetBitStream(cell handle) {
  const auto bs = reinterpret_cast<RakNet::BitStream *>(handle);
  if (!bs) {
    throw std::runtime_error{"Invalid BitStream handle"};
  }

  return bs;
}

std::unordered_set<std::shared_ptr<RakNet::BitStream>>::iterator
Script::FindBitStream(cell handle) {
  const auto bs =
      std::find_if(bitstreams_.begin(), bitstreams_.end(), [=](const auto &bs) {
        return reinterpret_cast<cell>(bs.get()) == handle;
      });
  if (bs == bitstreams_.end()) {
    throw std::runtime_error{"Invalid BitStream handle"};
  }

  return bs;
}

void Script::DeleteBitStream(cell handle) {
  bitstreams_.erase(FindBitStream(handle));
}