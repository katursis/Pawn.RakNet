#ifndef RAKNETMANAGER_HOOKS_H_
#define RAKNETMANAGER_HOOKS_H_

class Hooks
{
public:

	static bool Init(void);

	// functions
	static int						GetIndexFromPlayerID(const PlayerID &);
	static PlayerID					GetPlayerIDFromIndex(int index);
	static void						DeallocatePacket(Packet *p);
	static void						SendPacket(int playerid, RakNet::BitStream *bs); // player id == -1 => broadcast
	static void						SendRPC(int playerid, int rpcid, RakNet::BitStream *bs); // player id == -1 => broadcast
	
private:

	// handles

#ifdef _WIN32
	static void * __thiscall		HOOK_RakPeer__Ctor(void *);
	static Packet * __thiscall		HOOK_RakPeer__Receive(void *);	
	static void __thiscall			HOOK_RakPeer__SendBuffered(void *, unsigned char *, int, PacketPriority, 
																PacketReliability, char, PlayerID, bool, int);
	static bool __thiscall			HOOK_RakPeer__RPC(void *, unsigned char *, unsigned char *,
														unsigned int, PacketPriority, PacketReliability, 
														char, PlayerID, bool, bool, NetworkID, void *);
	static void * __thiscall		HOOK_RPCMap__AddIdentifierWithFunction(void *, unsigned char, void *, bool);
	static void * __thiscall		HOOK_RPCMap__GetNodeFromIndex(void *, unsigned char);
	static void __cdecl				HOOK_HandleRPCFunction(RPCParameters *);
#else
	static void * 					HOOK_RakPeer__Ctor(void *);
	static Packet * 				HOOK_RakPeer__Receive(void *);
	static void 					HOOK_RakPeer__SendBuffered(void *, unsigned char *, int, PacketPriority,
																PacketReliability, char, PlayerID, bool, int);
	static bool 					HOOK_RakPeer__RPC(void *, unsigned char *, unsigned char *,
														unsigned int, PacketPriority, PacketReliability,
														char, PlayerID, bool, bool, NetworkID, void *);
	static void * 					HOOK_RPCMap__AddIdentifierWithFunction(void *, unsigned char, void *, bool);
	static void * 					HOOK_RPCMap__GetNodeFromIndex(void *, unsigned char);
	static void 					HOOK_HandleRPCFunction(RPCParameters *);
#endif

	// hooks
	static std::shared_ptr<urmem::hook> _hook_rakpeer__ctor;
	static std::shared_ptr<urmem::hook>	_hook_rakpeer__receive;
	static std::shared_ptr<urmem::hook> _hook_rakpeer__send_buffered;
	static std::shared_ptr<urmem::hook> _hook_rakpeer__rpc;	
	static std::shared_ptr<urmem::hook> _hook_rpcmap__add_identifier_with_function;		
	static std::shared_ptr<urmem::hook> _hook_rpcmap__get_node_from_index;

	static std::array<RPCFunction, MAX_RPC_MAP_SIZE> _rpc_map; // original rpc map
	static int _last_rpc_index;

	static void	*_rakpeer; // rakpeer pointer
};

#endif // RAKNETMANAGER_HOOKS_H_

