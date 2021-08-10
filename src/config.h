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

#ifndef PAWNRAKNET_CONFIG_H_
#define PAWNRAKNET_CONFIG_H_

class Config {
 public:
  Config() = delete;

  Config(const std::string &file_path, bool read = false)
      : file_path_{file_path} {
    if (read) {
      Read();
    }
  }

  static std::shared_ptr<Config> New(const std::string &file_path,
                                     bool read = false) {
    return std::make_shared<Config>(file_path, read);
  }

  void Read() {
    std::fstream{file_path_, std::fstream::out | std::fstream::app};

    const auto config = cpptoml::parse_file(file_path_);

    intercept_incoming_packet_ =
        config->get_as<bool>("InterceptIncomingPacket").value_or(true);
    intercept_incoming_rpc_ =
        config->get_as<bool>("InterceptIncomingRPC").value_or(true);
    intercept_outcoming_packet_ =
        config->get_as<bool>("InterceptOutcomingPacket").value_or(true);
    intercept_outcoming_rpc_ =
        config->get_as<bool>("InterceptOutcomingRPC").value_or(true);
    intercept_incoming_raw_packet_ =
        config->get_as<bool>("InterceptIncomingRawPacket").value_or(true);
    use_caching_ = config->get_as<bool>("UseCaching").value_or(false);
  }

  void Save() {
    auto config = cpptoml::make_table();

    config->insert("InterceptIncomingPacket", intercept_incoming_packet_);
    config->insert("InterceptIncomingRPC", intercept_incoming_rpc_);
    config->insert("InterceptOutcomingPacket", intercept_outcoming_packet_);
    config->insert("InterceptOutcomingRPC", intercept_outcoming_rpc_);
    config->insert("InterceptIncomingRawPacket",
                   intercept_incoming_raw_packet_);
    config->insert("UseCaching", use_caching_);

    std::fstream{file_path_, std::fstream::out | std::fstream::trunc}
        << (*config);
  }

  bool InterceptIncomingPacket() const { return intercept_incoming_packet_; }

  bool InterceptIncomingRPC() const { return intercept_incoming_rpc_; }

  bool InterceptOutcomingPacket() const { return intercept_outcoming_packet_; }

  bool InterceptOutcomingRPC() const { return intercept_outcoming_rpc_; }

  bool InterceptIncomingRawPacket() const {
    return intercept_incoming_raw_packet_;
  }

  bool UseCaching() const { return use_caching_; }

 private:
  std::string file_path_;

  bool intercept_incoming_packet_{};
  bool intercept_incoming_rpc_{};
  bool intercept_outcoming_packet_{};
  bool intercept_outcoming_rpc_{};
  bool intercept_incoming_raw_packet_{};
  bool use_caching_{};
};

#endif  // PAWNRAKNET_CONFIG_H_
