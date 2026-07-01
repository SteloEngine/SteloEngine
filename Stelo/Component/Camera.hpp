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

#include "../ECS/Component.hpp"
#include "../System/GraphicManager.hpp"
#include "../Math/Matrix.hpp"
#include "../World/Transform.hpp"

namespace Stelo {

struct Camera : public Component {
private:
    Matrix4x4 viewProj;
    Widget* target;
    Buffer* buffer;
    uint32_t tsfVersion;
    float farZ, nearZ;
    union {
        struct {
            float fov, aspect;
        } perspective;
        struct {
            float width, height;
        } orthographic;
    };
    enum Type {
        Perspective,
        Orthographic
    } type = Type::Perspective;
    bool dirty;
public:
    Camera() : farZ(1000.0f), nearZ(0.1f), type(Type::Perspective), dirty(true),
               viewProj(), buffer(nullptr), tsfVersion(0) {}
    void OnAwake() {
        BufferDesc desc;
        desc.mSize = sizeof(Matrix4x4);
        desc.mUsage = BUFFER_USAGE_UNIFORM_BUFFER;
        desc.mFlags = BUFFER_CREATE_BINDING;
        desc.mMemoryUsage = MEMORY_USAGE_CPU_TO_GPU;
        GraphicManager::GetDevice()->CreateBuffer(&desc, &buffer);

        Comp<Camera> cp = CompDynamicCast<Camera>(GetHandle());

        target = GraphicManager::GetTarget();
        target->Bind(Event::Resize, 0, [cp](EventPtr e) {
            cp->OnTargetResize();
        });
        target->Bind(Event::Destroy, 0, [cp](EventPtr e) {
            cp->target->Unbind(Event::Resize, 0);
            cp->target->Unbind(Event::Destroy, 0);
            cp->target = nullptr;
        });
        
        if(type == Type::Perspective) perspective.fov = 60.0f;
        OnTargetResize();
    }

    void OnTargetResize() {
        float w = target->GetSize().width;
        float h = target->GetSize().height;

        if(type == Type::Perspective) perspective.aspect = w / h;
        else {
            orthographic.width = w;
            orthographic.height = h;
        }
        
        dirty = true;
    }

    void PreRender() {
        if(target == nullptr) return;
        Transform* tsf = GetTransform().GetUnsafePtr();
        if(!dirty && tsfVersion == tsf->GetVersion()) return;
        
        Matrix4x4 proj;
        Matrix4x4 view = Matrix4x4::LookToLH(tsf->GetPosition(), tsf->Forward(), tsf->Up());
        if(type == Type::Perspective) proj = Matrix4x4::PerFovLH(perspective.fov, perspective.aspect, nearZ, farZ);
        else proj = Matrix4x4::OrthoLH(orthographic.width, orthographic.height, nearZ, farZ);
        
        viewProj = view * proj;
        dirty = false;
        tsfVersion = tsf->GetVersion();

        MapBufferDesc mapDesc{.pBuffer = buffer, .mOffset = 0, .mSize = sizeof(Matrix4x4)};
        GraphicManager::GetDevice()->MapBuffer(&mapDesc);
        std::memcpy(mapDesc.pMapped, &viewProj, sizeof(Matrix4x4));
        GraphicManager::GetDevice()->UnmapBuffer(&mapDesc);
    }

    void Render() {
        BindResourceDesc bindDesc;
        bindDesc.ppBuffers     = &buffer;
        bindDesc.pOffsets      = nullptr;
        bindDesc.mStartBinding = 1;
        bindDesc.mCount        = 1;
        bindDesc.mResourceType = RESOURCE_BUFFER;
        bindDesc.mShaderStage  = SHADER_STAGE_VERTEX;
        
        GraphicManager::Cmd()->BindResource(1, &bindDesc); 
    }

    void OnDestroy() {
        if(target != nullptr) {
            target->Unbind(Event::Resize, 0);
            target->Unbind(Event::Destroy, 0);
            target = nullptr;
        }
        if(buffer != nullptr) {
            buffer->Release();
            buffer = nullptr;
        }
    }
};

REGISTER_COMPONENT(Camera)

}
