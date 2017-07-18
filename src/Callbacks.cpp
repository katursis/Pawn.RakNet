#include "Main.h"

// forward OnIncomingPacket(playerid, packetid, BitStream:bs);
bool Callbacks::OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_incoming_packet;

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

// forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
bool Callbacks::OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_incoming_rpc;

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

// forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
bool Callbacks::OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_outcoming_packet;

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

// forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
bool Callbacks::OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
  cell retval{};

  const auto &AMXs = Callbacks::GetAmxMap();

  for (const auto &i : AMXs) {
    const auto amx = i.first;
    const auto &public_data = i.second._public_on_outcoming_rpc;

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

  if (amx_FindPublic(amx, "OnIncomingPacket", &data._public_on_incoming_packet.id) == AMX_ERR_NONE) {
    data._public_on_incoming_packet.exists = true;
  }

  if (amx_FindPublic(amx, "OnIncomingRPC", &data._public_on_incoming_rpc.id) == AMX_ERR_NONE) {
    data._public_on_incoming_rpc.exists = true;
  }

  if (amx_FindPublic(amx, "OnOutcomingPacket", &data._public_on_outcoming_packet.id) == AMX_ERR_NONE) {
    data._public_on_outcoming_packet.exists = true;
  }

  if (amx_FindPublic(amx, "OnOutcomingRPC", &data._public_on_outcoming_rpc.id) == AMX_ERR_NONE) {
    data._public_on_outcoming_rpc.exists = true;
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
