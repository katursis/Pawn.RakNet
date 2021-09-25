/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 katursis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PAWNRAKNET_MAIN_H_
#define PAWNRAKNET_MAIN_H_

#include "samp-ptl/ptl.h"
#include "RakNet/BitStream.h"
#include "RakNet/StringCompressor.h"
#include "RakNet/PluginInterface.h"
#include "urmem/urmem.hpp"
#include "cpptoml/include/cpptoml.h"

#include <unordered_set>
#include <set>
#include <limits>
#include <list>
#include <array>
#include <string>
#include <regex>
#include <queue>
#include <thread>
#include <atomic>
#include <vector>

#include "Pawn.RakNet.inc"

#ifdef THISCALL
#undef THISCALL
#endif

#ifdef _WIN32
#define THISCALL __thiscall
#else
#define THISCALL
#endif

#include "config.h"
#include "bitstream_pool.h"
#include "internal_packet_channel.h"
#include "rakserver.h"
#include "script.h"
#include "native_param.h"
#include "plugin.h"
#include "hooks.h"

#endif  // PAWNRAKNET_MAIN_H_
