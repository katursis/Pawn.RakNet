/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 katursis
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

#ifndef PAWNRAKNET_INTERNAL_PACKET_CHANNEL_H_
#define PAWNRAKNET_INTERNAL_PACKET_CHANNEL_H_

class InternalPacketChannel {
 public:
  void PushPacket(InternalPacket *packet, const PlayerID &player_id,
                  bool is_outgoing_packet);

  InternalPacket *TryPopPacket();

  const PlayerID &GetPlayerId();

  bool IsOutgoingPacket();

  void PushResult(bool result);

  bool PopResult();

  void Open();

  void Close();

  bool IsClosed();

 private:
  std::atomic_bool packet_is_ready_{false};  // ready for consumer
  std::atomic_bool result_is_ready_{false};  // ready for producer
  std::atomic_bool is_closed_{false};

  InternalPacket *packet_{};
  PlayerID player_id_{};
  bool is_outgoing_packet_{};

  bool result_{};
};

#endif  // PAWNRAKNET_INTERNAL_PACKET_CHANNEL_H_
