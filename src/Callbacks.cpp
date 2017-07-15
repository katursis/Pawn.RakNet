#include "Main.h"

// forward OnPlayerReceivedPacket(player_id, packet_id, BitStream:bs);
bool Callbacks::OnPlayerReceivedPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_player_received_packet;

    if (public_data.exists) {
      if (bs) {
        bs->ResetReadPointer();
      }

      amx_Push(amx, reinterpret_cast<cell>(bs));
      amx_Push(amx, static_cast<cell>(packet_id));
      amx_Push(amx, static_cast<cell>(player_id));

      amx_Exec(amx, &retval, public_data.id);

      if (retval == 0) {
        return false;
      }
    }
  }

  return true;
}

// forward OnPlayerReceivedRPC(player_id, rpc_id, BitStream:bs);
bool Callbacks::OnPlayerReceivedRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_player_received_rpc;

    if (public_data.exists) {
      if (bs) {
        bs->ResetReadPointer();
      }

      amx_Push(amx, reinterpret_cast<cell>(bs));
      amx_Push(amx, static_cast<cell>(rpc_id));
      amx_Push(amx, static_cast<cell>(player_id));

      amx_Exec(amx, &retval, public_data.id);

      if (retval == 0) {
        return false;
      }
    }
  }

  return true;
}

// forward OnServerSendPacket(player_id, packet_id, BitStream:bs);
bool Callbacks::OnServerSendPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_server_send_packet;

    if (public_data.exists) {
      if (bs) {
        bs->ResetReadPointer();
      }

      amx_Push(amx, reinterpret_cast<cell>(bs));
      amx_Push(amx, static_cast<cell>(packet_id));
      amx_Push(amx, static_cast<cell>(player_id));

      amx_Exec(amx, &retval, public_data.id);

      if (retval == 0) {
        return false;
      }
    }
  }

  return true;
}

// forward OnServerSendRPC(player_id, rpc_id, BitStream:bs);
bool Callbacks::OnServerSendRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_server_send_rpc;

    if (public_data.exists) {
      if (bs) {
        bs->ResetReadPointer();
      }

      amx_Push(amx, reinterpret_cast<cell>(bs));
      amx_Push(amx, static_cast<cell>(rpc_id));
      amx_Push(amx, static_cast<cell>(player_id));

      amx_Exec(amx, &retval, public_data.id);

      if (retval == 0) {
        return false;
      }
    }
  }

  return true;
}

void Callbacks::OnAmxLoad(AMX *amx) {
  Data data{};

  if (amx_FindPublic(amx, "OnPlayerReceivedPacket", &data._public_on_player_received_packet.id) == AMX_ERR_NONE) {
    data._public_on_player_received_packet.exists = true;
  }

  if (amx_FindPublic(amx, "OnPlayerReceivedRPC", &data._public_on_player_received_rpc.id) == AMX_ERR_NONE) {
    data._public_on_player_received_rpc.exists = true;
  }

  if (amx_FindPublic(amx, "OnServerSendPacket", &data._public_on_server_send_packet.id) == AMX_ERR_NONE) {
    data._public_on_server_send_packet.exists = true;
  }

  if (amx_FindPublic(amx, "OnServerSendRPC", &data._public_on_server_send_rpc.id) == AMX_ERR_NONE) {
    data._public_on_server_send_rpc.exists = true;
  }

  Callbacks::GetAmxMap()[amx] = data;
}

void Callbacks::OnAmxUnload(AMX *amx) {
  Callbacks::GetAmxMap().erase(amx);
}

std::unordered_map<AMX *, Callbacks::Data> &Callbacks::GetAmxMap(void) {
  static std::unordered_map<AMX *, Callbacks::Data> AMXs;

  return AMXs;
}
