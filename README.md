# Pawn.RakNet
[![GitHub Release](https://img.shields.io/github/release/katursis/Pawn.RakNet.svg)](https://github.com/katursis/Pawn.RakNet/releases/latest)

Plugin for **SA:MP 0.3.7** server that allows you to capture and analyze RakNet traffic

## Main features
* Capture, modify, filter incoming/outgoing packets and RPCs
* Send your own packets and RPCs to a player
* Emulate incoming packets and RPCs from a player

## Documentation

[Pawn.RakNet wiki](https://github.com/katursis/Pawn.RakNet/wiki)

## Simple example
```pawn
const PLAYER_SYNC = 207;

IPacket:PLAYER_SYNC(playerid, BitStream:bs)
{
  new onFootData[PR_OnFootSync];

  BS_IgnoreBits(bs, 8);
  BS_ReadOnFootSync(bs, onFootData);

  printf(
    "PLAYER_SYNC[%d]:\nlrKey %d \nudKey %d \nkeys %d \nposition %.2f %.2f %.2f \nquaternion %.2f %.2f %.2f %.2f \nhealth %d \narmour %d \nadditionalKey %d \nweaponId %d \nspecialAction %d \nvelocity %.2f %.2f %.2f \nsurfingOffsets %.2f %.2f %.2f \nsurfingVehicleId %d \nanimationId %d \nanimationFlags %d",
    playerid,
    onFootData[PR_lrKey],
    onFootData[PR_udKey],
    onFootData[PR_keys],
    onFootData[PR_position][0],
    onFootData[PR_position][1],
    onFootData[PR_position][2],
    onFootData[PR_quaternion][0],
    onFootData[PR_quaternion][1],
    onFootData[PR_quaternion][2],
    onFootData[PR_quaternion][3],
    onFootData[PR_health],
    onFootData[PR_armour],
    onFootData[PR_additionalKey],
    onFootData[PR_weaponId],
    onFootData[PR_specialAction],
    onFootData[PR_velocity][0],
    onFootData[PR_velocity][1],
    onFootData[PR_velocity][2],
    onFootData[PR_surfingOffsets][0],
    onFootData[PR_surfingOffsets][1],
    onFootData[PR_surfingOffsets][2],
    onFootData[PR_surfingVehicleId],
    onFootData[PR_animationId],
    onFootData[PR_animationFlags]
  );

  return 1;
}
```
