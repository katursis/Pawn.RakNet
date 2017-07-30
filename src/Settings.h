#ifndef SETTINGS_H_
#define SETTINGS_H_

class Settings {
public:
    static constexpr char
        *kPluginName = "Pawn.RakNet",
        *kPluginVersion = "1.0",
        *kPublicVarNameVersion = "_pawnraknet_version",
        *kPublicVarNameIsGamemode = "_pawnraknet_is_gamemode",
#ifdef _WIN32
        *kPattern = "\x6A\xFF\x68\x5B\xA4\x4A\x00\x64\xA1\x00\x00" \
        "\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51" \
        "\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83" \
        "\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x24\xFF" \
        "\x00\x00\x00\x00\x74\x16",
        *kMask = "xxx????xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx";
#else
        *kPattern =
        "\x55\x89\xE5\x83\xEC\x18\xC7\x04\x24\xFF\xFF" \
        "\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF" \
        "\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF" \
        "\xFF\x89\xF0\x8B\x5D\xFF\x8B\x75\xFF\x89\xEC" \
        "\x5D\xC3",
        *kMask = "xxxxxxxxx????xx?xx?x????xxxxxx????xxxx?xx?xxxx";
#endif
};

#endif  // SETTINGS_H_
