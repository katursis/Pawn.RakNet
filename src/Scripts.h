#ifndef SCRIPTS_H_
#define SCRIPTS_H_

namespace Scripts {
    class Public {
    public:
        explicit Public(const std::string &name, AMX *amx) : _name{ name }, _amx{ amx } {
            _exists = (amx_FindPublic(_amx, _name.c_str(), &_index) == AMX_ERR_NONE && _index >= 0);
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
        const std::string _name;
        int _index;
        bool _exists;
    };

    class Script {
    public:
        enum Publics {
            ON_INCOMING_RPC,
            ON_INCOMING_PACKET,
            ON_OUTCOMIMG_RPC,
            ON_OUTCOMING_PACKET,
            NUMBER_OF_PUBLICS
        };

        explicit Script(AMX *amx) : _amx(amx) {
            const auto make_public = [amx](const std::string &name) {
                return std::unique_ptr<Public>(new Public{ name, amx });
            };

            _publics[ON_INCOMING_RPC] = make_public("OnIncomingRPC");
            _publics[ON_INCOMING_PACKET] = make_public("OnIncomingPacket");
            _publics[ON_OUTCOMIMG_RPC] = make_public("OnOutcomingRPC");
            _publics[ON_OUTCOMING_PACKET] = make_public("OnOutcomingPacket");
        }

        // forward OnIncomingRPC(playerid, rpcid, BitStream:bs);
        inline bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            cell retval{};

            const auto &pub = _publics[ON_INCOMING_RPC];

            if (pub->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(rpc_id));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, pub->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            // IRPC:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_INCOMING_RPC][rpc_id];

            if (handler && handler->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, handler->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            return true;
        }

        // forward OnIncomingPacket(playerid, packetid, BitStream:bs);
        inline bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            cell retval{};

            const auto &pub = _publics[ON_INCOMING_PACKET];

            if (pub->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(packet_id));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, pub->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            // IPacket:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_INCOMING_PACKET][packet_id];

            if (handler && handler->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, handler->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            return true;
        }

        // forward OnOutcomingRPC(playerid, rpcid, BitStream:bs);
        inline bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
            cell retval{};

            const auto &pub = _publics[ON_OUTCOMIMG_RPC];

            if (pub->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(rpc_id));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, pub->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            // ORPC:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_OUTCOMING_RPC][rpc_id];

            if (handler && handler->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, handler->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            return true;
        }

        // forward OnOutcomingPacket(playerid, packetid, BitStream:bs);
        inline bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
            cell retval{};

            const auto &pub = _publics[ON_OUTCOMING_PACKET];

            if (pub->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(packet_id));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, pub->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            // OPacket:<ID>(playerid, BitStream:bs)
            const auto &handler = _handlers[PR_OUTCOMING_PACKET][packet_id];

            if (handler && handler->exists()) {
                if (bs) {
                    bs->ResetReadPointer();
                }

                amx_Push(_amx, reinterpret_cast<cell>(bs));
                amx_Push(_amx, static_cast<cell>(player_id));

                amx_Exec(_amx, &retval, handler->get_index());

                if (retval == 0) {
                    return false;
                }
            }

            return true;
        }

        void InitHandlersRegistration() {
            int num_publics{};

            amx_NumPublics(_amx, &num_publics);

            if (!num_publics) {
                return;
            }

            std::regex r{ Settings::kRegHandlerPublicRegExp };

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

        void RegisterHandler(int id, const std::string &public_name, PR_HandlerType type) {
            _handlers.at(type).at(id) = std::unique_ptr<Public>(new Public{ public_name, _amx });

            if (!_handlers[type][id]->exists()) {
                throw std::runtime_error{ "Public " + public_name + " not exists" };
            }
        }

        inline AMX *get_amx() const {
            return _amx;
        }

    private:
        AMX *_amx;
        std::array<std::unique_ptr<Public>, NUMBER_OF_PUBLICS> _publics;
        std::array<std::array<std::unique_ptr<Public>, MAX_RPC_MAP_SIZE>, PR_NUMBER_OF_HANDLER_TYPES> _handlers;
    };

    std::list<std::unique_ptr<Script>> scripts;

    void Load(AMX *amx, bool is_gamemode) {
        auto script = std::unique_ptr<Script>(new Script{ amx });

        if (is_gamemode) {
            scripts.push_back(std::move(script));

            scripts.back()->InitHandlersRegistration();
        } else {
            scripts.push_front(std::move(script));

            scripts.front()->InitHandlersRegistration();
        }
    }

    void Unload(AMX *amx) {
        const auto script = std::find_if(scripts.begin(), scripts.end(), [amx](const std::unique_ptr<Script> &script) {
            return script->get_amx() == amx;
        });

        if (script != scripts.end()) {
            scripts.erase(script);
        }
    }

    void RegisterHandler(AMX *amx, int id, const std::string &public_name, PR_HandlerType type) {
        const auto script = std::find_if(scripts.begin(), scripts.end(), [amx](const std::unique_ptr<Script> &script) {
            return script->get_amx() == amx;
        });

        if (script != scripts.end()) {
            (*script)->RegisterHandler(id, public_name, type);
        }
    }

    bool OnIncomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnIncomingPacket(player_id, packet_id, bs);
        });
    }

    bool OnIncomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnIncomingRPC(player_id, rpc_id, bs);
        });
    }

    bool OnOutcomingPacket(int player_id, int packet_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnOutcomingPacket(player_id, packet_id, bs);
        });
    }

    bool OnOutcomingRPC(int player_id, int rpc_id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](const std::unique_ptr<Script> &script) {
            return script->OnOutcomingRPC(player_id, rpc_id, bs);
        });
    }
}

#endif // SCRIPTS_H_
