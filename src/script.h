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

#ifndef PAWNRAKNET_SCRIPT_H_
#define PAWNRAKNET_SCRIPT_H_

using PublicPtr = std::shared_ptr<ptl::Public>;

class Script : public ptl::AbstractScript<Script> {
 public:
  const char *VarIsGamemode() { return "_pawnraknet_is_gamemode"; }

  const char *VarVersion() { return "_pawnraknet_version"; }

  // native PR_Init();
  cell PR_Init();

  // native PR_RegHandler(eventid, const publicname[], PR_EventType:type);
  cell PR_RegHandler(unsigned char event_id, std::string public_name,
                     PR_EventType type);

  // native PR_SendPacket(BitStream:bs, playerid, PR_PacketPriority:priority =
  // PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED,
  // orderingchannel = 0);
  cell PR_SendPacket(BitStream *bs, int player_id, PR_PacketPriority priority,
                     PR_PacketReliability reliability,
                     unsigned char ordering_channel);

  // native PR_SendRPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority
  // = PR_HIGH_PRIORITY, PR_PacketReliability:reliability =
  // PR_RELIABLE_ORDERED, orderingchannel = 0);
  cell PR_SendRPC(BitStream *bs, int player_id, RPCIndex rpc_id,
                  PR_PacketPriority priority, PR_PacketReliability reliability,
                  unsigned char ordering_channel);

  // native PR_EmulateIncomingPacket(BitStream:bs, playerid);
  cell PR_EmulateIncomingPacket(BitStream *bs, int player_id);

  // native PR_EmulateIncomingRPC(BitStream:bs, playerid, rpcid);
  cell PR_EmulateIncomingRPC(BitStream *bs, int player_id, RPCIndex rpc_id);

  // native BitStream:BS_New();
  cell BS_New();

  // native BitStream:BS_NewCopy(BitStream:bs);
  cell BS_NewCopy(BitStream *bs);

  // native BS_Delete(&BitStream:bs);
  cell BS_Delete(cell *bs);

  // native BS_Reset(BitStream:bs);
  cell BS_Reset(BitStream *bs);

  // native BS_ResetReadPointer(BitStream:bs);
  cell BS_ResetReadPointer(BitStream *bs);

  // native BS_ResetWritePointer(BitStream:bs);
  cell BS_ResetWritePointer(BitStream *bs);

  // native BS_IgnoreBits(BitStream:bs, number_of_bits);
  cell BS_IgnoreBits(BitStream *bs, int number_of_bits);

  // native BS_SetWriteOffset(BitStream:bs, offset);
  cell BS_SetWriteOffset(BitStream *bs, int offset);

  // native BS_GetWriteOffset(BitStream:bs, &offset);
  cell BS_GetWriteOffset(BitStream *bs, cell *offset);

  // native BS_SetReadOffset(BitStream:bs, offset);
  cell BS_SetReadOffset(BitStream *bs, int offset);

  // native BS_GetReadOffset(BitStream:bs, &offset);
  cell BS_GetReadOffset(BitStream *bs, cell *offset);

  // native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
  cell BS_GetNumberOfBitsUsed(BitStream *bs, cell *number);

  // native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
  cell BS_GetNumberOfBytesUsed(BitStream *bs, cell *number);

  // native BS_GetNumberOfUnreadBits(BitStream:bs, &number);
  cell BS_GetNumberOfUnreadBits(BitStream *bs, cell *number);

  // native BS_GetNumberOfBitsAllocated(BitStream:bs, &number);
  cell BS_GetNumberOfBitsAllocated(BitStream *bs, cell *number);

  // native BS_WriteValue(BitStream:bs, {PR_ValueType, Float, _}:...);
  cell BS_WriteValue(cell *params);

  // native BS_ReadValue(BitStream:bs, {PR_ValueType, Float, _}:...);
  cell BS_ReadValue(cell *params);

  bool OnLoad();

  template <PR_EventType event_type>
  bool OnEvent(int player_id, unsigned char event_id, BitStream *bs) {
    if constexpr (event_type == PR_OUTGOING_PACKET) {
      if (!ExecPublic(public_on_outcoming_packet_, player_id, event_id, bs)) {
        return false;
      }
    } else if constexpr (event_type == PR_OUTGOING_RPC) {
      if (!ExecPublic(public_on_outcoming_rpc_, player_id, event_id, bs)) {
        return false;
      }
    }

    if constexpr (event_type != PR_INCOMING_CUSTOM_RPC) {
      if (!ExecPublic(std::get<event_type>(publics_), player_id, event_id,
                      bs)) {
        return false;
      }
    }

    for (const auto &handler : std::get<event_type>(handlers_).at(event_id)) {
      bs->resetReadPointer();

      if (!handler->Exec(player_id, bs)) {
        return false;
      }
    }

    bs->resetReadPointer();

    return true;
  }

  bool ExecPublic(const PublicPtr &pub, int player_id, unsigned char event_id,
                  BitStream *bs);

  void InitPublic(PR_EventType type, const std::string &public_name);

  void InitHandler(unsigned char event_id, const std::string &public_name,
                   PR_EventType type);

  void InitHandlers();

  BitStream *GetBitStream(cell handle);

  template <typename T, bool compressed = false>
  void WriteValue(BitStream *bs, cell value);

  template <typename T, bool compressed = false>
  cell ReadValue(BitStream *bs);

 private:
  const std::regex regex_reg_handler_public_name_{
      R"(^pr_r(?:ip|ir|op|or|irp|iip|oip|icr)_\w+$)"};

  std::shared_ptr<Config> config_;

  std::list<PublicPtr> publics_reg_handler_;

  std::array<PublicPtr, PR_NUMBER_OF_EVENT_TYPES> publics_;
  std::array<std::array<std::list<PublicPtr>, PR_MAX_HANDLERS>,
             PR_NUMBER_OF_EVENT_TYPES>
      handlers_;

  // backward compatibility
  PublicPtr public_on_outcoming_packet_;
  PublicPtr public_on_outcoming_rpc_;

  BitStreamPool bitstream_pool_;
};

#endif  // PAWNRAKNET_SCRIPT_H_
