#ifndef RAKNETMANAGER_CALLBACKS_H_
#define RAKNETMANAGER_CALLBACKS_H_

class Callbacks {
public:
    struct Data {
        struct {
            int id;
            bool exists;
        }	_public_on_incoming_packet,
            _public_on_incoming_rpc,
            _public_on_outcoming_packet,
            _public_on_outcoming_rpc;
    };

    static bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs);
    static bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs);

    static bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs);
    static bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs);

    static void OnAmxLoad(AMX *amx);
    static void OnAmxUnload(AMX *amx);

private:
    static std::unordered_map<AMX *, Data> &GetAmxMap(void);
};

#endif // RAKNETMANAGER_CALLBACKS_H_
