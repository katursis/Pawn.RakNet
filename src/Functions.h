#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

namespace Functions {
    namespace RakServer {
        bool Send(RakNet::BitStream *bitStream, int priority, int reliability, char orderingChannel, PlayerID playerId, bool broadcast) {
            return urmem::call_function<urmem::calling_convention::thiscall, bool>(
                Addresses::FUNC_RAKSERVER__SEND,
                Addresses::PTR_RAKSERVER,
                bitStream,
                priority,
                reliability,
                orderingChannel,
                playerId,
                broadcast
            );
        }

        bool RPC(RPCIndex *uniqueID, RakNet::BitStream *bitStream, int priority, int reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp) {
            return urmem::call_function<urmem::calling_convention::thiscall, bool>(
                Addresses::FUNC_RAKSERVER__RPC,
                Addresses::PTR_RAKSERVER,
                uniqueID,
                bitStream,
                priority,
                reliability,
                orderingChannel,
                playerId,
                broadcast,
                shiftTimestamp
            );
        }

        Packet * Receive() {
            return urmem::call_function<urmem::calling_convention::thiscall, Packet *>(
                Addresses::FUNC_RAKSERVER__RECEIVE,
                Addresses::PTR_RAKSERVER
            );
        }

        void DeallocatePacket(Packet *packet) {
            urmem::call_function<urmem::calling_convention::thiscall>(
                Addresses::FUNC_RAKSERVER__DEALLOCATE_PACKET,
                Addresses::PTR_RAKSERVER,
                packet
            );
        }

        void * RegisterAsRemoteProcedureCall(RPCIndex *uniqueID, RPCFunction functionPointer) {
            return urmem::call_function<urmem::calling_convention::thiscall, void *>(
                Addresses::FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL,
                Addresses::PTR_RAKSERVER,
                uniqueID,
                functionPointer
            );
        }

        int GetIndexFromPlayerID(const PlayerID &playerId) {
            return urmem::call_function<urmem::calling_convention::thiscall, int>(
                Addresses::FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID,
                Addresses::PTR_RAKSERVER,
                playerId
            );
        }

        const PlayerID &GetPlayerIDFromIndex(int index) {
            return urmem::call_function<urmem::calling_convention::thiscall, PlayerID>(
                Addresses::FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX,
                Addresses::PTR_RAKSERVER,
                index
            );
        }

        int GetPacketId(Packet *packet) {
            return urmem::call_function<urmem::calling_convention::cdeclcall, unsigned char>(Addresses::FUNC_GET_PACKET_ID, packet);
        }
    }

    void AssertParams(int count, cell *params) {
        if (params[0] != (count * sizeof(cell))) {
            throw std::runtime_error{"number of parameters must be equal to " + std::to_string(count)};
        }
    }

    int SetAmxString(AMX *amx, cell amx_addr, const char *source, int max) {
        cell *dest = reinterpret_cast<cell *>(amx->base + static_cast<int>(reinterpret_cast<AMX_HEADER *>(amx->base)->dat + amx_addr));
        cell *start = dest;

        while (max-- && *source) {
            *dest++ = static_cast<cell>(*source++);
        }
        *dest = 0;

        return dest - start;
    }

    bool GetAmxPublicVar(AMX *amx, const char *name, cell &out) {
        cell addr{}, *phys_addr{};

        if (
            amx_FindPubVar(amx, name, &addr) == AMX_ERR_NONE &&
            amx_GetAddr(amx, addr, &phys_addr) == AMX_ERR_NONE
        ) {
            out = *phys_addr;

            return true;
        }

        return false;
    }

    std::string GetAmxString(AMX *amx, cell amx_addr) {
        int len{};
        cell *addr{};

        if (
            amx_GetAddr(amx, amx_addr, &addr) == AMX_ERR_NONE &&
            amx_StrLen(addr, &len) == AMX_ERR_NONE &&
            len
        ) {
            len++;

            std::unique_ptr<char[]> buf{new char[len]{}};

            if (
                buf &&
                amx_GetString(buf.get(), addr, 0, len) == AMX_ERR_NONE
            ) {
                return buf.get();
            }
        }

        return {};
    }

    RakNet::BitStream * GetAmxBitStream(cell amx_addr) {
        const auto bs = reinterpret_cast<RakNet::BitStream *>(amx_addr);

        if (!bs) {
            throw std::runtime_error{"invalid BitStream handle"};
        }

        return bs;
    }

    cell & GetAmxParamRef(AMX *amx, cell amx_addr) {
        cell *phys_addr{};

        if (amx_GetAddr(amx, amx_addr, &phys_addr) != AMX_ERR_NONE) {
            throw std::runtime_error{"invalid param"};
        }

        return *phys_addr;
    }

    template<typename T>
    struct Value {
        static inline void Push(AMX *amx, T value, cell &addr_release) {
            amx_Push(amx, static_cast<cell>(value));
        }
    };

    template<typename T>
    struct Value<T *> {
        static inline void Push(AMX *amx, T *value, cell &addr_release) {
            amx_Push(amx, reinterpret_cast<cell>(value));
        }
    };

    template<>
    struct Value<float> {
        static inline void Push(AMX *amx, float value, cell &addr_release) {
            amx_Push(amx, amx_ftoc(value));
        }
    };

    template<>
    struct Value<const char *> {
        static inline void Push(AMX *amx, const char *value, cell &addr_release) {
            cell tmp{};
            amx_PushString(amx, &tmp, nullptr, value, 0, 0);

            if (!addr_release) {
                addr_release = tmp;
            }
        }
    };

    inline void AmxPushInternal(AMX *amx, cell &addr_release) {
    }

    template<typename T>
    inline void AmxPushInternal(AMX *amx, cell &addr_release, T arg1) {
        Value<T>::Push(amx, arg1, addr_release);
    }

    template<typename T, typename... ARGS>
    inline void AmxPushInternal(AMX *amx, cell &addr_release, T arg1, ARGS ... args) {
        AmxPushInternal(amx, addr_release, args...);

        Value<T>::Push(amx, arg1, addr_release);
    }

    template<typename... ARGS>
    inline cell AmxCallPublic(AMX *amx, int index, ARGS ... args) {
        cell addr_release{}, retval{};

        AmxPushInternal(amx, addr_release, args...);

        amx_Exec(amx, &retval, index);

        if (addr_release) {
            amx_Release(amx, addr_release);
        }

        return retval;
    }
};

#endif // FUNCTIONS_H_
