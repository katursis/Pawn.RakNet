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

#ifndef SCRIPTS_H_
#define SCRIPTS_H_

namespace Scripts {
    class Public {
    public:
        explicit Public(const std::string &name, AMX *amx) : _name{name}, _amx{amx} {
            _exists = (amx_FindPublic(_amx, _name.c_str(), &_index) == AMX_ERR_NONE && _index >= 0);
        }

        template<typename ... ARGS>
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
        explicit Script(AMX *amx) : _amx{amx} {}

        template<PR_EventType eventType>
        inline bool OnEvent(int player_id, int id, RakNet::BitStream *bs) {
            const auto &pub = _publics[eventType];

            if (pub->exists()) {
                bs->ResetReadPointer();

                if (!pub->call(player_id, id, bs)) {
                    return false;
                }
            }

            const auto &handler = _handlers[eventType][id];

            if (handler) {
                bs->ResetReadPointer();

                if (!handler->call(player_id, bs)) {
                    return false;
                }
            }

            return true;
        }

        void InitEvent(PR_EventType type, const std::string &public_name) {
            _publics.at(type) = std::unique_ptr<Public>{new Public{public_name, _amx}};
        }

        void RegisterHandler(int id, const std::string &public_name, PR_EventType type) {
            _handlers.at(type).at(id) = std::unique_ptr<Public>{new Public{public_name, _amx}};

            if (!_handlers[type][id]->exists()) {
                _handlers[type][id] = nullptr;

                throw std::runtime_error{"public " + public_name + " not exists"};
            }
        }

        void Init() {
            InitEvent(PR_INCOMING_PACKET, "OnIncomingPacket");
            InitEvent(PR_INCOMING_RPC, "OnIncomingRPC");
            InitEvent(PR_OUTCOMING_PACKET, "OnOutcomingPacket");
            InitEvent(PR_OUTCOMING_RPC, "OnOutcomingRPC");

            int num_publics{};

            amx_NumPublics(_amx, &num_publics);

            std::regex r{Settings::kRegHandlerPublicRegExp};

            for (int i{}; i < num_publics; ++i) {
                if (std::regex_match(Functions::GetAmxPublicName(_amx, i), r)) {
                    amx_Exec(_amx, nullptr, i);
                }
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

    void Load(AMX *amx, bool is_gamemode) {
        if (is_gamemode) {
            scripts.emplace_back(amx);
        } else {
            scripts.emplace_front(amx);
        }
    }

    void Unload(AMX *amx) {
        const auto script = std::find(scripts.begin(), scripts.end(), amx);

        if (script != scripts.end()) {
            scripts.erase(script);
        }
    }

    Script & GetScript(AMX *amx) {
        const auto script = std::find(scripts.begin(), scripts.end(), amx);

        if (script == scripts.end()) {
            throw std::runtime_error{"script not found"};
        }

        return *script;
    }

    template<PR_EventType eventType>
    bool OnEvent(int player_id, int id, RakNet::BitStream *bs) {
        return std::all_of(scripts.begin(), scripts.end(), [=](Script &script) {
            return script.OnEvent<eventType>(player_id, id, bs);
        });
    }
}

#endif // SCRIPTS_H_
