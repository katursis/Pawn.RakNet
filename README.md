# Pawn.RakNet
[![GitHub Release](https://img.shields.io/github/release/urShadow/Pawn.RakNet.svg)](https://github.com/urShadow/Pawn.RakNet/releases/latest)

Plugin for **SA:MP** server that allows you to analyze RakNet traffic in Pawn

## Main features
* Capture, modify, and stop processing incoming/outcoming packets and RPCs
* Send packets and RPCs to player
* Emulate packets and RPCs from player

## Simple example
```pawn
const AIM_SYNC = 203; // 0.3.7

IPacket:AIM_SYNC(playerid, BitStream:bs) // incoming packet
{
    new aimData[PR_AimSync];

    BS_IgnoreBits(bs, 8); // packet id (byte)
    BS_ReadAimSync(bs, aimData);

    if (aimData[PR_aimZ] != aimData[PR_aimZ]) // is NaN
    {
        aimData[PR_aimZ] = 0.0;

        BS_SetWriteOffset(bs, 8);
        BS_WriteAimSync(bs, aimData); // modify data
    }

    return 1; // continue processing
}
```
## Documentation

[Wiki](https://github.com/urShadow/Pawn.RakNet/wiki)