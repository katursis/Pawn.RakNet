/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020 urShadow
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

namespace Addresses {
#ifdef _WIN32
    enum RakServerOffset {
        SEND = 7,
        RPC = 32,
        RECEIVE = 10,
        REGISTER_AS_REMOTE_PROCEDURE_CALL = 29,
        DEALLOCATE_PACKET = 12,
        ATTACH_PLUGIN = 41,
        GET_INDEX_FROM_PLAYER_ID = 57,
        GET_PLAYER_ID_FROM_INDEX = 58
    };
#else
    enum RakServerOffset {
        SEND = 9,
        RPC = 35,
        RECEIVE = 11,
        REGISTER_AS_REMOTE_PROCEDURE_CALL = 30,
        DEALLOCATE_PACKET = 13,
        ATTACH_PLUGIN = 42,
        GET_INDEX_FROM_PLAYER_ID = 58,
        GET_PLAYER_ID_FROM_INDEX = 59
    };
#endif

    urmem::address_t
        PTR_RAKSERVER{},

        FUNC_RAKSERVER__SEND{},
        FUNC_RAKSERVER__RPC{},
        FUNC_RAKSERVER__RECEIVE{},
        FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL{},
        FUNC_RAKSERVER__DEALLOCATE_PACKET{},
        FUNC_RAKSERVER__ATTACH_PLUGIN{},
        FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID{},
        FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX{},

        FUNC_GET_PACKET_ID{};

        void InitRakServer(urmem::address_t rakserver, urmem::address_t *vmt) {
            PTR_RAKSERVER = rakserver;

            FUNC_RAKSERVER__SEND = vmt[RakServerOffset::SEND];
            FUNC_RAKSERVER__RPC = vmt[RakServerOffset::RPC];
            FUNC_RAKSERVER__RECEIVE = vmt[RakServerOffset::RECEIVE];
            FUNC_RAKSERVER__REGISTER_AS_REMOTE_PROCEDURE_CALL = vmt[RakServerOffset::REGISTER_AS_REMOTE_PROCEDURE_CALL];
            FUNC_RAKSERVER__DEALLOCATE_PACKET = vmt[RakServerOffset::DEALLOCATE_PACKET];
            FUNC_RAKSERVER__ATTACH_PLUGIN = vmt[RakServerOffset::ATTACH_PLUGIN];
            FUNC_RAKSERVER__GET_INDEX_FROM_PLAYER_ID = vmt[RakServerOffset::GET_INDEX_FROM_PLAYER_ID];
            FUNC_RAKSERVER__GET_PLAYER_ID_FROM_INDEX = vmt[RakServerOffset::GET_PLAYER_ID_FROM_INDEX];
        }
};

#endif // ADDRESSES_H_
