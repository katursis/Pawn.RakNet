#include "Main.h"

std::shared_ptr<urmem::hook>	Hooks::_hook_rakpeer__ctor;
std::shared_ptr<urmem::hook>	Hooks::_hook_rakpeer__receive;
std::shared_ptr<urmem::hook>	Hooks::_hook_rakpeer__send_buffered;
std::shared_ptr<urmem::hook>	Hooks::_hook_rakpeer__rpc;
std::shared_ptr<urmem::hook>	Hooks::_hook_rpcmap__add_identifier_with_function;
std::shared_ptr<urmem::hook>	Hooks::_hook_rpcmap__get_node_from_index;

std::array<RPCFunction, MAX_RPC_MAP_SIZE>	Hooks::_rpc_map;
int											Hooks::_last_rpc_index{};
void										*Hooks::_rakpeer{};

bool Hooks::Init(void)
{
	if (!Addresses::Init())
		return logprintf("[RakNetManager] Error: addresses not found."), false;

	Hooks::_hook_rakpeer__ctor =
		urmem::hook::create("rakpeer__ctor",
			Addresses::FUNC_RAKPEER__CTOR, (urmem::address_t)Hooks::HOOK_RakPeer__Ctor);

	Hooks::_hook_rakpeer__receive =
		urmem::hook::create("rakpeer__receive",
			Addresses::FUNC_RAKPEER__RECEIVE, (urmem::address_t)Hooks::HOOK_RakPeer__Receive);

	Hooks::_hook_rakpeer__send_buffered =
		urmem::hook::create("rakpeer__send_buffered",
			Addresses::FUNC_RAKPEER__SEND_BUFFERED, (urmem::address_t)Hooks::HOOK_RakPeer__SendBuffered);

	Hooks::_hook_rakpeer__rpc =
		urmem::hook::create("rakpeer__rpc",
			Addresses::FUNC_RAKPEER__RPC, (urmem::address_t)Hooks::HOOK_RakPeer__RPC);

	Hooks::_hook_rpcmap__add_identifier_with_function =
		urmem::hook::create("rpcmap__add_identifier_with_function",
			Addresses::FUNC_RPCMAP__ADD_IDENTIFIER_WITH_FUNCTION,
			(urmem::address_t)Hooks::HOOK_RPCMap__AddIdentifierWithFunction);

	Hooks::_hook_rpcmap__get_node_from_index =
		urmem::hook::create("rpcmap__get_node_from_index",
			Addresses::FUNC_RPCMAP__GET_NODE_FROM_INDEX,
			(urmem::address_t)Hooks::HOOK_RPCMap__GetNodeFromIndex);

	return true;
}

#ifdef _WIN32
void * __thiscall Hooks::HOOK_RakPeer__Ctor(void *_this)
#else
void *Hooks::HOOK_RakPeer__Ctor(void *_this)
#endif
{
	urmem::hook::context context(Hooks::_hook_rakpeer__ctor);

	Hooks::_rakpeer = _this;

	return context.call_original<void *>(urmem::calling_convention::thiscall, _this);
};

#ifdef _WIN32
Packet * __thiscall Hooks::HOOK_RakPeer__Receive(void *_this)
#else
Packet *Hooks::HOOK_RakPeer__Receive(void *_this)
#endif
{
	urmem::hook::context context(Hooks::_hook_rakpeer__receive);

	Packet *packet = context.call_original<Packet *>(urmem::calling_convention::thiscall, _this);

	if (packet)
	{	
		RakNet::BitStream bitstream(packet->data, packet->length, false);

		if (!Callbacks::OnPlayerReceivedPacket(packet->playerIndex, packet->data[0], &bitstream))
		{			
			Hooks::DeallocatePacket(packet);

			return nullptr;
		}
	}

	return packet;
}

#ifdef _WIN32
void __thiscall Hooks::HOOK_RakPeer__SendBuffered(void *_this, unsigned char *data, int numberOfBitsToSend,
	PacketPriority priority, PacketReliability reliability, char orderingChannel,
	PlayerID playerId, bool broadcast, int connectionMode)
#else
void Hooks::HOOK_RakPeer__SendBuffered(void *_this, unsigned char *data, int numberOfBitsToSend,
	PacketPriority priority, PacketReliability reliability, char orderingChannel,
	PlayerID playerId, bool broadcast, int connectionMode)
#endif
{
	urmem::hook::context context(Hooks::_hook_rakpeer__send_buffered);
	
	if (data && numberOfBitsToSend > 0)
	{
		RakNet::BitStream bitstream(data, BITS_TO_BYTES(numberOfBitsToSend), false);

		if (!Callbacks::OnServerSendPacket(Hooks::GetIndexFromPlayerID(playerId), data[0], &bitstream)) 
		{
			return;
		}
	}

	return context.call_original<void>(urmem::calling_convention::thiscall, _this, data,
		numberOfBitsToSend, priority, reliability, orderingChannel, playerId, broadcast, connectionMode);
}

#ifdef _WIN32
bool __thiscall Hooks::HOOK_RakPeer__RPC(void *_this, unsigned char *uniqueID,
	unsigned char *data, unsigned int bitLength, PacketPriority priority,
	PacketReliability reliability, char orderingChannel, PlayerID playerId,
	bool broadcast, bool shiftTimestamp, NetworkID networkID, void *replyFromTarget)
#else
bool Hooks::HOOK_RakPeer__RPC(void *_this, unsigned char *uniqueID,
	unsigned char *data, unsigned int bitLength, PacketPriority priority,
	PacketReliability reliability, char orderingChannel, PlayerID playerId,
	bool broadcast, bool shiftTimestamp, NetworkID networkID, void *replyFromTarget)
#endif
{
	urmem::hook::context context(Hooks::_hook_rakpeer__rpc);

	if (data && bitLength > 0)
	{		
		RakNet::BitStream bitstream(data, BITS_TO_BYTES(bitLength), false);

		if (!Callbacks::OnServerSendRPC(Hooks::GetIndexFromPlayerID(playerId), *uniqueID, &bitstream)) {
			return false;
		}
	}

	return context.call_original<bool>(urmem::calling_convention::thiscall, _this, uniqueID, data,
		bitLength, priority, reliability, orderingChannel, playerId, broadcast, shiftTimestamp,
		networkID, replyFromTarget);
}

#ifdef _WIN32
void * __thiscall Hooks::HOOK_RPCMap__AddIdentifierWithFunction(
	void *_this, unsigned char uniqueIdentifier, void *functionPointer, bool isPointerToMember)
#else
void *Hooks::HOOK_RPCMap__AddIdentifierWithFunction(
	void *_this, unsigned char uniqueIdentifier, void *functionPointer, bool isPointerToMember)
#endif
{
	urmem::hook::context context(Hooks::_hook_rpcmap__add_identifier_with_function);

	Hooks::_rpc_map[uniqueIdentifier] = reinterpret_cast<RPCFunction>(functionPointer);

	return context.call_original<void *>(urmem::calling_convention::thiscall, _this, uniqueIdentifier,
		reinterpret_cast<void *>(Hooks::HOOK_HandleRPCFunction), isPointerToMember);
}

#ifdef _WIN32
void * __thiscall Hooks::HOOK_RPCMap__GetNodeFromIndex(void *_this, unsigned char index)
#else
void *Hooks::HOOK_RPCMap__GetNodeFromIndex(void *_this, unsigned char index)
#endif
{
	urmem::hook::context context(Hooks::_hook_rpcmap__get_node_from_index);

	Hooks::_last_rpc_index = static_cast<int>(index);

	return context.call_original<void *>(urmem::calling_convention::thiscall, _this, index);
}

#ifdef _WIN32
void __cdecl Hooks::HOOK_HandleRPCFunction(RPCParameters *parameter)
#else
void Hooks::HOOK_HandleRPCFunction(RPCParameters *parameter)
#endif
{
	if (parameter)
	{
		RakNet::BitStream bitstream(parameter->input, BITS_TO_BYTES(parameter->numberOfBitsOfData), false);

		int playerid = Hooks::GetIndexFromPlayerID(parameter->sender);

		if (!Callbacks::OnPlayerReceivedRPC(playerid, Hooks::_last_rpc_index, &bitstream))
			return;

		Hooks::_rpc_map[Hooks::_last_rpc_index](parameter);
	}
}

int Hooks::GetIndexFromPlayerID(const PlayerID &id)
{
	return urmem::memory::call_function<int>(urmem::calling_convention::thiscall,
		Addresses::FUNC_RAKPEER__GET_INDEX_FROM_PLAYER_ID,
		Hooks::_rakpeer,
		id);
}

PlayerID Hooks::GetPlayerIDFromIndex(int index)
{
	return urmem::memory::call_function<PlayerID>(urmem::calling_convention::thiscall,
		Addresses::FUNC_RAKPEER__GET_PLAYER_ID_FROM_INDEX,
		Hooks::_rakpeer,
		index);
}

void Hooks::DeallocatePacket(Packet *p)
{
	return urmem::memory::call_function(urmem::calling_convention::thiscall,
		Addresses::FUNC_RAKPEER__DEALLOCATE_PACKET, 
		Hooks::_rakpeer,
		p);
}

void Hooks::SendPacket(int playerid, RakNet::BitStream *bs)
{
	urmem::hook::context(Hooks::_hook_rakpeer__send_buffered).call_original(urmem::calling_convention::thiscall, _rakpeer,
		bs->GetData(), bs->GetNumberOfBitsUsed(), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, '\0',
		Hooks::GetPlayerIDFromIndex(playerid), playerid == -1, 0);
}

void Hooks::SendRPC(int playerid, int rpcid, RakNet::BitStream *bs)
{
	urmem::hook::context(Hooks::_hook_rakpeer__rpc).call_original<bool>(urmem::calling_convention::thiscall, _rakpeer,
		&rpcid, bs->GetData(), bs->GetNumberOfBitsUsed(), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, '\0',
		Hooks::GetPlayerIDFromIndex(playerid), playerid == -1, false, UNASSIGNED_NETWORK_ID, nullptr);
}

