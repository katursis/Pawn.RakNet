# Pawn.RakNet
[![GitHub Release](https://img.shields.io/github/release/urShadow/Pawn.RakNet.svg)](https://github.com/urShadow/Pawn.RakNet/releases/latest)

Plugin for **SA:MP** server that allows you to work with RakNet in Pawn

## Main features
* Intercept and modify incoming/outcoming packets
* Send packets to player
* Emulate packets from player

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
        BS_WriteAimSync(bs, aimData); // rewrite data
    }

    return 1; // continue processing
}
```
## Documentation

[Wiki](https://github.com/urShadow/Pawn.RakNet/wiki)