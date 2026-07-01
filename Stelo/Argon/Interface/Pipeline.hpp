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

#include "DeclareFunction.hpp"
#include "Enums.hpp"
#include "IUnknown.hpp"

struct InputAttribute {
    /// @brief Semantic name
    /// @note You can use semantic name from ShaderSemantic enum or custom
    /// semantic name
    const char* mSemanticName;
    uint32_t mLocation;
    uint32_t mOffset;
    Format mFormat;
    ShaderSemantic mSemantic;
};

struct InputElement {
    const InputAttribute* pAttribute;
    uint32_t numAttribute;
    uint32_t mBinding;
    uint32_t mStride;
    VertexStepRate mRate;
};

struct RasterizerStateDesc {
    float mSlopeScaledDepthBias = 0.0f;
    int32_t mDepthBias = 0;
    CullMode mCullMode = CULL_MODE_BACK;
    FillMode mFillMode = FILL_MODE_SOLID;
    FrontFace mFrontFace = FRONT_FACE_CW;
    bool mMultiSample = false;
    bool mScissor = false;
    bool mDepthClampEnable = false;
};

// struct DepthStateDesc {
//     bool      mDepthTest;
//     bool      mDepthWrite;
//     CompareOp mDepthFunc;
//     bool      mStencilTest;
//     uint8_t   mStencilReadMask;
//     uint8_t   mStencilWriteMask;
//     CompareOp mStencilFrontFunc;
//     StencilOp mStencilFrontFail;
//     StencilOp mDepthFrontFail;
//     StencilOp mStencilFrontPass;
//     CompareOp mStencilBackFunc;
//     StencilOp mStencilBackFail;
//     StencilOp mStencilBackPass;
// };

struct PipelineDesc {
    RasterizerStateDesc mRasterizerState;
    const void* pNext = nullptr;
    const char* pDebugName = nullptr;
    ShaderDesc* mVertexShader;
    ShaderDesc* mFragmentShader;
    const InputElement* pInputElements = nullptr;
    uint32_t mInputElementCount = 0;
    PipelineType mType = PIPELINE_TYPE_GRAPHICS;
    Format mColorFormat = FORMAT_BGRA8_UNORM;
    Format mDepthStencilFormat = FORMAT_UNDEFINED;
    PrimitiveTopology mTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    bool mEnableBlending = false;
};

struct Pipeline : public IUnknown {
  private:
    DEFINE_GRAPHICS_API_TABLE(ReleasePipeline);
    PrimitiveTopology mTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  protected:
    Pipeline(GraphicsApi api, const PipelineDesc& desc) : IUnknown(api), mTopology(desc.mTopology) {}

  public:
    PrimitiveTopology GetTopology() const {
        return mTopology;
    }
    void Release() {
        _ReleasePipeline[GetIndexAPI()](this);
    }
};

#undef ARGON_INTERNAL
