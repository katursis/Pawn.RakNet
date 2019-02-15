#ifndef SCRIPTS_H_
#define SCRIPTS_H_

namespace Scripts {
    class Public {
    public:
        explicit Public(const std::string &name, AMX *amx) : _name{name}, _amx{amx} {
            _exists = (amx_FindPublic(_amx, _name.c_str(), &_index) == AMX_ERR_NONE && _index >= 0);
        }

        explicit Public(int index, AMX *amx) : _index{index}, _amx{amx} {
            char pname[sNAMEMAX + 1]{};

            _exists = (amx_GetPublic(_amx, _index, pname) == AMX_ERR_NONE);

            if (_exists) {
                _name = pname;
            }
        }

        template<typename... ARGS>
        inline cell call(ARGS ... args) {
            return Functions::AmxCallPublic(_amx, get_index(), args...);
        }

        inline bool exists() const {
            return _exists;
        }

        inline int get_index() {
            if (!Settings::use_caching) {
                amx_FindPublic(_amx, _name.c_str(), &_index);
            }

            return _index;
        }

    private:
        AMX *_amx;
        std::string _name;
        int _index;
        bool _exists;
    };

    class Script {
    public:
        explicit Script(AMX *amx) : _amx{amx} {
            InitPublic(PR_INCOMING_RPC, "OnIncomingRPC");
            InitPublic(PR_INCOMING_PACKET, "OnIncomingPacket");
            InitPublic(PR_OUTCOMING_RPC, "OnOutcomingRPC");
            InitPublic(PR_OUTCOMING_PACKET, "OnOutcomingPacket");
        }

        // forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        inline bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[PR_INCOMING_RPC];

            if (pub->exists()) {
                bs->ResetReadPointer();

                if (!pub->call(player_id, rpc_id, bs)) {
                    return false;
                }
            }

            // IRPC:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_INCOMING_RPC][rpc_id];

            if (handler && handler->exists()) {
                bs->ResetReadPointer();

                if (!handler->call(player_id, bs)) {
                    return false;
                }
            }

            return true;
        }

        // forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        inline bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[PR_INCOMING_PACKET];

            if (pub->exists()) {
                bs->ResetReadPointer();

                if (!pub->call(player_id, packet_id, bs)) {
                    return false;
                }
            }

            // IPacket:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_INCOMING_PACKET][packet_id];

            if (handler && handler->exists()) {
                bs->ResetReadPointer();

                if (!handler->call(player_id, bs)) {
                    return false;
                }
            }

            return true;
        }

        // forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
        inline bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[PR_OUTCOMING_RPC];

            if (pub->exists()) {
                bs->ResetReadPointer();

                if (!pub->call(player_id, rpc_id, bs)) {
                    return false;
                }
            }

            // ORPC:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_OUTCOMING_RPC][rpc_id];

            if (handler && handler->exists()) {
                bs->ResetReadPointer();

                if (!handler->call(player_id, bs)) {
                    return false;
                }
            }

            return true;
        }

        // forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        inline bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            const auto &pub = _publics[PR_OUTCOMING_PACKET];

            if (pub->exists()) {
                bs->ResetReadPointer();

                if (!pub->call(player_id, packet_id, bs)) {
                    return false;
                }
            }

            // OPacket:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_OUTCOMING_PACKET][packet_id];

            if (handler && handler->exists()) {
                bs->ResetReadPointer();

                if (!handler->call(player_id, bs)) {
                    return false;
                }
            }

            return true;
        }

        void InitPublic(PR_EventType type, const std::string &name) {
            _publics.at(type) = std::unique_ptr<Public>{new Public{name, _amx}};
        }

        void InitHandlersRegistration() {
            int num_publics{};

            amx_NumPublics(_amx, &num_publics);

            if (!num_publics) {
                return;
            }

            std::regex r{Settings::kRegHandlerPublicRegExp};

            for (int i{}; i < num_publics; i++) {
                char public_name[32]{};

                amx_GetPublic(_amx, i, public_name);

                if (std::regex_match(public_name, r)) {
                    if (_amx->flags & AMX_FLAG_NTVREG) { // all native functions are registered
                        amx_Exec(_amx, nullptr, i);
                    } else {
                        _amx->flags |= AMX_FLAG_NTVREG;

                        amx_Exec(_amx, nullptr, i);

                        _amx->flags &= ~AMX_FLAG_NTVREG;
                    }
                }
            }
        }

        void RegisterHandler(int id, const std::string &public_name, PR_EventType type) {
            _handlers.at(type).at(id) = std::unique_ptr<Public>{new Public{public_name, _amx}};

            if (!_handlers[type][id]->exists()) {
                throw std::runtime_error{"public " + public_name + " not exists"};
            }
        }

        cell NewBitStream() {
            const auto bs = std::make_shared<RakNet::BitStream>();

            _bitstreams.insert(bs);

            return reinterpret_cast<cell>(bs.get());
        }

        void DeleteBitStream(cell handle) {
            const auto bs = std::find_if(_bitstreams.begin(), _bitstreams.end(), [handle](const std::shared_ptr<RakNet::BitStream> &bs) {
                return reinterpret_cast<cell>(bs.get()) == handle;
            });

            if (bs == _bitstreams.end()) {
                throw std::runtime_error{"invalid BitStream handle"};
            }

            _bitstreams.erase(bs);
        }

        inline bool operator==(AMX *amx) {
            return _amx == amx;
        }
 
    private:
        AMX *_amx;
        std::array<std::unique_ptr<Public>, PR_NUMBER_OF_EVENT_TYPES> _publics;
        std::array<std::array<std::unique_ptr<Public>, PR_MAX_HANDLERS>, PR_NUMBER_OF_EVENT_TYPES> _handlers;
        std::unordered_set<std::shared_ptr<RakNet::BitStream>> _bitstreams;
    };

    std::list<Script> scripts;

    Script & GetScript(AMX *amx) {
        const auto script = std::find(scripts.begin(), scripts.end(), amx);

        if (script == scripts.end()) {
            throw std::runtime_error{"amx not found"};
        }

        return *script;
    }

    void Load(AMX *amx, bool is_gamemode) {
        if (is_gamemode) {
            scripts.emplace_back(amx);
        } else {
            scripts.emplace_front(amx);
        }

        GetScript(amx).InitHandlersRegistration();
    }

    void Unload(AMX *amx) {
        const auto script = std::find(scripts.begin(), scripts.end(), amx);

        if (script != scripts.end()) {
            scripts.erase(script);
        }
    }

    bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](Script &script) {
            return script.OnIncomingPacket(player_id, packet_id, bs);
        });
    }

    bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](Script &script) {
            return script.OnIncomingRPC(player_id, rpc_id, bs);
        });
    }

    bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](Script &script) {
            return script.OnOutcomingPacket(player_id, packet_id, bs);
        });
    }

    bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](Script &script) {
            return script.OnOutcomingRPC(player_id, rpc_id, bs);
        });
    }
}

#endif // SCRIPTS_H_
