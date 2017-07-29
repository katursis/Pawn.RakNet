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
        PR_OUTCOMING_PACKET
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
    };

    enum PR_PacketPriority
    {
        SYSTEM_PRIORITY,
        HIGH_PRIORITY,
        MEDIUM_PRIORITY,
        LOW_PRIORITY,
        NUMBER_OF_PRIORITIES
    };

    enum PR_PacketReliability
    {
        UNRELIABLE = 6,
        UNRELIABLE_SEQUENCED,
        RELIABLE,
        RELIABLE_ORDERED,
        RELIABLE_SEQUENCED
    };

    #if !defined __cplusplus
        public _pawnraknet_version = PAWNRAKNET_INCLUDE_VERSION;
        #pragma unused _pawnraknet_version

        #define IRPC:%0(%1) \
        forward pr_rir_%0(); \
        public pr_rir_%0() \
            PR_RegHandler(RPC_%0, "pr_ir_"#%0, PR_INCOMING_RPC); \
        forward pr_ir_%0(%1); \
        public pr_ir_%0(%1)

        #define IPacket:%0(%1) \
        forward pr_rip_%0(); \
        public pr_rip_%0() \
            PR_RegHandler(ID_%0, "pr_ip_"#%0, PR_INCOMING_PACKET); \
        forward pr_ip_%0(%1); \
        public pr_ip_%0(%1)

        #define ORPC:%0(%1) \
        forward pr_ror_%0(); \
        public pr_ror_%0() \
            PR_RegHandler(RPC_%0, "pr_or_"#%0, PR_OUTCOMING_RPC); \
        forward pr_or_%0(%1); \
        public pr_or_%0(%1)

        #define OPacket:%0(%1) \
        forward pr_rop_%0(); \
        public pr_rop_%0() \
            PR_RegHandler(ID_%0, "pr_op_"#%0, PR_OUTCOMING_PACKET); \
        forward pr_op_%0(%1); \
        public pr_op_%0(%1)

        #define IncomingRPC IRPC
        #define OutcomingRPC ORPC
        #define IncomingPacket IPacket
        #define OutcomingPacket OPacket

        // RPC/Packet ids you can get from:
        // https://github.com/P3ti/RakSAMP/blob/master/raknet/SAMP/SAMPRPC.cpp
        // https://github.com/P3ti/RakSAMP/blob/master/raknet/PacketEnumerations.h
        // player_id == -1 => send to all players (broadcast)

        native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);
        native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = HIGH_PRIORITY, PR_PacketReliability:reliability = RELIABLE_ORDERED);

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

        native PR_RegHandler(id, const publicname[], PR_HandlerType:type);

        forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
    #endif // !defined __cplusplus
#endif // PAWNRAKNET_INC_
