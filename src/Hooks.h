#ifndef RAKNETMANAGER_HOOKS_H_
#define RAKNETMANAGER_HOOKS_H_

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall
#else
#define THISCALL
#endif

class Hooks
{
public:

	static bool Init(void);

	static int GetIndexFromPlayerID(const PlayerID &);
	static PlayerID GetPlayerIDFromIndex(int index);
	static void DeallocatePacket(Packet *p);
	static bool SendPacket(int player_id, RakNet::BitStream *bs,
		int priority, int reliability); // player id == -1 => broadcast
	static bool SendRPC(int player_id, int rpc_id, RakNet::BitStream *bs,
		int priority, int reliability); // player id == -1 => broadcast

private:

	struct RPCHandle
	{
		static void Create(void)
		{
			Generator<0>::Run();
		}

		template<size_t ID>
		struct Generator
		{
			static void Handle(RPCParameters *p)
			{
				Hooks::ReceiveRPC(ID, p);
			}

			static void Run(void)
			{
				Hooks::_custom_rpc[ID] = reinterpret_cast<RPCFunction>(&Handle);

				Generator<ID + 1>::Run();
			}
		};
	};

	static void * HOOK_GetRakServerInterface(void);
	static bool THISCALL HOOK_RakServer__Send(void *_this, RakNet::BitStream *bitStream, int priority,
		int reliability, char orderingChannel, PlayerID playerId, bool broadcast);
	static bool THISCALL HOOK_RakServer__RPC(void *_this, RPCIndex *uniqueID, RakNet::BitStream *bitStream, int priority,
		int reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp);
	static Packet * THISCALL HOOK_RakServer__Receive(void *_this);
	static void * THISCALL HOOK_RakServer__RegisterAsRemoteProcedureCall(void *_this, RPCIndex *uniqueID, RPCFunction functionPointer);
	static void ReceiveRPC(int rpc_id, RPCParameters *p);

	static std::shared_ptr<urmem::hook>
		_hook_get_rak_server_interface,
		_hook_rakserver__send,
		_hook_rakserver__rpc,
		_hook_rakserver__receive,
		_hook_rakserver__register_as_remote_procedure_call;

	static std::array<RPCFunction, MAX_RPC_MAP_SIZE>
		_original_rpc,
		_custom_rpc;
};

template<>
struct Hooks::RPCHandle::Generator<MAX_RPC_MAP_SIZE>
{
	static void Run(void) {}
};

#endif // RAKNETMANAGER_HOOKS_H_