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

void InternalPacketChannel::PushPacket(InternalPacket *packet,
                                       const PlayerID &player_id,
                                       bool is_outgoing_packet) {
  packet_ = packet;
  player_id_ = player_id;
  is_outgoing_packet_ = is_outgoing_packet;

  packet_is_ready_ = true;
}

InternalPacket *InternalPacketChannel::TryPopPacket() {
  if (!packet_is_ready_) {
    return nullptr;
  }

  packet_is_ready_ = false;

  return packet_;
}

const PlayerID &InternalPacketChannel::GetPlayerId() { return player_id_; }

bool InternalPacketChannel::IsOutgoingPacket() { return is_outgoing_packet_; }

void InternalPacketChannel::PushResult(bool result) {
  result_ = result;

  result_is_ready_ = true;
}

bool InternalPacketChannel::PopResult() {
  while (!result_is_ready_) {
    if (is_closed_) {
      return true;
    }

    std::this_thread::yield();
  }

  result_is_ready_ = false;

  return result_;
}

void InternalPacketChannel::Open() { is_closed_ = false; }

void InternalPacketChannel::Close() { is_closed_ = true; }

bool InternalPacketChannel::IsClosed() { return is_closed_; };
