#ifndef SCRIPTS_H_
#define SCRIPTS_H_

class Public {
public:
    explicit Public(const std::string &name, AMX *amx) : _name{ name }, _amx{ amx } {
        _exists = (amx_FindPublic(amx, name.c_str(), &_index) == AMX_ERR_NONE);
    }

    inline bool exists() const {
        return _exists;
    }

    inline int get_index() const {
        return _index;
    }

private:
    std::string _name;
    int _index;
    bool _exists;
    AMX *_amx;
};

class Script {
public:
    enum Publics {
        ON_INCOMING_PACKET,
        ON_INCOMING_RPC,
        ON_OUTCOMING_PACKET,
        ON_OUTCOMIMG_RPC,
        NUMBER_OF_PUBLICS
    };

    explicit Script(AMX *amx) : _amx(amx) {
        const auto make_public = [amx](const std::string &name) {
            return std::make_unique<Public>(name, amx);
        };

        _publics[ON_INCOMING_PACKET] = make_public("OnIncomingPacket");
        _publics[ON_INCOMING_RPC] = make_public("OnIncomingRPC");
        _publics[ON_OUTCOMING_PACKET] = make_public("OnOutcomingPacket");
        _publics[ON_OUTCOMIMG_RPC] = make_public("OnOutcomingRPC");
    }

    // forward OnIncomingPacket(playerid, packetid, BitStream:bs);
    inline bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        const auto &pub = _publics[ON_INCOMING_PACKET];

        if (!pub->exists()) {
            return true;
        }

        if (bs) {
            bs->ResetReadPointer();
        }

        amx_Push(_amx, reinterpret_cast<cell>(bs));
        amx_Push(_amx, static_cast<cell>(packet_id));
        amx_Push(_amx, static_cast<cell>(player_id));

        cell retval{};

        amx_Exec(_amx, &retval, pub->get_index());

        return retval == 1;
    }

    // forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
    inline bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        const auto &pub = _publics[ON_INCOMING_RPC];

        if (!pub->exists()) {
            return true;
        }

        if (bs) {
            bs->ResetReadPointer();
        }

        amx_Push(_amx, reinterpret_cast<cell>(bs));
        amx_Push(_amx, static_cast<cell>(rpc_id));
        amx_Push(_amx, static_cast<cell>(player_id));

        cell retval{};

        amx_Exec(_amx, &retval, pub->get_index());

        return retval == 1;
    }

    // forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
    inline bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        const auto &pub = _publics[ON_OUTCOMING_PACKET];

        if (!pub->exists()) {
            return true;
        }

        if (bs) {
            bs->ResetReadPointer();
        }

        amx_Push(_amx, reinterpret_cast<cell>(bs));
        amx_Push(_amx, static_cast<cell>(packet_id));
        amx_Push(_amx, static_cast<cell>(player_id));

        cell retval{};

        amx_Exec(_amx, &retval, pub->get_index());

        return retval == 1;
    }

    // forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
    inline bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        const auto &pub = _publics[ON_OUTCOMIMG_RPC];

        if (!pub->exists()) {
            return true;
        }

        if (bs) {
            bs->ResetReadPointer();
        }

        amx_Push(_amx, reinterpret_cast<cell>(bs));
        amx_Push(_amx, static_cast<cell>(rpc_id));
        amx_Push(_amx, static_cast<cell>(player_id));

        cell retval{};

        amx_Exec(_amx, &retval, pub->get_index());

        return retval == 1;
    }

    inline AMX *get_amx() const {
        return _amx;
    }

private:
    AMX *_amx;
    std::array<std::unique_ptr<Public>, NUMBER_OF_PUBLICS> _publics{};
};

class Scripts : public Singleton<Scripts> {
public:
    void Load(AMX *amx, bool is_gamemode) {
        auto script = std::make_unique<Script>(amx);

        if (is_gamemode) {
            _scripts.push_back(std::move(script));
        } else {
            _scripts.push_front(std::move(script));
        }
    }

    void Unload(AMX *amx) {
        const auto script = std::find_if(_scripts.begin(), _scripts.end(), [amx](const std::unique_ptr<Script> &script) {
            return script->get_amx() == amx;
        });

        if (script != _scripts.end()) {
            _scripts.erase(script);
        }
    }

    bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnIncomingPacket(player_id, packet_id, bs);
        });
    }

    bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnIncomingRPC(player_id, rpc_id, bs);
        });
    }

    bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnOutcomingPacket(player_id, packet_id, bs);
        });
    }

    bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return std::all_of(_scripts.begin(), _scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnOutcomingRPC(player_id, rpc_id, bs);
        });
    }

private:
    friend class Singleton<Scripts>;

    Scripts() = default;

    std::list<std::unique_ptr<Script>> _scripts;
};

#endif  // SCRIPTS_H_
