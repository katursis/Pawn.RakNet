#ifndef SETTINGS_H_
#define SETTINGS_H_

namespace Settings {
    constexpr char
        *kPluginName = "Pawn.RakNet",
        *kPluginVersion = "1.2.0",
        *kConfigFile = "plugins/pawnraknet.cfg",
        *kPublicVarNameVersion = "_pawnraknet_version",
        *kPublicVarNameIsGamemode = "_pawnraknet_is_gamemode",
        *kRegHandlerPublicRegExp = R"(^pr_r(?:ir|ip|or|op)_\w+$)",
#ifdef _WIN32
        *kGetRakServerInterfacePattern =
        "\x6A\xFF\x68\x5B\xA4\x4A\x00\x64\xA1\x00\x00" \
        "\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51" \
        "\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83" \
        "\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x24\xFF" \
        "\x00\x00\x00\x00\x74\x16",
        *kGetRakServerInterfaceMask = "???????xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx";
#else
        *kGetRakServerInterfacePattern =
        "\x55\x89\xE5\x83\xEC\x18\xC7\x04\x24\xFF\xFF" \
        "\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF" \
        "\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF" \
        "\xFF\x89\xF0\x8B\x5D\xFF\x8B\x75\xFF\x89\xEC" \
        "\x5D\xC3",
        *kGetRakServerInterfaceMask = "?????xxxx????xx?xx?x????xxxxxx????xxxx?xx?xxxx",

        *kGetPacketIdPattern =
        "\x55\xB8\xFF\x00\x00\x00\x89\xE5\x8B\x55\x08\x85\xD2\x74\x0D\x8B" \
        "\x52\x10\x0F\xB6\x02\x3C\xFF\x74\x07\x0F\xB6\xC0\x5D\xC3\x66\x90" \
        "\x0F\xB6\x42\x05\x5D\xC3",
        *kGetPacketIdMask = "xxxxxxxxxxxxxxxxxxxxxx?xxxxxxxxxxxxxxx";
#endif
    bool
        intercept_incoming_rpc{},
        intercept_incoming_packet{},
        intercept_outcoming_rpc{},
        intercept_outcoming_packet{},

        use_caching{};

    void Read() {
        std::fstream{kConfigFile, std::fstream::out | std::fstream::app};

        const auto config = cpptoml::parse_file(kConfigFile);

        intercept_incoming_rpc = config->get_as<bool>("InterceptIncomingRPC").value_or(true);
        intercept_incoming_packet = config->get_as<bool>("InterceptIncomingPacket").value_or(true);
        intercept_outcoming_rpc = config->get_as<bool>("InterceptOutcomingRPC").value_or(true);
        intercept_outcoming_packet = config->get_as<bool>("InterceptOutcomingPacket").value_or(true);

        use_caching = config->get_as<bool>("UseCaching").value_or(false);
    }

    void Save() {
        auto config = cpptoml::make_table();

        config->insert("InterceptIncomingRPC", intercept_incoming_rpc);
        config->insert("InterceptIncomingPacket", intercept_incoming_packet);
        config->insert("InterceptOutcomingRPC", intercept_outcoming_rpc);
        config->insert("InterceptOutcomingPacket", intercept_outcoming_packet);

        config->insert("UseCaching", use_caching);

        std::fstream{kConfigFile, std::fstream::out | std::fstream::trunc} << (*config);
    }
}

#endif // SETTINGS_H_
