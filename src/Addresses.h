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

#ifndef ADDRESSES_H_
#define ADDRESSES_H_

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall

enum RakServerOffsets {
    SEND = 7,
    RPC = 32,
    RECEIVE = 10,
    REGISTER_AS_REMOTE_PROCEDURE_CALL = 29,
    DEALLOCATE_PACKET = 12,
    GET_INDEX_FROM_PLAYER_ID = 57,
    GET_PLAYER_ID_FROM_INDEX = 58
};
#else
#define THISCALL

enum RakServerOffsets {
    SEND = 9,
    RPC = 35,
    RECEIVE = 11,
    REGISTER_AS_REMOTE_PROCEDURE_CALL = 30,
    DEALLOCATE_PACKET = 13,
    GET_INDEX_FROM_PLAYER_ID = 58,
    GET_PLAYER_ID_FROM_INDEX = 59
};
#endif

namespace Addresses {
    urmem::address_t
        PTR_RAKSERVER{},
        FUNC_RAKSERVER__SEND{},
        FUNC_RAKSERVER__RPC{},
        FUNC_RAKSERVER__RECEIVE{},
        FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL{},
        FUNC_RAKSERVER__DEALLOCATE_PACKET{},
        FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID{},
        FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX{},
        FUNC_GET_PACKET_ID{};

    urmem::address_t * Init(urmem::address_t rakserver) {
        if (const auto vmt = urmem::pointer(PTR_RAKSERVER = rakserver).field<urmem::address_t *>(0)) {
            FUNC_RAKSERVER__SEND = vmt[RakServerOffsets::SEND];
            FUNC_RAKSERVER__RPC = vmt[RakServerOffsets::RPC];
            FUNC_RAKSERVER__RECEIVE = vmt[RakServerOffsets::RECEIVE];
            FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL = vmt[RakServerOffsets::REGISTER_AS_REMOTE_PROCEDURE_CALL];
            FUNC_RAKSERVER__DEALLOCATE_PACKET = vmt[RakServerOffsets::DEALLOCATE_PACKET];
            FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID = vmt[RakServerOffsets::GET_INDEX_FROM_PLAYER_ID];
            FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX = vmt[RakServerOffsets::GET_PLAYER_ID_FROM_INDEX];

            return vmt;
        }

        return nullptr;
    }
};

#endif // ADDRESSES_H_
