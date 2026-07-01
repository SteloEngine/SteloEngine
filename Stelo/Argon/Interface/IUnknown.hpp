/*
 * Copyright (c) 2026 Stelo Engine
 *
 * This file is part of Stelo Engine.
 *
 * Licensed under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once

#include "../Config.hpp"
#include "Enums.hpp"
#include <array>

class IUnknown {
  private:
    static std::array<uint8_t, GRAPHICS_API_COUNT> CreateLookupTable() {
        uint8_t current_index = 1;
        std::array<uint8_t, GRAPHICS_API_COUNT> lookup {};
        lookup[GRAPHICS_API_UNKNOWN] = 0;
#define STELO_ASSIGN_API_INDEX(Name, _) lookup[GRAPHICS_API_##Name] = (ENABLE_##Name) ? current_index++ : 0; 
        GRAPHICS_API_LIST(STELO_ASSIGN_API_INDEX, _)
#undef STELO_ASSIGN_API_INDEX
        return lookup;
    }
    inline const static auto G_ApiLookupTable = CreateLookupTable();
  protected:
    const GraphicsApi mApi;
    IUnknown(GraphicsApi api) : mApi(api) {}
  public:
    inline constexpr GraphicsApi GetAPI() const {
        return mApi;
    }
    inline uint8_t GetIndexAPI() const {
        return G_ApiLookupTable[mApi];
    }
};