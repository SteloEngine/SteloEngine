/*
 * Copyright (c) 2026 Stelo Engine
 *
 * This file is part of Stelo Engine.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations
 * under the License.
*/

#pragma once

#include <cstdint>
#include <random>
#include <chrono>

namespace Stelo {

struct UUID {
    union {
        uint8_t bytes[16];
        struct {
            uint64_t high;
            uint64_t low;
        };
    };

    static UUID Generate() {
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        static std::mt19937_64 rng(std::random_device{}());
        uint64_t random_val = rng();

        UUID uuid;
        uuid.high = (timestamp << 16) | 0x7000 | (random_val & 0x0FFF);
        uuid.low = rng();
        uuid.low = (uuid.low & 0x3FFFFFFFFFFFFFFF) | 0x8000000000000000;

        return uuid;
    }
    
    bool operator==(const UUID& other) const {
        return high == other.high && low == other.low;
    }
};

}