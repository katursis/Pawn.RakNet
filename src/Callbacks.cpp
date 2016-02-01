#include "Main.h"

// forward OnPlayerReceivedPacket(playerid, packetid, BitStream:bs);
bool Callbacks::OnPlayerReceivedPacket(int playerid, int packetid, RakNet::BitStream *bs)
{	
	int result = -1, idx = -1;		

	auto &amx_set = Callbacks::GetAmxSet();

	for (auto &amx : amx_set)
	{
		if (amx_FindPublic(amx, "OnPlayerReceivedPacket", &idx) == AMX_ERR_NONE)
		{
			bs->ResetReadPointer();

			amx_Push(amx, reinterpret_cast<cell>(bs));
			amx_Push(amx, packetid);
			amx_Push(amx, playerid);

			amx_Exec(amx, &result, idx);

			if (!result)
				return false;
		}
	}

	return true;
}

// forward OnPlayerReceivedRPC(playerid, rpcid, BitStream:bs);
bool Callbacks::OnPlayerReceivedRPC(int playerid, int rpcid, RakNet::BitStream *bs)
{	
	int result = -1, idx = -1;
	
	auto &amx_set = Callbacks::GetAmxSet();

	for (auto &amx : amx_set)
	{
		if (amx_FindPublic(amx, "OnPlayerReceivedRPC", &idx) == AMX_ERR_NONE)
		{
			bs->ResetReadPointer();

			amx_Push(amx, reinterpret_cast<cell>(bs));
			amx_Push(amx, rpcid);
			amx_Push(amx, playerid);

			amx_Exec(amx, &result, idx);

			if (!result)
				return false;
		}
	}

	return true;
}

// forward OnServerSendPacket(playerid, packetid, BitStream:bs);
bool Callbacks::OnServerSendPacket(int playerid, int packetid, RakNet::BitStream *bs)
{
	int result = -1, idx = -1;

	auto &amx_set = Callbacks::GetAmxSet();

	for (auto &amx : amx_set)
	{
		if (amx_FindPublic(amx, "OnServerSendPacket", &idx) == AMX_ERR_NONE)
		{
			bs->ResetReadPointer();

			amx_Push(amx, reinterpret_cast<cell>(bs));
			amx_Push(amx, packetid);		
			amx_Push(amx, playerid);

			amx_Exec(amx, &result, idx);

			if (!result)
				return false;
		}
	}

	return true;
}

// forward OnServerSendRPC(playerid, rpcid, BitStream:bs);
bool Callbacks::OnServerSendRPC(int playerid, int rpcid, RakNet::BitStream *bs)
{
	int result = -1, idx = -1;

	auto &amx_set = Callbacks::GetAmxSet();

	for (auto &amx : amx_set)
	{
		if (amx_FindPublic(amx, "OnServerSendRPC", &idx) == AMX_ERR_NONE)
		{
			bs->ResetReadPointer();

			amx_Push(amx, reinterpret_cast<cell>(bs));
			amx_Push(amx, rpcid);			
			amx_Push(amx, playerid);

			amx_Exec(amx, &result, idx);

			if (!result)
				return false;
		}
	}

	return true;
}

void Callbacks::OnAmxLoad(AMX *amx)
{
	Callbacks::GetAmxSet().insert(amx);
}

void Callbacks::OnAmxUnload(AMX *amx)
{
	Callbacks::GetAmxSet().erase(amx);
}

inline std::set<AMX *> &Callbacks::GetAmxSet(void)
{
	static std::set<AMX *> amx_set;

	return amx_set;
}