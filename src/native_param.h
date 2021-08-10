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

#ifndef PAWNRAKNET_NATIVE_PARAM_H_
#define PAWNRAKNET_NATIVE_PARAM_H_

struct NativeParam : Script::NativeParam {
  operator RakNet::BitStream*() { return script.GetBitStream(raw_value); }

  operator PR_PacketPriority() {
    return static_cast<PR_PacketPriority>(raw_value);
  }

  operator PR_PacketReliability() {
    return static_cast<PR_PacketReliability>(raw_value);
  }

  operator PR_EventType() { return static_cast<PR_EventType>(raw_value); }
};

#endif  // PAWNRAKNET_NATIVE_PARAM_H_
