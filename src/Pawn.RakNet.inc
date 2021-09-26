/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 katursis
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

    #if !defined PACK_PLUGIN_VERSION
        #define PACK_PLUGIN_VERSION(%0,%1,%2) (((%0) << 16) | ((%1) << 8) | (%2))
    #endif

    #define PAWNRAKNET_VERSION PACK_PLUGIN_VERSION(1, 5, 1)
    #define PAWNRAKNET_INCLUDE_VERSION PAWNRAKNET_VERSION // backward compatibility

    #define PR_MAX_HANDLERS 256

    enum PR_EventType
    {
        PR_INCOMING_PACKET,
        PR_INCOMING_RPC,
        PR_OUTGOING_PACKET,
        PR_OUTGOING_RPC,
        PR_INCOMING_RAW_PACKET,
        PR_INCOMING_INTERNAL_PACKET,
        PR_OUTGOING_INTERNAL_PACKET,
        PR_INCOMING_CUSTOM_RPC,

        PR_NUMBER_OF_EVENT_TYPES,

        // backward compatibility
        PR_OUTCOMING_PACKET = PR_OUTGOING_PACKET,
        PR_OUTCOMING_RPC = PR_OUTGOING_RPC,
    };

    enum PR_ValueType
    {
        PR_INT8, // char
        PR_INT16, // short
        PR_INT32, // int
        PR_UINT8, // unsigned char
        PR_UINT16, // unsigned short
        PR_UINT32, // unsigned int
        PR_FLOAT, // float
        PR_BOOL, // bool
        PR_STRING, // char[]

        // compressed types. For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
        PR_CINT8,
        PR_CINT16,
        PR_CINT32,
        PR_CUINT8,
        PR_CUINT16,
        PR_CUINT32,
        PR_CFLOAT, // read/write a float using 2 bytes instead of 4. Lossy. The range must be between -1 and +1
        PR_CBOOL,
        PR_CSTRING,

        PR_BITS,
        PR_FLOAT3, // float * 3
        PR_FLOAT4, // float * 4
        PR_VECTOR, // read/write a float vector, using 10 bytes instead of 12. Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important
        PR_NORM_QUAT, // read/write a normalized float quaternion in 6 bytes + 4 bits instead of 16 bytes. Slightly lossy

        PR_STRING8, // unsigned char + char[]
        PR_STRING32, // unsigned int + char[]

        PR_IGNORE_BITS, // shift read/write pointer
    };

    enum PR_PacketPriority
    {
        PR_SYSTEM_PRIORITY, // used by RakNet to send above-high priority messages
        PR_HIGH_PRIORITY, // high priority messages are send before medium priority messages
        PR_MEDIUM_PRIORITY, // medium priority messages are send before low priority messages
        PR_LOW_PRIORITY, // low priority messages are only sent when no other messages are waiting
    };

    enum PR_PacketReliability
    {
        PR_UNRELIABLE = 6, // same as regular UDP, except that it will also discard duplicate datagrams. RakNet adds (6 to 17) + 21 bits of overhead, 16 of which is used to detect duplicate packets and 6 to 17 of which is used for message length
        PR_UNRELIABLE_SEQUENCED, // regular UDP with a sequence counter. Out of order messages will be discarded. This adds an additional 13 bits on top what is used for UNRELIABLE
        PR_RELIABLE, // the message is sent reliably, but not necessarily in any order. Same overhead as UNRELIABLE
        PR_RELIABLE_ORDERED, // this message is reliable and will arrive in the order you sent it. Messages will be delayed while waiting for out of order messages. Same overhead as UNRELIABLE_SEQUENCED
        PR_RELIABLE_SEQUENCED, // this message is reliable and will arrive in the sequence you sent it. Out or order messages will be dropped. Same overhead as UNRELIABLE_SEQUENCED
    };

    #if !defined __cplusplus
        public _pawnraknet_version = PAWNRAKNET_VERSION;
        #pragma unused _pawnraknet_version

        public bool:_pawnraknet_is_gamemode = !defined FILTERSCRIPT;
        #pragma unused _pawnraknet_is_gamemode

        #define PR_BITS_TO_BYTES(%0) (((%0)+7)>>3)
        #define PR_BYTES_TO_BITS(%0) ((%0)<<3)

        #define PR_Handler<%0,%1>:%2(%3) \
        forward pr_r%1_%2(); \
        public pr_r%1_%2() PR_RegHandler(%2,"pr_"#%1"_"#%2,%0); \
        forward pr_%1_%2(%3); \
        public pr_%1_%2(%3)

        #define IPacket:%0(%1) PR_Handler<PR_INCOMING_PACKET,ip>:%0(%1)
        #define IRPC:%0(%1) PR_Handler<PR_INCOMING_RPC,ir>:%0(%1)
        #define OPacket:%0(%1) PR_Handler<PR_OUTGOING_PACKET,op>:%0(%1)
        #define ORPC:%0(%1) PR_Handler<PR_OUTGOING_RPC,or>:%0(%1)
        #define IRawPacket:%0(%1) PR_Handler<PR_INCOMING_RAW_PACKET,irp>:%0(%1)
        #define IInternalPacket:%0(%1) PR_Handler<PR_INCOMING_INTERNAL_PACKET,iip>:%0(%1)
        #define OInternalPacket:%0(%1) PR_Handler<PR_OUTGOING_INTERNAL_PACKET,oip>:%0(%1)
        #define ICustomRPC:%0(%1) PR_Handler<PR_INCOMING_CUSTOM_RPC,icr>:%0(%1)

        #define IncomingPacket IPacket
        #define IncomingRPC IRPC
        #define OutgoingPacket OPacket
        #define OutgoingRPC ORPC
        #define IncomingRawPacket IRawPacket
        #define IncomingInternalPacket IInternalPacket
        #define OutgoingInternalPacket OInternalPacket
        #define IncomingCustomRPC ICustomRPC

        // backward compatibility
        #define OutcomingPacket OutgoingPacket
        #define OutcomingRPC OutgoingRPC

        #define PR_MAX_WEAPON_SLOTS 13

        enum PR_OnFootSync
        {
            PR_lrKey,
            PR_udKey,
            PR_keys,
            Float:PR_position[3],
            Float:PR_quaternion[4],
            PR_health,
            PR_armour,
            PR_weaponId,
            PR_additionalKey,
            PR_specialAction,
            Float:PR_velocity[3],
            Float:PR_surfingOffsets[3],
            PR_surfingVehicleId,
            PR_animationId,
            PR_animationFlags,
        };

        enum PR_InCarSync
        {
            PR_vehicleId,
            PR_lrKey,
            PR_udKey,
            PR_keys,
            Float:PR_quaternion[4],
            Float:PR_position[3],
            Float:PR_velocity[3],
            Float:PR_vehicleHealth,
            PR_playerHealth,
            PR_armour,
            PR_weaponId,
            PR_additionalKey,
            PR_sirenState,
            PR_landingGearState,
            PR_trailerId,
            Float:PR_trainSpeed,
        };

        enum PR_TrailerSync
        {
            PR_trailerId,
            Float:PR_position[3],
            Float:PR_quaternion[4],
            Float:PR_velocity[3],
            Float:PR_angularVelocity[3],
        };

        enum PR_PassengerSync
        {
            PR_vehicleId,
            PR_seatId,
            PR_driveBy,
            PR_weaponId,
            PR_additionalKey,
            PR_playerHealth,
            PR_playerArmour,
            PR_lrKey,
            PR_udKey,
            PR_keys,
            Float:PR_position[3],
        };

        enum PR_UnoccupiedSync
        {
            PR_vehicleId,
            PR_seatId,
            Float:PR_roll[3],
            Float:PR_direction[3],
            Float:PR_position[3],
            Float:PR_velocity[3],
            Float:PR_angularVelocity[3],
            Float:PR_vehicleHealth,
        };

        enum PR_AimSync
        {
            PR_camMode,
            Float:PR_camFrontVec[3],
            Float:PR_camPos[3],
            Float:PR_aimZ,
            PR_camZoom,
            PR_weaponState,
            PR_aspectRatio,
        };

        enum PR_BulletSync
        {
            PR_hitType,
            PR_hitId,
            Float:PR_origin[3],
            Float:PR_hitPos[3],
            Float:PR_offsets[3],
            PR_weaponId,
        };

        enum PR_SpectatingSync
        {
            PR_lrKey,
            PR_udKey,
            PR_keys,
            Float:PR_position[3],
        };

        enum PR_MarkersSync
        {
            PR_numberOfPlayers,
            PR_playerIsActive[MAX_PLAYERS],
            PR_playerPositionX[MAX_PLAYERS],
            PR_playerPositionY[MAX_PLAYERS],
            PR_playerPositionZ[MAX_PLAYERS],

            PR_playerIsParticipant[MAX_PLAYERS], // Pawn.RakNet internal helping field
        };

        enum PR_WeaponsUpdate
        {
            PR_targetId,
            PR_targetActorId,
            PR_slotWeaponId[PR_MAX_WEAPON_SLOTS],
            PR_slotWeaponAmmo[PR_MAX_WEAPON_SLOTS],

            PR_slotUpdated[PR_MAX_WEAPON_SLOTS], // Pawn.RakNet internal helping field
        };

        enum PR_StatsUpdate
        {
            PR_money,
            PR_drunkLevel,
        };

        enum PR_RconCommand
        {
            PR_command[256],
        };

        native PR_Init(); // internal

        native PR_SendPacket(BitStream:bs, playerid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0);
        native PR_SendRPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0);

        #pragma deprecated Use PR_SendPacket instead
        native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0) = PR_SendPacket;
        #pragma deprecated Use PR_SendRPC instead
        native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0) = PR_SendRPC;

        native PR_EmulateIncomingPacket(BitStream:bs, playerid);
        native PR_EmulateIncomingRPC(BitStream:bs, playerid, rpcid);

        #pragma deprecated Use PR_EmulateIncomingPacket instead
        native BS_EmulateIncomingPacket(BitStream:bs, playerid) = PR_EmulateIncomingPacket;
        #pragma deprecated Use PR_EmulateIncomingRPC instead
        native BS_EmulateIncomingRPC(BitStream:bs, playerid, rpcid) = PR_EmulateIncomingRPC;

        native BitStream:BS_New();
        native BitStream:BS_NewCopy(BitStream:bs);
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
        native BS_GetNumberOfBitsAllocated(BitStream:bs, &number);

        native BS_WriteValue(BitStream:bs, {PR_ValueType, Float, _}:...);
        native BS_ReadValue(BitStream:bs, {PR_ValueType, Float, _}:...);

        native PR_RegHandler(eventid, const publicname[], PR_EventType:type);

        #define BS_ReadInt8(%0,%1) BS_ReadValue(%0,PR_INT8,%1)
        #define BS_ReadInt16(%0,%1) BS_ReadValue(%0,PR_INT16,%1)
        #define BS_ReadInt32(%0,%1) BS_ReadValue(%0,PR_INT32,%1)
        #define BS_ReadUint8(%0,%1) BS_ReadValue(%0,PR_UINT8,%1)
        #define BS_ReadUint16(%0,%1) BS_ReadValue(%0,PR_UINT16,%1)
        #define BS_ReadUint32(%0,%1) BS_ReadValue(%0,PR_UINT32,%1)
        #define BS_ReadFloat(%0,%1) BS_ReadValue(%0,PR_FLOAT,%1)
        #define BS_ReadBool(%0,%1) BS_ReadValue(%0,PR_BOOL,%1)
        #define BS_ReadString(%0,%1,%2) BS_ReadValue(%0,PR_STRING,%1,%2)

        #define BS_ReadCompressedInt8(%0,%1) BS_ReadValue(%0,PR_CINT8,%1)
        #define BS_ReadCompressedInt16(%0,%1) BS_ReadValue(%0,PR_CINT16,%1)
        #define BS_ReadCompressedInt32(%0,%1) BS_ReadValue(%0,PR_CINT32,%1)
        #define BS_ReadCompressedUint8(%0,%1) BS_ReadValue(%0,PR_CUINT8,%1)
        #define BS_ReadCompressedUint16(%0,%1) BS_ReadValue(%0,PR_CUINT16,%1)
        #define BS_ReadCompressedUint32(%0,%1) BS_ReadValue(%0,PR_CUINT32,%1)
        #define BS_ReadCompressedFloat(%0,%1) BS_ReadValue(%0,PR_CFLOAT,%1)
        #define BS_ReadCompressedBool(%0,%1) BS_ReadValue(%0,PR_CBOOL,%1)
        #define BS_ReadCompressedString(%0,%1,%2) BS_ReadValue(%0,PR_CSTRING,%1,%2)

        #define BS_ReadBits(%0,%1,%2) BS_ReadValue(%0,PR_BITS,%1,%2)
        #define BS_ReadFloat3(%0,%1) BS_ReadValue(%0,PR_FLOAT3,%1)
        #define BS_ReadFloat4(%0,%1) BS_ReadValue(%0,PR_FLOAT4,%1)
        #define BS_ReadVector(%0,%1) BS_ReadValue(%0,PR_VECTOR,%1)
        #define BS_ReadNormQuat(%0,%1) BS_ReadValue(%0,PR_NORM_QUAT,%1)

        #define BS_ReadString8(%0,%1) BS_ReadValue(%0,PR_STRING8,%1,sizeof(%1))
        #define BS_ReadString32(%0,%1) BS_ReadValue(%0,PR_STRING32,%1,sizeof(%1))

        #define BS_WriteInt8(%0,%1) BS_WriteValue(%0,PR_INT8,%1)
        #define BS_WriteInt16(%0,%1) BS_WriteValue(%0,PR_INT16,%1)
        #define BS_WriteInt32(%0,%1) BS_WriteValue(%0,PR_INT32,%1)
        #define BS_WriteUint8(%0,%1) BS_WriteValue(%0,PR_UINT8,%1)
        #define BS_WriteUint16(%0,%1) BS_WriteValue(%0,PR_UINT16,%1)
        #define BS_WriteUint32(%0,%1) BS_WriteValue(%0,PR_UINT32,%1)
        #define BS_WriteFloat(%0,%1) BS_WriteValue(%0,PR_FLOAT,%1)
        #define BS_WriteBool(%0,%1) BS_WriteValue(%0,PR_BOOL,%1)
        #define BS_WriteString(%0,%1) BS_WriteValue(%0,PR_STRING,%1)

        #define BS_WriteCompressedInt8(%0,%1) BS_WriteValue(%0,PR_CINT8,%1)
        #define BS_WriteCompressedInt16(%0,%1) BS_WriteValue(%0,PR_CINT16,%1)
        #define BS_WriteCompressedInt32(%0,%1) BS_WriteValue(%0,PR_CINT32,%1)
        #define BS_WriteCompressedUint8(%0,%1) BS_WriteValue(%0,PR_CUINT8,%1)
        #define BS_WriteCompressedUint16(%0,%1) BS_WriteValue(%0,PR_CUINT16,%1)
        #define BS_WriteCompressedUint32(%0,%1) BS_WriteValue(%0,PR_CUINT32,%1)
        #define BS_WriteCompressedFloat(%0,%1) BS_WriteValue(%0,PR_CFLOAT,%1)
        #define BS_WriteCompressedBool(%0,%1) BS_WriteValue(%0,PR_CBOOL,%1)
        #define BS_WriteCompressedString(%0,%1) BS_WriteValue(%0,PR_CSTRING,%1)

        #define BS_WriteBits(%0,%1,%2) BS_WriteValue(%0,PR_BITS,%1,%2)
        #define BS_WriteFloat3(%0,%1) BS_WriteValue(%0,PR_FLOAT3,%1)
        #define BS_WriteFloat4(%0,%1) BS_WriteValue(%0,PR_FLOAT4,%1)
        #define BS_WriteVector(%0,%1) BS_WriteValue(%0,PR_VECTOR,%1)
        #define BS_WriteNormQuat(%0,%1) BS_WriteValue(%0,PR_NORM_QUAT,%1)

        #define BS_WriteString8(%0,%1) BS_WriteValue(%0,PR_STRING8,%1)
        #define BS_WriteString32(%0,%1) BS_WriteValue(%0,PR_STRING32,%1)

        #define PR_EXPAND_ARR3(%0) %0[0],%0[1],%0[2]
        #define PR_EXPAND_ARR4(%0) %0[0],%0[1],%0[2],%0[3]

        stock BS_PackAspectRatio(Float:value)
        {
            return floatround((value - 1.0) * 255.0);
        }

        stock Float:BS_UnpackAspectRatio(value)
        {
            return float(value) / 255.0 + 1.0;
        }

        stock BS_PackCameraZoom(Float:value)
        {
            return floatround(((value - 35.0) / 35.0) * 63.0);
        }

        stock Float:BS_UnpackCameraZoom(value)
        {
            return (float(value) / 63.0) * 35.0 + 35.0;
        }

        stock BS_PackHealthArmour(health, armour, &healthArmour)
        {
            if (health > 0 && health < 100) {
                healthArmour = (health / 7) << 4;
            } else if (health >= 100) {
                healthArmour = 0xF0; // 0xF << 4
            } else {
                healthArmour = 0;
            }

            if (armour > 0 && armour < 100) {
                healthArmour |= (armour / 7);
            } else if (armour >= 100) {
                healthArmour |= 0xF;
            }
        }

        stock BS_UnpackHealthArmour(healthArmour, &health, &armour)
        {
            health = healthArmour >> 4;
            if (health == 0xF) {
                health = 100;
            } else {
                health *= 7;
            }

            armour = healthArmour & 0xF;
            if (armour == 0xF) {
                armour = 100;
            } else {
                armour *= 7;
            }
        }

        stock BS_ReadOnFootSync(BitStream:bs, data[PR_OnFootSync], outgoing = false)
        {
            if (outgoing) {
                new hasLeftRight, hasUpDown, hasSurfInfo, hasAnimation, healthArmour;

                BS_ReadValue(bs, PR_BOOL, hasLeftRight);

                if (hasLeftRight) {
                    BS_ReadValue(bs, PR_UINT16, data[PR_lrKey]);
                } else {
                    data[PR_lrKey] = 0;
                }

                BS_ReadValue(bs, PR_BOOL, hasUpDown);

                if (hasUpDown) {
                    BS_ReadValue(bs, PR_UINT16, data[PR_udKey]);
                } else {
                    data[PR_udKey] = 0;
                }

                BS_ReadValue(
                    bs,
                    PR_UINT16, data[PR_keys],
                    PR_FLOAT3, data[PR_position],
                    PR_NORM_QUAT, data[PR_quaternion],
                    PR_UINT8, healthArmour,
                    PR_UINT8, data[PR_weaponId],
                    PR_UINT8, data[PR_specialAction],
                    PR_VECTOR, data[PR_velocity],
                    PR_BOOL, hasSurfInfo
                );

                BS_UnpackHealthArmour(healthArmour, data[PR_health], data[PR_armour]);

                if (hasSurfInfo) {
                    BS_ReadValue(
                        bs,
                        PR_UINT16, data[PR_surfingVehicleId],
                        PR_FLOAT3, data[PR_surfingOffsets]
                    );
                } else {
                    data[PR_surfingVehicleId] = 0;
                }

                BS_ReadValue(bs, PR_BOOL, hasAnimation);

                if (hasAnimation) {
                    BS_ReadValue(
                        bs,
                        PR_INT16, data[PR_animationId],
                        PR_INT16, data[PR_animationFlags]
                    );
                } else {
                    data[PR_animationId] = 0;
                    data[PR_animationFlags] = 0;
                }
            } else {
                BS_ReadValue(
                    bs,
                    PR_UINT16, data[PR_lrKey],
                    PR_UINT16, data[PR_udKey],
                    PR_UINT16, data[PR_keys],
                    PR_FLOAT3, data[PR_position],
                    PR_FLOAT4, data[PR_quaternion],
                    PR_UINT8, data[PR_health],
                    PR_UINT8, data[PR_armour],
                    PR_BITS, data[PR_additionalKey], 2,
                    PR_BITS, data[PR_weaponId], 6,
                    PR_UINT8, data[PR_specialAction],
                    PR_FLOAT3, data[PR_velocity],
                    PR_FLOAT3, data[PR_surfingOffsets],
                    PR_UINT16, data[PR_surfingVehicleId],
                    PR_INT16, data[PR_animationId],
                    PR_INT16, data[PR_animationFlags]
                );
            }
        }

        stock BS_ReadInCarSync(BitStream:bs, data[PR_InCarSync], outgoing = false)
        {
            if (outgoing) {
                new vehicleHealth, healthArmour;

                BS_ReadValue(
                    bs,
                    PR_UINT16, data[PR_vehicleId],
                    PR_UINT16, data[PR_lrKey],
                    PR_UINT16, data[PR_udKey],
                    PR_UINT16, data[PR_keys],
                    PR_NORM_QUAT, data[PR_quaternion],
                    PR_FLOAT3, data[PR_position],
                    PR_VECTOR, data[PR_velocity],
                    PR_UINT16, vehicleHealth,
                    PR_UINT8, healthArmour,
                    PR_UINT8, data[PR_weaponId],
                    PR_BOOL, data[PR_sirenState],
                    PR_BOOL, data[PR_landingGearState]
                );

                data[PR_vehicleHealth] = float(vehicleHealth);

                BS_UnpackHealthArmour(healthArmour, data[PR_playerHealth], data[PR_armour]);

                new hasTrainSpeed, hasTrailer;

                BS_ReadValue(bs, PR_BOOL, hasTrainSpeed);

                if (hasTrainSpeed) {
                    BS_ReadValue(bs, PR_FLOAT, data[PR_trainSpeed]);
                } else {
                    data[PR_trainSpeed] = 0.0;
                }

                BS_ReadValue(bs, PR_BOOL, hasTrailer);

                if (hasTrailer) {
                    BS_ReadValue(bs, PR_UINT16, data[PR_trailerId]);
                } else {
                    data[PR_trailerId] = 0;
                }
            } else {
                BS_ReadValue(
                    bs,
                    PR_UINT16, data[PR_vehicleId],
                    PR_UINT16, data[PR_lrKey],
                    PR_UINT16, data[PR_udKey],
                    PR_UINT16, data[PR_keys],
                    PR_FLOAT4, data[PR_quaternion],
                    PR_FLOAT3, data[PR_position],
                    PR_FLOAT3, data[PR_velocity],
                    PR_FLOAT, data[PR_vehicleHealth],
                    PR_UINT8, data[PR_playerHealth],
                    PR_UINT8, data[PR_armour],
                    PR_BITS, data[PR_additionalKey], 2,
                    PR_BITS, data[PR_weaponId], 6,
                    PR_UINT8, data[PR_sirenState],
                    PR_UINT8, data[PR_landingGearState],
                    PR_UINT16, data[PR_trailerId],
                    PR_FLOAT, data[PR_trainSpeed]
                );
            }
        }

        stock BS_ReadTrailerSync(BitStream:bs, data[PR_TrailerSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[PR_trailerId],
                PR_FLOAT3, data[PR_position],
                PR_FLOAT4, data[PR_quaternion],
                PR_FLOAT3, data[PR_velocity],
                PR_FLOAT3, data[PR_angularVelocity]
            );
        }

        stock BS_ReadPassengerSync(BitStream:bs, data[PR_PassengerSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[PR_vehicleId],
                PR_BITS, data[PR_driveBy], 2,
                PR_BITS, data[PR_seatId], 6,
                PR_BITS, data[PR_additionalKey], 2,
                PR_BITS, data[PR_weaponId], 6,
                PR_UINT8, data[PR_playerHealth],
                PR_UINT8, data[PR_playerArmour],
                PR_UINT16, data[PR_lrKey],
                PR_UINT16, data[PR_udKey],
                PR_UINT16, data[PR_keys],
                PR_FLOAT3, data[PR_position]
            );
        }

        stock BS_ReadUnoccupiedSync(BitStream:bs, data[PR_UnoccupiedSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[PR_vehicleId],
                PR_UINT8, data[PR_seatId],
                PR_FLOAT3, data[PR_roll],
                PR_FLOAT3, data[PR_direction],
                PR_FLOAT3, data[PR_position],
                PR_FLOAT3, data[PR_velocity],
                PR_FLOAT3, data[PR_angularVelocity],
                PR_FLOAT, data[PR_vehicleHealth]
            );
        }

        stock BS_ReadAimSync(BitStream:bs, data[PR_AimSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT8, data[PR_camMode],
                PR_FLOAT3, data[PR_camFrontVec],
                PR_FLOAT3, data[PR_camPos],
                PR_FLOAT, data[PR_aimZ],
                PR_BITS, data[PR_weaponState], 2,
                PR_BITS, data[PR_camZoom], 6,
                PR_UINT8, data[PR_aspectRatio]
            );
        }

        stock BS_ReadBulletSync(BitStream:bs, data[PR_BulletSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT8, data[PR_hitType],
                PR_UINT16, data[PR_hitId],
                PR_FLOAT3, data[PR_origin],
                PR_FLOAT3, data[PR_hitPos],
                PR_FLOAT3, data[PR_offsets],
                PR_UINT8, data[PR_weaponId]
            );
        }

        stock BS_ReadSpectatingSync(BitStream:bs, data[PR_SpectatingSync])
        {
            BS_ReadValue(
                bs,
                PR_UINT16, data[PR_lrKey],
                PR_UINT16, data[PR_udKey],
                PR_UINT16, data[PR_keys],
                PR_FLOAT3, data[PR_position]
            );
        }

        stock BS_ReadMarkersSync(BitStream:bs, data[PR_MarkersSync])
        {
            new numberOfPlayers, playerId, isActive;

            BS_ReadInt32(bs, numberOfPlayers);

            if (numberOfPlayers < 0 || numberOfPlayers > MAX_PLAYERS) {
                return;
            }

            data[PR_numberOfPlayers] = numberOfPlayers;

            for (new i; i < numberOfPlayers; i++) {
                playerId = isActive = 0;

                BS_ReadUint16(bs, playerId);

                if (playerId >= MAX_PLAYERS) {
                    return;
                }

                data[PR_playerIsParticipant][playerId] = true;

                BS_ReadCompressedBool(bs, isActive);

                if (isActive) {
                    data[PR_playerIsActive][playerId] = true;

                    BS_ReadValue(
                        bs,
                        PR_INT16, data[PR_playerPositionX][playerId],
                        PR_INT16, data[PR_playerPositionY][playerId],
                        PR_INT16, data[PR_playerPositionZ][playerId]
                    );
                }
            }
        }

        stock BS_ReadWeaponsUpdate(BitStream:bs, data[PR_WeaponsUpdate])
        {
            new numberOfBytes, numberOfSlots, slotId, weaponId, ammo;

            BS_GetNumberOfBytesUsed(bs, numberOfBytes);

            if (numberOfBytes > 5) {
                numberOfSlots = (numberOfBytes - 5) / 4;
            }

            BS_ReadValue(
                bs,
                PR_UINT16, data[PR_targetId],
                PR_UINT16, data[PR_targetActorId]
            );

            while (numberOfSlots--) {
                slotId = weaponId = ammo = 0;

                BS_ReadValue(
                    bs,
                    PR_UINT8, slotId,
                    PR_UINT8, weaponId,
                    PR_UINT16, ammo
                );

                if (slotId < PR_MAX_WEAPON_SLOTS) {
                    data[PR_slotWeaponId][slotId] = weaponId;
                    data[PR_slotWeaponAmmo][slotId] = ammo;
                    data[PR_slotUpdated][slotId] = true;
                }
            }
        }

        stock BS_ReadStatsUpdate(BitStream:bs, data[PR_StatsUpdate])
        {
            BS_ReadValue(
                bs,
                PR_INT32, data[PR_money],
                PR_INT32, data[PR_drunkLevel]
            );
        }

        stock BS_ReadRconCommand(BitStream:bs, data[PR_RconCommand])
        {
            BS_ReadValue(
                bs,
                PR_STRING32, data[PR_command], sizeof(data[PR_command])
            );
        }

        stock BS_WriteOnFootSync(BitStream:bs, data[PR_OnFootSync], outgoing = false)
        {
            if (outgoing) {
                new healthArmour;

                if (data[PR_lrKey]) {
                    BS_WriteValue(
                        bs,
                        PR_BOOL, true,
                        PR_UINT16, data[PR_lrKey]
                    );
                } else {
                    BS_WriteValue(bs, PR_BOOL, false);
                }

                if (data[PR_udKey]) {
                    BS_WriteValue(
                        bs,
                        PR_BOOL, true,
                        PR_UINT16, data[PR_udKey]
                    );
                } else {
                    BS_WriteValue(bs, PR_BOOL, false);
                }

                BS_PackHealthArmour(data[PR_health], data[PR_armour], healthArmour);

                BS_WriteValue(
                    bs,
                    PR_UINT16, data[PR_keys],
                    PR_FLOAT3, data[PR_position],
                    PR_NORM_QUAT, data[PR_quaternion],
                    PR_UINT8, healthArmour,
                    PR_UINT8, data[PR_weaponId],
                    PR_UINT8, data[PR_specialAction],
                    PR_VECTOR, data[PR_velocity]
                );

                if (data[PR_surfingVehicleId]) {
                    BS_WriteValue(
                        bs,
                        PR_BOOL, true,
                        PR_UINT16, data[PR_surfingVehicleId],
                        PR_FLOAT3, data[PR_surfingOffsets]
                    );
                } else {
                    BS_WriteValue(bs, PR_BOOL, false);
                }

                if (data[PR_animationId] || data[PR_animationFlags]) {
                    BS_WriteValue(
                        bs,
                        PR_BOOL, true,
                        PR_INT16, data[PR_animationId],
                        PR_INT16, data[PR_animationFlags]
                    );
                } else {
                    BS_WriteValue(bs, PR_BOOL, false);
                }
            } else {
                BS_WriteValue(
                    bs,
                    PR_UINT16, data[PR_lrKey],
                    PR_UINT16, data[PR_udKey],
                    PR_UINT16, data[PR_keys],
                    PR_FLOAT3, data[PR_position],
                    PR_FLOAT4, data[PR_quaternion],
                    PR_UINT8, data[PR_health],
                    PR_UINT8, data[PR_armour],
                    PR_BITS, data[PR_additionalKey], 2,
                    PR_BITS, data[PR_weaponId], 6,
                    PR_UINT8, data[PR_specialAction],
                    PR_FLOAT3, data[PR_velocity],
                    PR_FLOAT3, data[PR_surfingOffsets],
                    PR_UINT16, data[PR_surfingVehicleId],
                    PR_INT16, data[PR_animationId],
                    PR_INT16, data[PR_animationFlags]
                );
            }
        }

        stock BS_WriteInCarSync(BitStream:bs, data[PR_InCarSync], outgoing = false)
        {
            if (outgoing) {
                new healthArmour;

                BS_PackHealthArmour(data[PR_playerHealth], data[PR_armour], healthArmour);

                BS_WriteValue(
                    bs,
                    PR_UINT16, data[PR_vehicleId],
                    PR_UINT16, data[PR_lrKey],
                    PR_UINT16, data[PR_udKey],
                    PR_UINT16, data[PR_keys],
                    PR_NORM_QUAT, data[PR_quaternion],
                    PR_FLOAT3, data[PR_position],
                    PR_VECTOR, data[PR_velocity],
                    PR_UINT16, floatround(data[PR_vehicleHealth]),
                    PR_UINT8, healthArmour,
                    PR_UINT8, data[PR_weaponId],
                    PR_BOOL, data[PR_sirenState],
                    PR_BOOL, data[PR_landingGearState]
                );

                if (data[PR_trainSpeed]) {
                    BS_WriteValue(bs,
                        PR_BOOL, true,
                        PR_FLOAT, data[PR_trainSpeed]
                    );
                } else {
                    BS_WriteValue(bs, PR_BOOL, false);
                }

                if (data[PR_trailerId]) {
                    BS_WriteValue(bs,
                        PR_BOOL, true,
                        PR_UINT16, data[PR_trailerId]
                    );
                } else {
                    BS_WriteValue(bs, PR_BOOL, false);
                }
            } else {
                BS_WriteValue(
                    bs,
                    PR_UINT16, data[PR_vehicleId],
                    PR_UINT16, data[PR_lrKey],
                    PR_UINT16, data[PR_udKey],
                    PR_UINT16, data[PR_keys],
                    PR_FLOAT4, data[PR_quaternion],
                    PR_FLOAT3, data[PR_position],
                    PR_FLOAT3, data[PR_velocity],
                    PR_FLOAT, data[PR_vehicleHealth],
                    PR_UINT8, data[PR_playerHealth],
                    PR_UINT8, data[PR_armour],
                    PR_BITS, data[PR_additionalKey], 2,
                    PR_BITS, data[PR_weaponId], 6,
                    PR_UINT8, data[PR_sirenState],
                    PR_UINT8, data[PR_landingGearState],
                    PR_UINT16, data[PR_trailerId],
                    PR_FLOAT, data[PR_trainSpeed]
                );
            }
        }

        stock BS_WriteTrailerSync(BitStream:bs, data[PR_TrailerSync])
        {
            BS_WriteValue(
                bs,
                PR_UINT16, data[PR_trailerId],
                PR_FLOAT3, data[PR_position],
                PR_FLOAT4, data[PR_quaternion],
                PR_FLOAT3, data[PR_velocity],
                PR_FLOAT3, data[PR_angularVelocity]
            );
        }

        stock BS_WritePassengerSync(BitStream:bs, data[PR_PassengerSync])
        {
            BS_WriteValue(
                bs,
                PR_UINT16, data[PR_vehicleId],
                PR_BITS, data[PR_driveBy], 2,
                PR_BITS, data[PR_seatId], 6,
                PR_BITS, data[PR_additionalKey], 2,
                PR_BITS, data[PR_weaponId], 6,
                PR_UINT8, data[PR_playerHealth],
                PR_UINT8, data[PR_playerArmour],
                PR_UINT16, data[PR_lrKey],
                PR_UINT16, data[PR_udKey],
                PR_UINT16, data[PR_keys],
                PR_FLOAT3, data[PR_position]
            );
        }

        stock BS_WriteUnoccupiedSync(BitStream:bs, data[PR_UnoccupiedSync])
        {
            BS_WriteValue(
                bs,
                PR_UINT16, data[PR_vehicleId],
                PR_UINT8, data[PR_seatId],
                PR_FLOAT3, data[PR_roll],
                PR_FLOAT3, data[PR_direction],
                PR_FLOAT3, data[PR_position],
                PR_FLOAT3, data[PR_velocity],
                PR_FLOAT3, data[PR_angularVelocity],
                PR_FLOAT, data[PR_vehicleHealth]
            );
        }

        stock BS_WriteAimSync(BitStream:bs, data[PR_AimSync])
        {
            BS_WriteValue(
                bs,
                PR_UINT8, data[PR_camMode],
                PR_FLOAT3, data[PR_camFrontVec],
                PR_FLOAT3, data[PR_camPos],
                PR_FLOAT, data[PR_aimZ],
                PR_BITS, data[PR_weaponState], 2,
                PR_BITS, data[PR_camZoom], 6,
                PR_UINT8, data[PR_aspectRatio]
            );
        }

        stock BS_WriteBulletSync(BitStream:bs, data[PR_BulletSync])
        {
            BS_WriteValue(
                bs,
                PR_UINT8, data[PR_hitType],
                PR_UINT16, data[PR_hitId],
                PR_FLOAT3, data[PR_origin],
                PR_FLOAT3, data[PR_hitPos],
                PR_FLOAT3, data[PR_offsets],
                PR_UINT8, data[PR_weaponId]
            );
        }

        stock BS_WriteSpectatingSync(BitStream:bs, data[PR_SpectatingSync])
        {
            BS_WriteValue(
                bs,
                PR_UINT16, data[PR_lrKey],
                PR_UINT16, data[PR_udKey],
                PR_UINT16, data[PR_keys],
                PR_FLOAT3, data[PR_position]
            );
        }

        stock BS_WriteMarkersSync(BitStream:bs, data[PR_MarkersSync])
        {
            BS_WriteInt32(bs, data[PR_numberOfPlayers]);

            for (new i; i < MAX_PLAYERS; i++) {
                if (!data[PR_playerIsParticipant][i]) {
                    continue;
                }

                BS_WriteValue(
                    bs,
                    PR_UINT16, i,
                    PR_CBOOL, data[PR_playerIsActive][i]
                );

                if (data[PR_playerIsActive][i]) {
                    BS_WriteValue(
                        bs,
                        PR_INT16, data[PR_playerPositionX][i],
                        PR_INT16, data[PR_playerPositionY][i],
                        PR_INT16, data[PR_playerPositionZ][i]
                    );
                }
            }
        }

        stock BS_WriteWeaponsUpdate(BitStream:bs, data[PR_WeaponsUpdate])
        {
            BS_WriteValue(
                bs,
                PR_UINT16, data[PR_targetId],
                PR_UINT16, data[PR_targetActorId]
            );

            for (new slotId; slotId < PR_MAX_WEAPON_SLOTS; slotId++) {
                if (!data[PR_slotUpdated][slotId]) {
                    continue;
                }

                BS_WriteValue(
                    bs,
                    PR_UINT8, slotId,
                    PR_UINT8, data[PR_slotWeaponId][slotId],
                    PR_UINT16, data[PR_slotWeaponAmmo][slotId]
                );
            }
        }

        stock BS_WriteStatsUpdate(BitStream:bs, data[PR_StatsUpdate])
        {
            BS_WriteValue(
                bs,
                PR_INT32, data[PR_money],
                PR_INT32, data[PR_drunkLevel]
            );
        }

        stock BS_WriteRconCommand(BitStream:bs, data[PR_RconCommand])
        {
            BS_WriteValue(
                bs,
                PR_STRING32, data[PR_command]
            );
        }

        stock PR_SendPacketToPlayerStream(BitStream:bs, playerid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0)
        {
            #if defined foreach
            foreach (new i : Player) {
            #else
            for (new i = GetPlayerPoolSize(); i >= 0; i--) {
            #endif
                if (!IsPlayerStreamedIn(i, playerid)) {
                    continue;
                }

                PR_SendPacket(bs, i, priority, reliability, orderingchannel);
            }
        }

        stock PR_SendRPCToPlayerStream(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0)
        {
            #if defined foreach
            foreach (new i : Player) {
            #else
            for (new i = GetPlayerPoolSize(); i >= 0; i--) {
            #endif
                if (!IsPlayerStreamedIn(i, playerid)) {
                    continue;
                }

                PR_SendRPC(bs, i, rpcid, priority, reliability, orderingchannel);
            }
        }

        stock PR_SendPacketToVehicleStream(BitStream:bs, vehicleid, excludedplayerid = INVALID_PLAYER_ID, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0)
        {
            #if defined foreach
            foreach (new i : Player) {
            #else
            for (new i = GetPlayerPoolSize(); i >= 0; i--) {
            #endif
                if (i == excludedplayerid || !IsVehicleStreamedIn(vehicleid, i)) {
                    continue;
                }

                PR_SendPacket(bs, i, priority, reliability, orderingchannel);
            }
        }

        stock PR_SendRPCToVehicleStream(BitStream:bs, vehicleid, rpcid, excludedplayerid = INVALID_PLAYER_ID, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED, orderingchannel = 0)
        {
            #if defined foreach
            foreach (new i : Player) {
            #else
            for (new i = GetPlayerPoolSize(); i >= 0; i--) {
            #endif
                if (i == excludedplayerid || !IsVehicleStreamedIn(vehicleid, i)) {
                    continue;
                }

                PR_SendRPC(bs, i, rpcid, priority, reliability, orderingchannel);
            }
        }

        forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        forward OnOutgoingPacket(playerid, packetid, BitStream:bs);
        forward OnOutgoingRPC(playerid, rpcid, BitStream:bs);
        forward OnIncomingRawPacket(playerid, packetid, BitStream:bs);
        forward OnIncomingInternalPacket(playerid, packetid, BitStream:bs);
        forward OnOutgoingInternalPacket(playerid, packetid, BitStream:bs);

        #pragma deprecated Use OnOutgoingPacket instead
        forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        #pragma deprecated Use OnOutgoingRPC instead
        forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);

        #if defined FILTERSCRIPT
            public OnFilterScriptInit()
            {
                PR_Init();

                #if defined PawnRakNet_OnFilterScriptInit
                    PawnRakNet_OnFilterScriptInit();
                #endif

                return 1;
            }

            #if defined _ALS_OnFilterScriptInit
                #undef OnFilterScriptInit
            #else
                #define _ALS_OnFilterScriptInit
            #endif

            #define OnFilterScriptInit PawnRakNet_OnFilterScriptInit

            #if defined PawnRakNet_OnFilterScriptInit
                forward PawnRakNet_OnFilterScriptInit();
            #endif
        #else
            public OnGameModeInit()
            {
                PR_Init();

                #if defined PawnRakNet_OnGameModeInit
                    PawnRakNet_OnGameModeInit();
                #endif

                return 1;
            }

            #if defined _ALS_OnGameModeInit
                #undef OnGameModeInit
            #else
                #define _ALS_OnGameModeInit
            #endif

            #define OnGameModeInit PawnRakNet_OnGameModeInit

            #if defined PawnRakNet_OnGameModeInit
                forward PawnRakNet_OnGameModeInit();
            #endif
        #endif
    #endif // !defined __cplusplus
#endif // PAWNRAKNET_INC_
