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

Config::Config(const std::string &file_path) : file_path_{file_path} {}

void Config::Read() {
  std::fstream{file_path_, std::fstream::out | std::fstream::app};

  const auto config = cpptoml::parse_file(file_path_);

  // backward compatibility
  bool intercept_outcoming_packet =
      config->get_as<bool>("InterceptOutcomingPacket").value_or(true);
  bool intercept_outcoming_rpc =
      config->get_as<bool>("InterceptOutcomingRPC").value_or(true);

  intercept_incoming_packet_ =
      config->get_as<bool>("InterceptIncomingPacket").value_or(true);
  intercept_incoming_rpc_ =
      config->get_as<bool>("InterceptIncomingRPC").value_or(true);
  intercept_outgoing_packet_ = config->get_as<bool>("InterceptOutgoingPacket")
                                   .value_or(intercept_outcoming_packet);
  intercept_outgoing_rpc_ = config->get_as<bool>("InterceptOutgoingRPC")
                                .value_or(intercept_outcoming_rpc);
  intercept_incoming_raw_packet_ =
      config->get_as<bool>("InterceptIncomingRawPacket").value_or(true);
  intercept_incoming_internal_packet_ =
      config->get_as<bool>("InterceptIncomingInternalPacket").value_or(false);
  intercept_outgoing_internal_packet_ =
      config->get_as<bool>("InterceptOutgoingInternalPacket").value_or(false);

  auto packet_ids = config->get_array_of<int64_t>("WhiteListInternalPackets")
                        .value_or(std::vector<int64_t>{});
  for (auto &packet_id : packet_ids) {
    if (packet_id < 0 ||
        packet_id > (std::numeric_limits<unsigned char>::max)()) {
      continue;
    }

    whitelist_internal_packets_[static_cast<unsigned char>(packet_id)] = true;

    whitelist_is_empty_ = false;
  }

  use_caching_ = config->get_as<bool>("UseCaching").value_or(false);
}

void Config::Save() {
  auto config = cpptoml::make_table();

  config->insert("InterceptIncomingPacket", intercept_incoming_packet_);
  config->insert("InterceptIncomingRPC", intercept_incoming_rpc_);
  config->insert("InterceptOutgoingPacket", intercept_outgoing_packet_);
  config->insert("InterceptOutgoingRPC", intercept_outgoing_rpc_);
  config->insert("InterceptIncomingRawPacket", intercept_incoming_raw_packet_);
  config->insert("InterceptIncomingInternalPacket",
                 intercept_incoming_internal_packet_);
  config->insert("InterceptOutgoingInternalPacket",
                 intercept_outgoing_internal_packet_);

  auto packet_ids = cpptoml::make_array();
  if (!whitelist_is_empty_) {
    for (std::size_t packet_id{};
         packet_id < whitelist_internal_packets_.size(); packet_id++) {
      if (whitelist_internal_packets_[packet_id]) {
        packet_ids->push_back(packet_id);
      }
    }
  }
  config->insert("WhiteListInternalPackets", packet_ids);

  config->insert("UseCaching", use_caching_);

  std::fstream{file_path_, std::fstream::out | std::fstream::trunc}
      << (*config);
}

bool Config::InterceptIncomingPacket() const {
  return intercept_incoming_packet_;
}

bool Config::InterceptIncomingRPC() const { return intercept_incoming_rpc_; }

bool Config::InterceptOutgoingPacket() const {
  return intercept_outgoing_packet_;
}

bool Config::InterceptOutgoingRPC() const { return intercept_outgoing_rpc_; }

bool Config::InterceptIncomingRawPacket() const {
  return intercept_incoming_raw_packet_;
}

bool Config::InterceptIncomingInternalPacket() const {
  return intercept_incoming_internal_packet_;
}

bool Config::InterceptOutgoingInternalPacket() const {
  return intercept_outgoing_internal_packet_;
}

bool Config::IsWhiteListedInternalPacket(unsigned char packet_id) const {
  return whitelist_is_empty_ || whitelist_internal_packets_[packet_id];
}

bool Config::UseCaching() const { return use_caching_; }
