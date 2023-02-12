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

#ifndef PAWNRAKNET_CONFIG_H_
#define PAWNRAKNET_CONFIG_H_

class Config {
 public:
  Config() = delete;

  explicit Config(const std::string &file_path);

  void Read();

  void Save();

  bool InterceptIncomingPacket() const;

  bool InterceptIncomingRPC() const;

  bool InterceptOutgoingPacket() const;

  bool InterceptOutgoingRPC() const;

  bool UseCaching() const;

  bool LogAmxErrors() const;

 private:
  std::string file_path_;

  bool intercept_incoming_packet_{};
  bool intercept_incoming_rpc_{};
  bool intercept_outgoing_packet_{};
  bool intercept_outgoing_rpc_{};

  bool use_caching_{};
  bool log_amx_errors_{};
};

#endif  // PAWNRAKNET_CONFIG_H_
