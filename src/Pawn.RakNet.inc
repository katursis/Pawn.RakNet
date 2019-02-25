/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2019 urShadow
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

    #define PAWNRAKNET_INCLUDE_VERSION 130

    #define PR_MAX_HANDLERS 255

    enum PR_EventType
    {
        PR_INCOMING_PACKET,
        PR_INCOMING_RPC,
        PR_OUTCOMING_PACKET,
        PR_OUTCOMING_RPC,
        PR_NUMBER_OF_EVENT_TYPES
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
        PR_CSTRING,

        PR_BITS,
        PR_FLOAT3,
        PR_FLOAT4,
        PR_VECTOR,
        PR_NORM_QUAT
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
            PR_animationFlags
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
            Float:PR_trainSpeed
        };

        enum PR_TrailerSync
        {
            PR_trailerId,
            Float:PR_position[3],
            Float:PR_quaternion[4],
            Float:PR_velocity[3],
            Float:PR_angularVelocity[3]
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
            Float:PR_position[3]
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
            Float:PR_vehicleHealth
        };

        enum PR_AimSync
        {
            PR_camMode,
            Float:PR_camFrontVec[3],
            Float:PR_camPos[3],
            Float:PR_aimZ,
            PR_camZoom,
            PR_weaponState,
            PR_aspectRatio
        };

        enum PR_BulletSync
        {
            PR_hitType,
            PR_hitId,
            Float:PR_origin[3],
            Float:PR_hitPos[3],
            Float:PR_offsets[3],
            PR_weaponId
        };

        enum PR_SpectatingSync
        {
            PR_lrKey,
            PR_udKey,
            PR_keys,
            Float:PR_position[3]
        };

        enum PR_WeaponsUpdate
        {
            PR_targetId,
            PR_targetActorId,
            PR_slotWeaponId[PR_MAX_WEAPON_SLOTS],
            PR_slotWeaponAmmo[PR_MAX_WEAPON_SLOTS],

            PR_slotUpdated[PR_MAX_WEAPON_SLOTS] // helping field
        };

        native BS_Send(BitStream:bs, playerid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED);
        native BS_RPC(BitStream:bs, playerid, rpcid, PR_PacketPriority:priority = PR_HIGH_PRIORITY, PR_PacketReliability:reliability = PR_RELIABLE_ORDERED);

        native BS_EmulateIncomingPacket(BitStream:bs, playerid);
        native BS_EmulateIncomingRPC(BitStream:bs, playerid, rpcid);

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
        native BS_GetNumberOfBitsAllocated(BitStream:bs, &number);

        native BS_WriteValue(BitStream:bs, {PR_ValueType, Float, _}:...);
        native BS_ReadValue(BitStream:bs, {PR_ValueType, Float, _}:...);

        native PR_RegHandler(id, const publicname[], PR_EventType:type);
        native PR_Init(bool:is_gamemode);

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

        #define PR_EXPAND_ARR3(%0) %0[0],%0[1],%0[2]
        #define PR_EXPAND_ARR4(%0) %0[0],%0[1],%0[2],%0[3]

        stock BS_PackHealthArmour(health, armour, &healthArmour) {
            if (health > 0 && health < 100) {
                healthArmour = (health / 7) << 4;
            } else if (health >= 100) {
                healthArmour = 0xF << 4;
            }

            if (armour > 0 && armour < 100) {
                healthArmour |= (armour / 7);
            } else if (armour >= 100) {
                healthArmour |= 0xF;
            }
        }

        stock BS_UnpackHealthArmour(healthArmour, &health, &armour) {
            new armourTemp, healthTemp;

            armourTemp = (healthArmour & 0x0F);
            healthTemp = (healthArmour >> 4);

            if (armourTemp == 0xF) {
                armour = 100;
            } else if (armourTemp == 0) {
                armour = 0;
            } else {
                armour = armourTemp * 7;
            }

            if (healthTemp == 0xF) {
                health = 100;
            } else if (healthTemp == 0) {
                health = 0;
            } else {
                health = healthTemp * 7;
            }
        }

        stock BS_ReadOnFootSync(BitStream:bs, data[PR_OnFootSync], outcoming = false)
        {
            if (outcoming) {
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

        stock BS_ReadInCarSync(BitStream:bs, data[PR_InCarSync], outcoming = false)
        {
            if (outcoming) {
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

        stock BS_ReadWeaponsUpdate(BitStream:bs, data[PR_WeaponsUpdate])
        {
            new numberOfBytes, numberOfSlots;

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
                new slotId, weaponId, ammo;

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

        stock BS_WriteOnFootSync(BitStream:bs, data[PR_OnFootSync], outcoming = false)
        {
            if (outcoming) {
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

        stock BS_WriteInCarSync(BitStream:bs, data[PR_InCarSync], outcoming = false)
        {
            if (outcoming) {
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

        stock BS_WriteWeaponsUpdate(BitStream:bs, data[PR_WeaponsUpdate])
        {
            BS_WriteValue(
                bs,
                PR_UINT16, data[PR_targetId],
                PR_UINT16, data[PR_targetActorId]
            );

            for (new slotId; slotId < PR_MAX_WEAPON_SLOTS; ++slotId) {
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

        forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);

        #if defined FILTERSCRIPT
            public OnFilterScriptInit()
            {
                PR_Init(false);

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
                PR_Init(true);

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
