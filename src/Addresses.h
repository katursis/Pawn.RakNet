#ifndef RAKNETMANAGER_ADDRESSES_H_
#define RAKNETMANAGER_ADDRESSES_H_

class Addresses
{
public:

	static bool Init(void);

	static urmem::address_t		FUNC_RAKPEER__CTOR;
	static urmem::address_t		FUNC_RAKPEER__RECEIVE;
	static urmem::address_t		FUNC_RAKPEER__SEND_BUFFERED;
	static urmem::address_t		FUNC_RAKPEER__RPC;
	static urmem::address_t		FUNC_RAKPEER__GET_INDEX_FROM_PLAYER_ID;
	static urmem::address_t		FUNC_RAKPEER__GET_PLAYER_ID_FROM_INDEX;
	static urmem::address_t		FUNC_RAKPEER__DEALLOCATE_PACKET;

	static urmem::address_t		FUNC_RPCMAP__ADD_IDENTIFIER_WITH_FUNCTION;
	static urmem::address_t		FUNC_RPCMAP__GET_NODE_FROM_INDEX;
};

#endif // RAKNETMANAGER_ADDRESSES_H_
