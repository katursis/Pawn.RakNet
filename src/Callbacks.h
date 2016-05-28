#ifndef RAKNETMANAGER_CALLBACKS_H_
#define RAKNETMANAGER_CALLBACKS_H_

class Callbacks
{
public:

	struct Data
	{
		struct
		{
			int id;
			bool exists;
		}	_public_on_player_received_packet,
			_public_on_player_received_rpc,
			_public_on_server_send_packet,
			_public_on_server_send_rpc;
	};

	static bool OnPlayerReceivedPacket(int player_id, int packet_id, RakNet::BitStream *bs);
	static bool OnPlayerReceivedRPC(int player_id, int rpc_id, RakNet::BitStream *bs);

	static bool OnServerSendPacket(int player_id, int packet_id, RakNet::BitStream *bs);
	static bool OnServerSendRPC(int player_id, int rpc_id, RakNet::BitStream *bs);

	static void OnAmxLoad(AMX *amx);
	static void OnAmxUnload(AMX *amx);

private:

	static std::unordered_map<AMX *, Data> &GetAmxMap(void);
};

#endif // RAKNETMANAGER_CALLBACKS_H_