#include "Main.h"

std::shared_ptr<urmem::hook>
Hooks::_hook_get_rak_server_interface,
Hooks::_hook_rakserver__send,
Hooks::_hook_rakserver__rpc,
Hooks::_hook_rakserver__receive,
Hooks::_hook_rakserver__register_as_remote_procedure_call;

std::array<RPCFunction, MAX_RPC_MAP_SIZE>
Hooks::_original_rpc,
Hooks::_custom_rpc;

#ifdef _WIN32
const char *pattern =
"\x6A\xFF\x68\x5B\xA4\x4A\x00\x64\xA1\x00\x00" \
"\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51" \
"\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83" \
"\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x24\xFF" \
"\x00\x00\x00\x00\x74\x16";

const char *mask = "xxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx?xxxxxx";
#else
const char *pattern =
"\x55\x89\xE5\x83\xEC\x18\xC7\x04\x24\xFF\xFF" \
"\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF" \
"\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF" \
"\xFF\x89\xF0\x8B\x5D\xFF\x8B\x75\xFF\x89\xEC" \
"\x5D\xC3";

const char *mask = "xxxxxxxxx????xx?xx?x????xxxxxx????xxxx?xx?xxxx";
#endif

bool Hooks::Init(void)
{
	urmem::sig_scanner scanner;

	if (scanner.init(urmem::get_func_addr(logprintf)))
	{
		urmem::address_t addr{};

		if (scanner.find(pattern, mask, addr))
		{
			_hook_get_rak_server_interface = std::make_shared<urmem::hook>(addr, urmem::get_func_addr(&HOOK_GetRakServerInterface));

			return true;
		}
	}

	return false;
}

void * Hooks::HOOK_GetRakServerInterface(void)
{
	urmem::hook::raii scope(*_hook_get_rak_server_interface);

	auto rakserver = urmem::call_function<urmem::calling_convention::cdeclcall, void *>(
		_hook_get_rak_server_interface->get_original_addr());

	if (Addresses::Init(reinterpret_cast<urmem::address_t>(rakserver)))
	{
		_hook_rakserver__send = std::make_shared<urmem::hook>(
			Addresses::FUNC_RAKSERVER__SEND,
			urmem::get_func_addr(&HOOK_RakServer__Send));

		_hook_rakserver__rpc = std::make_shared<urmem::hook>(
			Addresses::FUNC_RAKSERVER__RPC,
			urmem::get_func_addr(&HOOK_RakServer__RPC));

		_hook_rakserver__receive = std::make_shared<urmem::hook>(
			Addresses::FUNC_RAKSERVER__RECEIVE,
			urmem::get_func_addr(&HOOK_RakServer__Receive));

		_hook_rakserver__register_as_remote_procedure_call = std::make_shared<urmem::hook>(
			Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
			urmem::get_func_addr(&HOOK_RakServer__RegisterAsRemoteProcedureCall));

		_original_rpc.fill(nullptr);
		_custom_rpc.fill(nullptr);

		RPCHandle::Create();

		logprintf("[RNM] Initialized. Version: 2.0.0. Author: urShadow");
	}

	return rakserver;
}

bool THISCALL Hooks::HOOK_RakServer__Send(void *_this, RakNet::BitStream *bitStream, int priority,
	int reliability, char orderingChannel, PlayerID playerId, bool broadcast)
{
	urmem::hook::raii scope(*_hook_rakserver__send);

	if (bitStream)
	{
		auto read_offset = bitStream->GetReadOffset(), write_offset = bitStream->GetWriteOffset();

		if (!Callbacks::OnServerSendPacket(GetIndexFromPlayerID(playerId), bitStream->GetData()[0], bitStream))
			return false;

		bitStream->SetReadOffset(read_offset), bitStream->SetWriteOffset(write_offset);
	}

	return urmem::call_function<urmem::calling_convention::thiscall, bool>(
		Addresses::FUNC_RAKSERVER__SEND, _this, bitStream, priority, reliability,
		orderingChannel, playerId, broadcast);
}

bool THISCALL Hooks::HOOK_RakServer__RPC(void *_this, RPCIndex *uniqueID, RakNet::BitStream *bitStream, int priority,
	int reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp)
{
	urmem::hook::raii scope(*_hook_rakserver__rpc);

	if (uniqueID && bitStream)
	{
		auto read_offset = bitStream->GetReadOffset(), write_offset = bitStream->GetWriteOffset();

		if (!Callbacks::OnServerSendRPC(GetIndexFromPlayerID(playerId), *uniqueID, bitStream))
			return false;

		bitStream->SetReadOffset(read_offset), bitStream->SetWriteOffset(write_offset);
	}

	return urmem::call_function<urmem::calling_convention::thiscall, bool>(
		Addresses::FUNC_RAKSERVER__RPC, _this, uniqueID, bitStream, priority, reliability,
		orderingChannel, playerId, broadcast, shiftTimestamp);
}

Packet * THISCALL Hooks::HOOK_RakServer__Receive(void *_this)
{
	urmem::hook::raii scope(*_hook_rakserver__receive);

	Packet *packet = urmem::call_function<urmem::calling_convention::thiscall, Packet *>(
		Addresses::FUNC_RAKSERVER__RECEIVE, _this);

	if (packet && packet->data)
	{
		RakNet::BitStream bitstream(packet->data, packet->length, false);

		if (!Callbacks::OnPlayerReceivedPacket(packet->playerIndex, packet->data[0], &bitstream))
		{
			DeallocatePacket(packet);

			return nullptr;
		}
	}

	return packet;
}

void * THISCALL Hooks::HOOK_RakServer__RegisterAsRemoteProcedureCall(void *_this, RPCIndex *uniqueID, RPCFunction functionPointer)
{
	urmem::hook::raii scope(*_hook_rakserver__register_as_remote_procedure_call);

	if (uniqueID && functionPointer)
	{
		_original_rpc[*uniqueID] = functionPointer;

		return urmem::call_function<urmem::calling_convention::thiscall, void *>(
			Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
			_this, uniqueID, _custom_rpc[*uniqueID]);
	}

	return urmem::call_function<urmem::calling_convention::thiscall, void *>(
		Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
		_this, uniqueID, functionPointer);
}

void Hooks::ReceiveRPC(int rpc_id, RPCParameters *p)
{
	if (p)
	{
		int player_id = GetIndexFromPlayerID(p->sender);

		std::shared_ptr<RakNet::BitStream> bs;

		if (p->input)
			bs = std::make_shared<RakNet::BitStream>(p->input, BITS_TO_BYTES(p->numberOfBitsOfData), false);

		if (!Callbacks::OnPlayerReceivedRPC(player_id, rpc_id, bs.get()))
			return;

		try
		{
			_original_rpc.at(rpc_id)(p);
		}
		catch (const std::exception &e)
		{
			logprintf("[RNM] %s: %s", __FUNCTION__, e.what());
		}
	}
}

int Hooks::GetIndexFromPlayerID(const PlayerID &id)
{
	return urmem::call_function<urmem::calling_convention::thiscall, int>(
		Addresses::FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID, Addresses::RAKSERVER, id);
}

PlayerID Hooks::GetPlayerIDFromIndex(int index)
{
	return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
		Addresses::FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX, Addresses::RAKSERVER, index);
}

void Hooks::DeallocatePacket(Packet *p)
{
	urmem::call_function<urmem::calling_convention::thiscall>(
		Addresses::FUNC_RAKSERVER__DEALLOCATE_PACKET, Addresses::RAKSERVER, p);
}

bool Hooks::SendPacket(int player_id, RakNet::BitStream *bs, int priority, int reliability)
{
	bool enabled = _hook_rakserver__send->is_enabled();

	if (enabled)
		_hook_rakserver__send->disable();

	auto result = urmem::call_function<urmem::calling_convention::thiscall, bool>(
		Addresses::FUNC_RAKSERVER__SEND,
		Addresses::RAKSERVER,
		bs,
		priority,
		reliability,
		'\0',
		GetPlayerIDFromIndex(player_id),
		player_id == -1);

	if (enabled)
		_hook_rakserver__send->enable();

	return result;
}

bool Hooks::SendRPC(int player_id, int rpc_id, RakNet::BitStream *bs, int priority, int reliability)
{
	bool enabled = _hook_rakserver__rpc->is_enabled();

	if (enabled)
		_hook_rakserver__rpc->disable();

	static RPCIndex id{};

	id = static_cast<RPCIndex>(rpc_id);

	auto result = urmem::call_function<urmem::calling_convention::thiscall, bool>(
		Addresses::FUNC_RAKSERVER__RPC,
		Addresses::RAKSERVER,
		&id,
		bs,
		priority,
		reliability,
		'\0',
		GetPlayerIDFromIndex(player_id),
		player_id == -1,
		false);

	if (enabled)
		_hook_rakserver__rpc->enable();

	return result;
}