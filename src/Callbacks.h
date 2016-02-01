#ifndef RAKNETMANAGER_CALLBACKS_H_
#define RAKNETMANAGER_CALLBACKS_H_

class Callbacks
{
public:	

	static bool OnPlayerReceivedPacket(int playerid, int packetid, RakNet::BitStream *bs);
	static bool OnPlayerReceivedRPC(int playerid, int rpcid, RakNet::BitStream *bs);
	
	static bool OnServerSendPacket(int playerid, int packetid, RakNet::BitStream *bs);
	static bool OnServerSendRPC(int playerid, int rpcid, RakNet::BitStream *bs);

	static void OnAmxLoad(AMX *amx);
	static void OnAmxUnload(AMX *amx);

private:

	static std::set<AMX *> &GetAmxSet(void);	
};

#endif // RAKNETMANAGER_CALLBACKS_H_
