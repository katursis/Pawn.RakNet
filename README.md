# RakNet Manager
Plugin for SA:MP Server that allows you to work with RakNet in PAWN
# Examples (0.3.7-R2)
Sending RPC
```pawn
stock SendChatMessage(senderid, msg[])
{
  new BitStream:bs = BS_New(), RPC_Chat = 101;
  
  BS_WriteValue(bs, RNM_INT16, senderid, 
                    RNM_INT8, strlen(msg), 
                    RNM_STRING, msg);
                    
  RNM_SendRPC(senderid, RPC_Chat, bs);
  
  BS_Delete(bs);
}
```
Sending packet
```pawn
stock FakeLostConnectionForPlayer(playerid)
{
  new BitStream:bs = BS_New(), ID_CONNECTION_LOST = 33;
  
  BS_WriteValue(bs, RNM_INT8, ID_CONNECTION_LOST);
  
  RNM_SendPacket(playerid, bs);
  
  BS_Delete(bs);
}
```
Intercept incoming onfoot sync of player 
 ```pawn
new ID_PLAYER_SYNC = 207;
public OnPlayerReceivedPacket(playerid, packetid, BitStream:bs)
{
	if(packetid == ID_PLAYER_SYNC)
	{
	new lrkeys, udkeys, sampkeys,
	Float:pos[3], Float:quaternion[4],
	health, armor, weaponid, specialaction,
	Float:speed[3], Float:surfingoffsets[3],
	surfingvehid, animationid, animflags;
	 
    BS_IgnoreBits(bs, 8); // packet id 1 byte
	BS_ReadValue(bs, RNM_UINT16, lrkeys,
					 RNM_UINT16, udkeys,
				     RNM_UINT16, sampkeys,
					 RNM_FLOAT, pos[0],
					 RNM_FLOAT, pos[1],
					 RNM_FLOAT, pos[2],
					 RNM_FLOAT, quaternion[0],
					 RNM_FLOAT, quaternion[1],
                     RNM_FLOAT, quaternion[2],
                     RNM_FLOAT, quaternion[3],
	                 RNM_UINT8, health,
                     RNM_UINT8, armor,
	                 RNM_UINT8, weaponid,
                     RNM_UINT8, specialaction,
                     RNM_FLOAT, speed[0],
					 RNM_FLOAT, speed[1],
					 RNM_FLOAT, speed[2],
					 RNM_FLOAT, surfingoffsets[0],
					 RNM_FLOAT, surfingoffsets[1],
					 RNM_FLOAT, surfingoffsets[2],
					 RNM_UINT16, surfingvehid,
					 RNM_INT16, animationid,
					 RNM_INT16, animflags);
					  
	printf("lrkeys %d udkeys %d sampkeys %d\n" \
	       "pos %.2f %.2f %.2f\n" \
		   "quaternion %.2f %.2f %.2f %.2f\n" \
		   "health %d\n" \
		   "armor %d\n" \
		   "weaponid %d\n" \
		   "specialaction %d\n" \
		   "speed %.2f %.2f %.2f\n" \
		   "surfingoffsets %.2f %.2f %.2f\n" \
		   "surfingvehid %d\n" \
		   "anim %d flags %d",
		   lrkeys, udkeys, sampkeys,
	       pos[0], pos[1], pos[2],
	       quaternion[0], quaternion[1], quaternion[2], quaternion[3],
	       health, armor, weaponid, specialaction,
	       speed[0], speed[1], speed[2],
		   surfingoffsets[0], surfingoffsets[1], surfingoffsets[2],
	       surfingvehid, animationid, animflags
		   );
    }
	return 1;
}
```
Intercept incoming chat message rpc
```pawn
new RPC_Chat = 101;
public OnPlayerReceivedRPC(playerid, rpcid, BitStream:bs)
{
	if(rpcid == RPC_Chat) 
	{
	new message[256], len;
	
	BS_ReadValue(bs, RNM_UINT8, len,
					RNM_STRING, message, len);
					 
	printf("chat message from player %d: %s", playerid, message);
	}
	
	return 1;
} 
```
