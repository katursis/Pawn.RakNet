/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 urShadow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if !defined PAWNRAKNET_INC_
    #define PAWNRAKNET_INC_

    #define PAWNRAKNET_INCLUDE_VERSION 10

    enum PR_HandlerType
    {
        PR_INCOMING_RPC,
        PR_INCOMING_PACKET,
        PR_OUTCOMING_RPC,
        PR_OUTCOMING_PACKET,
        PR_NUMBER_OF_HANDLER_TYPES
    };

    enum PR_ValueType
    {
        PR_INT8,
        PR_INT16,
        PR_INT32,
        PR_UINT8,
        PR_UINT16,
        PR_UINT32,
        PR_FLOAT,
        PR_BOOL,
        PR_STRING,

        // compressed
        PR_CINT8,
        PR_CINT16,
        PR_CINT32,
        PR_CUINT8,
        PR_CUINT16,
        PR_CUINT32,
        PR_CFLOAT,
        PR_CBOOL,

        PR_BITS
    };

    enum PR_PacketPriority
    {
        PR_SYSTEM_PRIORITY,
        PR_HIGH_PRIORITY,
        PR_MEDIUM_PRIORITY,
        PR_LOW_PRIORITY
    };

    enum PR_PacketReliability
    {
        PR_UNRELIABLE = 6,
        PR_UNRELIABLE_SEQUENCED,
        PR_RELIABLE,
        PR_RELIABLE_ORDERED,
        PR_RELIABLE_SEQUENCED
    };

    #if !defined __cplusplus
        public _pawnraknet_version = PAWNRAKNET_INCLUDE_VERSION;
        #pragma unused _pawnraknet_version

        public bool:_pawnraknet_is_gamemode = !defined FILTERSCRIPT;
        #pragma unused _pawnraknet_is_gamemode

        #define IRPC:%0(%1) \
        forward pr_rir_%0(); \
        public pr_rir_%0() \
            PR_RegHandler(%0, "pr_ir_"#%0, PR_INCOMING_RPC); \
        forward pr_ir_%0(%1); \
        public pr_ir_%0(%1)

        #define IPacket:%0(%1) \
        forward pr_rip_%0(); \
        public pr_rip_%0() \
            PR_RegHandler(%0, "pr_ip_"#%0, PR_INCOMING_PACKET); \
        forward pr_ip_%0(%1); \
        public pr_ip_%0(%1)

        #define ORPC:%0(%1) \
        forward pr_ror_%0(); \
        public pr_ror_%0() \
            PR_RegHandler(%0, "pr_or_"#%0, PR_OUTCOMING_RPC); \
        forward pr_or_%0(%1); \
        public pr_or_%0(%1)

        #define OPacket:%0(%1) \
        forward pr_rop_%0(); \
        public pr_rop_%0() \
            PR_RegHandler(%0, "pr_op_"#%0, PR_OUTCOMING_PACKET); \
        forward pr_op_%0(%1); \
        public pr_op_%0(%1)

        #define IncomingRPC IRPC
        #define OutcomingRPC ORPC
        #define IncomingPacket IPacket
        #define OutcomingPacket OPacket

        enum PR_OnFootSync
        {
            lrKeys,
            udKeys,
            sampKeys,
            Float:position[3],
            Float:quaternion[4],
            health, 
            armor, 
            weaponId, 
            specialAction,
            Float:speed[3], 
            Float:surfingOffsets[3],
            surfingVehicleId, 
            animationId, 
            animFlags
        };

        enum PR_InCarSync
        {
            vehicleId,
            lrKeys,
            udKeys,
            sampKeys,
            Float:quaternion[4],
            Float:position[3],
            Float:speed[3],
            Float:vehicleHealth,
            playerHealth,
            armor, 
            weaponId,
            siren,
            landingGearState,
            trailerId,
            Float:trainSpeed
        };

        enum PR_PassengerSync
        {
            vehicleId,
            seatId,
            currentWeapon,
            playerHealth,
            playerArmor,
            lrKeys,
            udKeys,
            sampKeys,
            Float:position[3]
        };

        enum PR_AimSync
        {
            camMode,
            Float:aimf1[3],
            Float:aimPos[3],
            Float:aimZ,
            camExtZoom,
            weaponState,
            unk
        };

        enum PR_BulletSync
        {
            type,
            targetId,
            Float:origin[3],
            Float:target[3],
            Float:center[3],
            weaponId
        };

        // RPC/Packet ids you can get from:
        // https://github.com/P3ti/RakSAMP/blob/master/raknet/SAMP/SAMPRPC.cpp
        // https://github.com/P3ti/RakSAMP/blob/master/raknet/PacketEnumerations.h
        // player_id == -1 => send to all players (broadcast)

        native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED);
        native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED);

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

        native BS_WriteValue(BitStream:bs, {PR_ValueType, Float, _}:...);
        native BS_ReadValue(BitStream:bs, {PR_ValueType, Float, _}:...);

        #define BS_ReadInt8(%0,%1) BS_ReadValue(%0, PR_INT8, %1)
        #define BS_ReadInt16(%0,%1) BS_ReadValue(%0, PR_INT16, %1)
        #define BS_ReadInt32(%0,%1) BS_ReadValue(%0, PR_INT32, %1)
        #define BS_ReadUint8(%0,%1) BS_ReadValue(%0, PR_UINT8, %1)
        #define BS_ReadUint16(%0,%1) BS_ReadValue(%0, PR_UINT16, %1)
        #define BS_ReadUint32(%0,%1) BS_ReadValue(%0, PR_UINT32, %1)
        #define BS_ReadFloat(%0,%1) BS_ReadValue(%0, PR_FLOAT, %1)
        #define BS_ReadBool(%0,%1) BS_ReadValue(%0, PR_BOOL, %1)
        #define BS_ReadString(%0,%1,%2) BS_ReadValue(%0, PR_STRING, %1, %2)

        #define BS_ReadCompressedInt8(%0,%1) BS_ReadValue(%0, PR_CINT8, %1)
        #define BS_ReadCompressedInt16(%0,%1) BS_ReadValue(%0, PR_CINT16, %1)
        #define BS_ReadCompressedInt32(%0,%1) BS_ReadValue(%0, PR_CINT32, %1)
        #define BS_ReadCompressedUint8(%0,%1) BS_ReadValue(%0, PR_CUINT8, %1)
        #define BS_ReadCompressedUint16(%0,%1) BS_ReadValue(%0, PR_CUINT16, %1)
        #define BS_ReadCompressedUint32(%0,%1) BS_ReadValue(%0, PR_CUINT32, %1)
        #define BS_ReadCompressedFloat(%0,%1) BS_ReadValue(%0, PR_CFLOAT, %1)
        #define BS_ReadCompressedBool(%0,%1) BS_ReadValue(%0, PR_CBOOL, %1)

        #define BS_WriteInt8(%0,%1) BS_WriteValue(%0, PR_INT8, %1)
        #define BS_WriteInt16(%0,%1) BS_WriteValue(%0, PR_INT16, %1)
        #define BS_WriteInt32(%0,%1) BS_WriteValue(%0, PR_INT32, %1)
        #define BS_WriteUint8(%0,%1) BS_WriteValue(%0, PR_UINT8, %1)
        #define BS_WriteUint16(%0,%1) BS_WriteValue(%0, PR_UINT16, %1)
        #define BS_WriteUint32(%0,%1) BS_WriteValue(%0, PR_UINT32, %1)
        #define BS_WriteFloat(%0,%1) BS_WriteValue(%0, PR_FLOAT, %1)
        #define BS_WriteBool(%0,%1) BS_WriteValue(%0, PR_BOOL, %1)
        #define BS_WriteString(%0,%1) BS_WriteValue(%0, PR_STRING, %1)

        #define BS_WriteCompressedInt8(%0,%1) BS_WriteValue(%0, PR_CINT8, %1)
        #define BS_WriteCompressedInt16(%0,%1) BS_WriteValue(%0, PR_CINT16, %1)
        #define BS_WriteCompressedInt32(%0,%1) BS_WriteValue(%0, PR_CINT32, %1)
        #define BS_WriteCompressedUint8(%0,%1) BS_WriteValue(%0, PR_CUINT8, %1)
        #define BS_WriteCompressedUint16(%0,%1) BS_WriteValue(%0, PR_CUINT16, %1)
        #define BS_WriteCompressedUint32(%0,%1) BS_WriteValue(%0, PR_CUINT32, %1)
        #define BS_WriteCompressedFloat(%0,%1) BS_WriteValue(%0, PR_CFLOAT, %1)
        #define BS_WriteCompressedBool(%0,%1) BS_WriteValue(%0, PR_CBOOL, %1)

        stock BS_ReadOnFootSync(BitStream:bs, data[PR_OnFootSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[lrKeys],
                PR_UINT16, data[udKeys],
                PR_UINT16, data[sampKeys],
                PR_FLOAT, data[position][0],
                PR_FLOAT, data[position][1],
                PR_FLOAT, data[position][2],
                PR_FLOAT, data[quaternion][0],
                PR_FLOAT, data[quaternion][1],
                PR_FLOAT, data[quaternion][2],
                PR_FLOAT, data[quaternion][3],
                PR_UINT8, data[health],
                PR_UINT8, data[armor],
                PR_UINT8, data[weaponId],
                PR_UINT8, data[specialAction],
                PR_FLOAT, data[speed][0],
                PR_FLOAT, data[speed][1],
                PR_FLOAT, data[speed][2],
                PR_FLOAT, data[surfingOffsets][0],
                PR_FLOAT, data[surfingOffsets][1],
                PR_FLOAT, data[surfingOffsets][2],
                PR_UINT16, data[surfingVehicleId],
                PR_INT16, data[animationId],
                PR_INT16, data[animFlags]
            );
        }

        stock BS_ReadInCarSync(BitStream:bs, data[PR_InCarSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[vehicleId],
                PR_UINT16, data[lrKeys],
                PR_UINT16, data[udKeys],
                PR_UINT16, data[sampKeys],
                PR_FLOAT, data[quaternion][0],
                PR_FLOAT, data[quaternion][1],
                PR_FLOAT, data[quaternion][2],
                PR_FLOAT, data[quaternion][3],
                PR_FLOAT, data[position][0],
                PR_FLOAT, data[position][1],
                PR_FLOAT, data[position][2],
                PR_FLOAT, data[speed][0],
                PR_FLOAT, data[speed][1],
                PR_FLOAT, data[speed][2],
                PR_FLOAT, data[vehicleHealth],
                PR_UINT8, data[playerHealth],
                PR_UINT8, data[armor],
                PR_UINT8, data[weaponId],
                PR_UINT8, data[siren],
                PR_UINT8, data[landingGearState],
                PR_UINT16, data[trailerId],
                PR_FLOAT, data[trainSpeed]
            );
        }

        stock BS_ReadPassengerSync(BitStream:bs, data[PR_PassengerSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[vehicleId],
                PR_UINT8, data[seatId],
                PR_UINT8, data[currentWeapon],
                PR_UINT8, data[playerHealth],
                PR_UINT8, data[playerArmor],
                PR_UINT16, data[lrKeys],
                PR_UINT16, data[udKeys],
                PR_UINT16, data[sampKeys],
                PR_FLOAT, data[position][0],
                PR_FLOAT, data[position][1],
                PR_FLOAT, data[position][2]
            );
        }

        stock BS_ReadAimSync(BitStream:bs, data[PR_AimSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT8, data[camMode],
                PR_FLOAT, data[aimf1][0],
                PR_FLOAT, data[aimf1][1],
                PR_FLOAT, data[aimf1][2],
                PR_FLOAT, data[aimPos][0],
                PR_FLOAT, data[aimPos][1],
                PR_FLOAT, data[aimPos][2],
                PR_FLOAT, data[aimZ],

                // read bit fields in reverse order
                PR_BITS, data[weaponState], 2,
                PR_BITS, data[camExtZoom], 6,

                PR_UINT8, data[unk]
            );
        }

        stock BS_ReadBulletSync(BitStream:bs, data[PR_BulletSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT8, data[type],
                PR_UINT16, data[targetId],
                PR_FLOAT, data[origin][0],
                PR_FLOAT, data[origin][1],
                PR_FLOAT, data[origin][2],
                PR_FLOAT, data[target][0],
                PR_FLOAT, data[target][1],
                PR_FLOAT, data[target][2],
                PR_FLOAT, data[center][0],
                PR_FLOAT, data[center][1],
                PR_FLOAT, data[center][2],
                PR_UINT8, data[weaponId]
            );
        }

        native PR_RegHandler(id, const publicname[], PR_HandlerType:type);

        forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
    #endif // !defined __cplusplus
#endif // PAWNRAKNET_INC_
