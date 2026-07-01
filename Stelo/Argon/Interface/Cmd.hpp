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

#define ARGON_INTERNAL

#include "DeclareTypes.hpp"
#include "ALog.hpp"
#include "DeclareFunction.hpp"
#include "Enums.hpp"
#include "IUnknown.hpp"

struct BindResourceDesc {
    union {
        Buffer** ppBuffers;
        Texture** ppTextures;
    };
    const uint64_t* pOffsets;

    uint32_t mStartBinding;
    uint32_t mCount;
    ResourceType mResourceType;
    ShaderStage mShaderStage;
};

struct CmdDesc {
    const void* pNext = nullptr;
    const char* pDebugName = nullptr;
    bool mBeginImmediately = false;
};

struct Cmd : public IUnknown {
  private:
    DEFINE_GRAPHICS_API_TABLE(CmdBegin);

    DEFINE_GRAPHICS_API_TABLE(CmdActiveRenderPass);
    DEFINE_GRAPHICS_API_TABLE(CmdActiveComputePass);
    DEFINE_GRAPHICS_API_TABLE(CmdActiveTransferPass);

    DEFINE_GRAPHICS_API_TABLE(CmdSetViewport);
    DEFINE_GRAPHICS_API_TABLE(CmdSetScissor);
    DEFINE_GRAPHICS_API_TABLE(CmdSetStencilRef);
    DEFINE_GRAPHICS_API_TABLE(CmdBindPipeline);
    DEFINE_GRAPHICS_API_TABLE(CmdBindIndexBuffer);
    DEFINE_GRAPHICS_API_TABLE(CmdBindVertexBuffers);
    DEFINE_GRAPHICS_API_TABLE(CmdBindDescriptorSets);
    DEFINE_GRAPHICS_API_TABLE(CmdBindResource);

    DEFINE_GRAPHICS_API_TABLE(CmdResourceBarrier);
    DEFINE_GRAPHICS_API_TABLE(CmdUpdateBuffer);

    DEFINE_GRAPHICS_API_TABLE(CmdDraw);
    DEFINE_GRAPHICS_API_TABLE(CmdDrawInstanced);
    DEFINE_GRAPHICS_API_TABLE(CmdDrawIndexed);
    DEFINE_GRAPHICS_API_TABLE(CmdDrawIndexedInstanced);
    DEFINE_GRAPHICS_API_TABLE(CmdDispatch);
    DEFINE_GRAPHICS_API_TABLE(CmdEnd);
    DEFINE_GRAPHICS_API_TABLE(ReleaseCmd);
  protected:
    CmdState mState = CMD_STATE_IDLE;
    
    Cmd(GraphicsApi api) : IUnknown(api) {}
    void SetState(CmdState state) {
        mState = state;
    }

  public:
    CmdState GetState() const {
        return mState;
    }

    Result Begin( ALog* pLog = nullptr) {
        return _CmdBegin[GetIndexAPI()](this, pLog);
    }

    Result ActiveRenderPass(RenderPass* pRenderPass, ALog* pLog = nullptr) {
        return _CmdActiveRenderPass[GetIndexAPI()](this, pRenderPass, pLog);
    }
    Result ActiveComputePass(ALog* pLog = nullptr) {
        return _CmdActiveComputePass[GetIndexAPI()](this, pLog);
    }
    Result ActiveTransferPass(ALog* pLog = nullptr) {
        return _CmdActiveTransferPass[GetIndexAPI()](this, pLog);
    }

    Result SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth, ALog* pLog = nullptr) {
        return _CmdSetViewport[GetIndexAPI()](this, x, y, width, height, minDepth, maxDepth, pLog);
    }
    Result SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, ALog* pLog = nullptr) {
        return _CmdSetScissor[GetIndexAPI()](this, x, y, width, height, pLog);
    }
    Result SetStencilRef(uint32_t val, ALog* pLog = nullptr) {
        return _CmdSetStencilRef[GetIndexAPI()](this, val, pLog);
    }
    Result BindPipeline(Pipeline* pPipeline, ALog* pLog = nullptr) {
        return _CmdBindPipeline[GetIndexAPI()](this, pPipeline, pLog);
    }
    Result BindIndexBuffer(Buffer* pBuffer, IndexType indexType, uint64_t offset, ALog* pLog = nullptr) {
        return _CmdBindIndexBuffer[GetIndexAPI()](this, pBuffer, indexType, offset, pLog);
    }
    Result BindVertexBuffers(uint32_t bufferCount, Buffer** ppBuffers, const uint32_t* pStrides, const uint64_t* pOffsets, ALog* pLog = nullptr) {
        return _CmdBindVertexBuffers[GetIndexAPI()](this, bufferCount, ppBuffers, pStrides, pOffsets, pLog);
    }
    Result BindDescriptorSets(PipelineLayout* pLayout, uint32_t firstSet, uint32_t setCount, const DescriptorSet** ppSets, const uint32_t* pDynamicOffsets, ALog* pLog = nullptr) {
        return _CmdBindDescriptorSets[GetIndexAPI()](this, pLayout, firstSet, setCount, ppSets, pDynamicOffsets, pLog);
    }
    Result BindResource(uint32_t mNum, const BindResourceDesc* pBinding, ALog* pLog = nullptr) {
        return _CmdBindResource[GetIndexAPI()](this, mNum, pBinding, pLog);
    }
    Result ResourceBarrier(uint32_t barrierCount, const BufferBarrier* pBarriers, ALog* pLog = nullptr) {
        return _CmdResourceBarrier[GetIndexAPI()](this, barrierCount, pBarriers, pLog);
    }
    Result UpdateBuffer(const BufferUpdateDesc* pDesc, ALog* pLog = nullptr) {
        return _CmdUpdateBuffer[GetIndexAPI()](this, pDesc, pLog);
    }
    Result Draw(uint32_t vertexCount, uint32_t firstVertex, ALog* pLog = nullptr) {
        return _CmdDraw[GetIndexAPI()](this, vertexCount, firstVertex, pLog);
    }
    Result DrawInstanced(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance, ALog* pLog = nullptr) {
        return _CmdDrawInstanced[GetIndexAPI()](this, vertexCount, firstVertex, instanceCount, firstInstance, pLog);
    }
    Result DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex, ALog* pLog = nullptr) {
        return _CmdDrawIndexed[GetIndexAPI()](this, indexCount, firstIndex, firstVertex, pLog);
    }
    Result DrawIndexedInstanced(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance, ALog* pLog = nullptr) {
        return _CmdDrawIndexedInstanced[GetIndexAPI()](this, indexCount, firstIndex, instanceCount, firstVertex, firstInstance, pLog);
    }
    Result Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ, ALog* pLog = nullptr) {
        return _CmdDispatch[GetIndexAPI()](this, groupCountX, groupCountY, groupCountZ, pLog);
    }

    Result End(ALog* pLog = nullptr) {
        return _CmdEnd[GetIndexAPI()](this, pLog);
    }
    void Release() {
        _ReleaseCmd[GetIndexAPI()](this);
    }
};

#undef ARGON_INTERNAL
