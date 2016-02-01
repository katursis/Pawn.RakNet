#include "Main.h"

urmem::address_t	Addresses::FUNC_RAKPEER__CTOR{};
urmem::address_t	Addresses::FUNC_RAKPEER__RECEIVE{};
urmem::address_t	Addresses::FUNC_RAKPEER__SEND_BUFFERED{};
urmem::address_t	Addresses::FUNC_RAKPEER__RPC{};
urmem::address_t	Addresses::FUNC_RAKPEER__GET_INDEX_FROM_PLAYER_ID{};
urmem::address_t	Addresses::FUNC_RAKPEER__GET_PLAYER_ID_FROM_INDEX{};
urmem::address_t	Addresses::FUNC_RAKPEER__DEALLOCATE_PACKET{};

urmem::address_t	Addresses::FUNC_RPCMAP__ADD_IDENTIFIER_WITH_FUNCTION{};
urmem::address_t	Addresses::FUNC_RPCMAP__GET_NODE_FROM_INDEX{};

// TODO: autosearching of addresses
bool Addresses::Init(void)
{
	
#ifdef WIN32
	Addresses::FUNC_RAKPEER__CTOR = 0x455140;
	Addresses::FUNC_RAKPEER__RECEIVE = 0x458510;
	Addresses::FUNC_RAKPEER__SEND_BUFFERED = 0x4522C0;
	Addresses::FUNC_RAKPEER__RPC = 0x455340;
	Addresses::FUNC_RAKPEER__GET_INDEX_FROM_PLAYER_ID = 0x455620;
	Addresses::FUNC_RAKPEER__GET_PLAYER_ID_FROM_INDEX = 0x450160;
	Addresses::FUNC_RAKPEER__DEALLOCATE_PACKET = 0x4500B0;
	Addresses::FUNC_RPCMAP__ADD_IDENTIFIER_WITH_FUNCTION = 0x460AC0;
	Addresses::FUNC_RPCMAP__GET_NODE_FROM_INDEX = 0x460AA0;
#else	
	Addresses::FUNC_RAKPEER__CTOR = 0x8072D30;
	Addresses::FUNC_RAKPEER__RECEIVE = 0x8071D60;
	Addresses::FUNC_RAKPEER__SEND_BUFFERED = 0x806E980;
	Addresses::FUNC_RAKPEER__RPC = 0x8071710;
	Addresses::FUNC_RAKPEER__GET_INDEX_FROM_PLAYER_ID = 0x80716E0;
	Addresses::FUNC_RAKPEER__GET_PLAYER_ID_FROM_INDEX = 0x806D6B0;
	Addresses::FUNC_RAKPEER__DEALLOCATE_PACKET = 0x806D480;
	Addresses::FUNC_RPCMAP__ADD_IDENTIFIER_WITH_FUNCTION = 0x8067A10;
	Addresses::FUNC_RPCMAP__GET_NODE_FROM_INDEX = 0x80679E0;	
#endif

	return true;
}