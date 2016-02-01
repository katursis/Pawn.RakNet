/* 
>> RakNet Manager plugin by urShadow
>> Version 1.0.0
>> 2016 
 */
 
#if defined _raknetmanager_included
  #endinput
#endif
#define _raknetmanager_included

/*
>> BitStream's types
*/
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

/*
>> Send data to client. Fresh RPC/Packet ids you can get here:
>> https://github.com/P3ti/RakSAMP/blob/master/raknet/SAMP/SAMPRPC.cpp
>> https://github.com/P3ti/RakSAMP/blob/master/raknet/PacketEnumerations.h
>> playerid == -1 => send to all players
*/
native RNM_SendRPC(playerid, rpcid, BitStream:bs);  
native RNM_SendPacket(playerid, BitStream:bs);

native BitStream:BS_New();
native BS_Delete(&BitStream:bs);

native BS_Reset(BitStream:bs);
native BS_ResetReadPointer(BitStream:bs);
native BS_ResetWritePointer(BitStream:bs);
native BS_IgnoreBits(BitStream:bs, bitcount);

native BS_SetWriteOffset(BitStream:bs, offset);
native BS_GetWriteOffset(BitStream:bs, &offset);
native BS_SetReadOffset(BitStream:bs, offset);
native BS_GetReadOffset(BitStream:bs, &offset);

native BS_GetNumberOfBitsUsed(BitStream:bs, &number);
native BS_GetNumberOfBytesUsed(BitStream:bs, &number);
native BS_GetNumberOfUnreadBits(BitStream:bs, &number);

native BS_WriteValue(BitStream:bs, {Float,_}:...);
native BS_ReadValue(BitStream:bs, {Float,_}:...);

/*
>> Calls on incoming data from client
*/
forward OnPlayerReceivedPacket(playerid, packetid, BitStream:bs);
forward OnPlayerReceivedRPC(playerid, rpcid, BitStream:bs);

/*
>> Calls on outcoming data to client
*/
forward OnServerSendPacket(playerid, packetid, BitStream:bs);
forward OnServerSendRPC(playerid, rpcid, BitStream:bs);
