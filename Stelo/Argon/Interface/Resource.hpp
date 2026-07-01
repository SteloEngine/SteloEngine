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

#include "Enums.hpp"
#include "IUnknown.hpp"

struct Resource : public IUnknown {
private:
protected:
    Resource(GraphicsApi api, ResourceType type) : IUnknown(api), mType(type) {}
    ResourceStateFlags mState = RESOURCE_STATE_UNDEFINED;
    const ResourceType mType;
public:
    inline ResourceType GetType() const {
        return mType;
    } 
    inline ResourceStateFlags GetState() const {
        return mState;
    }
};
