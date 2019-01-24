#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

namespace Functions {
    int GetIndexFromPlayerID(const PlayerID &id) {
        return urmem::call_function<urmem::calling_convention::thiscall, int>(
            Addresses::FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID,
            Addresses::PTR_RAKSERVER,
            id
        );
    }

    PlayerID GetPlayerIDFromIndex(int index) {
        return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
            Addresses::FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX,
            Addresses::PTR_RAKSERVER,
            index
        );
    }

    void DeallocatePacket(Packet *p) {
        urmem::call_function<urmem::calling_convention::thiscall>(
            Addresses::FUNC_RAKSERVER__DEALLOCATE_PACKET,
            Addresses::PTR_RAKSERVER,
            p
        );
    }

    bool SendPacket(
        int player_id,
        RakNet::BitStream *bs,
        int priority,
        int reliability
    ) {
        return urmem::call_function<urmem::calling_convention::thiscall, bool>(
            Addresses::FUNC_RAKSERVER__SEND,
            Addresses::PTR_RAKSERVER,
            bs,
            priority,
            reliability,
            '\0',
            GetPlayerIDFromIndex(player_id),
            player_id == -1
        );
    }

    bool SendRPC(
        int player_id,
        int rpc_id,
        RakNet::BitStream *bs,
        int priority,
        int reliability
    ) {
        return urmem::call_function<urmem::calling_convention::thiscall, bool>(
            Addresses::FUNC_RAKSERVER__RPC,
            Addresses::PTR_RAKSERVER,
            &rpc_id,
            bs,
            priority,
            reliability,
            '\0',
            GetPlayerIDFromIndex(player_id),
            player_id == -1,
            false
        );
    }
};

#endif // FUNCTIONS_H_
