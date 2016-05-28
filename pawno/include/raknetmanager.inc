// RakNet Manager
// Version: 2.0.0
// Author: urShadow 
 
#if defined _raknetmanager_included
  #endinput
#endif
#define _raknetmanager_included

enum
{
	RNM_INT8,
	RNM_INT16,
	RNM_INT32,
	RNM_UINT8,
	RNM_UINT16,
	RNM_UINT32,
	RNM_FLOAT,
	RNM_BOOL,
	RNM_STRING,
	
	// compressed values
	RNM_CINT8,
	RNM_CINT16,
	RNM_CINT32,
	RNM_CUINT8,
	RNM_CUINT16,
	RNM_CUINT32,
	RNM_CFLOAT,
	RNM_CBOOL,
}

enum PacketPriority
{
	SYSTEM_PRIORITY,
	HIGH_PRIORITY,
	MEDIUM_PRIORITY,
	LOW_PRIORITY,
	NUMBER_OF_PRIORITIES
}

enum PacketReliability
{
	UNRELIABLE = 6,
	UNRELIABLE_SEQUENCED,
	RELIABLE,
	RELIABLE_ORDERED,
	RELIABLE_SEQUENCED
}

// RPC/Packet ids you can get from:
// https://github.com/P3ti/RakSAMP/blob/master/raknet/SAMP/SAMPRPC.cpp
// https://github.com/P3ti/RakSAMP/blob/master/raknet/PacketEnumerations.h
// player_id == -1 => send to all players (broadcast)

native BS_RPC(BitStream:bs, player_id, rpc_id, PacketPriority:priority = HIGH_PRIORITY, PacketReliability:reliability = RELIABLE_ORDERED);
native BS_Send(BitStream:bs, player_id, PacketPriority:priority = HIGH_PRIORITY, PacketReliability:reliability = RELIABLE_ORDERED);

native BitStream:BS_New();
native BS_Delete(&BitStream:bs);

native BS_Reset(BitStream:bs);
native BS_ResetReadPointer(BitStream:bs);
native BS_ResetWritePointer(BitStream:bs);
native BS_IgnoreBits(BitStream:bs, number_of_bits);

native BS_SetWriteOffset(BitStream:bs, offset);
native BS_GetWriteOffset(BitStream:bs, &offset);
native BS_SetReadOffset(BitStream:bs, offset);
native BS_GetReadOffset(BitStream:bs, &offset);

native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
native BS_GetNumberOfUnreadBits(BitStream:bs, &number);

native BS_WriteValue(BitStream:bs, {Float,_}:...);
native BS_ReadValue(BitStream:bs, {Float,_}:...);

forward OnPlayerReceivedPacket(player_id, packet_id, BitStream:bs);
forward OnPlayerReceivedRPC(player_id, rpc_id, BitStream:bs);
forward OnServerSendPacket(player_id, packet_id, BitStream:bs);
forward OnServerSendRPC(player_id, rpc_id, BitStream:bs);