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

#if ENABLE_METAL

#include "Pipeline.hpp"
#include "Device.hpp"
#include "MetalUtils.hpp"
#include "Shader.hpp"

#include <new>
#include <string>

Result Metal_CreatePipeline(Device* device, const PipelineDesc* desc, Pipeline** out, ALog* log) {
    if (out) *out = nullptr;
    if (device == nullptr || desc == nullptr || out == nullptr || desc->mVertexShader == nullptr ||
        desc->mColorFormat == FORMAT_UNDEFINED) {
        WriteMetalLog(log, ALog::Level::Error, "CreatePipeline received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if ((desc->mInputElementCount > 0 && desc->pInputElements == nullptr)) {
        WriteMetalLog(log, ALog::Level::Error, "CreatePipeline received missing vertex layout arrays");
        return RESULT_INVALID_ARGUMENT;
    }

    MTLPixelFormat colorFormat = MetalPixelFormat(desc->mColorFormat);
    if (colorFormat == MTLPixelFormatInvalid) {
        WriteMetalLog(log, ALog::Level::Error, "CreatePipeline received unsupported color format");
        return RESULT_UNSUPPORTED_API;
    }

    try {
        MTLRenderPipelineDescriptor* nativeDesc = [[MTLRenderPipelineDescriptor alloc] init];
        nativeDesc.vertexFunction = CompileShader(desc->mVertexShader, DEVICE(device)->pDevice, log);
        if (desc->mFragmentShader != nullptr) {
            nativeDesc.fragmentFunction = CompileShader(desc->mFragmentShader, DEVICE(device)->pDevice, log);
        }
        nativeDesc.colorAttachments[0].pixelFormat = colorFormat;
        nativeDesc.colorAttachments[0].blendingEnabled = desc->mEnableBlending;
        if (desc->mDepthStencilFormat != FORMAT_UNDEFINED) {
            nativeDesc.depthAttachmentPixelFormat = MetalPixelFormat(desc->mDepthStencilFormat);
        }

        // Create vertex descriptor
        if (desc->mInputElementCount > 0) {
            MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];

            for (uint32_t i = 0; i < desc->mInputElementCount; ++i) {
                const InputElement& element = desc->pInputElements[i];

                uint32_t actualStride = element.mStride;

                if (actualStride == 0) {
                    for (uint32_t j = 0; j < element.numAttribute; ++j) {
                        const InputAttribute& attr = element.pAttribute[j];
                        uint32_t formatSize = GetFormatByteSize(attr.mFormat);
                        uint32_t attributeEnd = attr.mOffset + formatSize;

                        if (attributeEnd > actualStride) actualStride = attributeEnd;
                    }
                }

                if (element.mBinding >= 31 || actualStride == 0) {
                    [vertexDesc release];
                    [nativeDesc release];
                    WriteMetalLog(log, ALog::Level::Error,
                                  "CreatePipeline received invalid vertex buffer layout "
                                  "(Stride is 0 or Binding out of bounds)");
                    return RESULT_INVALID_ARGUMENT;
                }

                vertexDesc.layouts[element.mBinding].stride = actualStride;
                vertexDesc.layouts[element.mBinding].stepFunction = MetalStepFunction(element.mRate);
                vertexDesc.layouts[element.mBinding].stepRate = 1;

                for (uint32_t j = 0; j < element.numAttribute; ++j) {
                    const InputAttribute& attr = element.pAttribute[j];
                    MTLVertexFormat format = MetalVertexFormat(attr.mFormat);

                    if (format == MTLVertexFormatInvalid || attr.mLocation >= 31) {
                        [vertexDesc release];
                        [nativeDesc release];
                        WriteMetalLog(log, ALog::Level::Error, "CreatePipeline received unsupported vertex attribute");
                        return RESULT_INVALID_ARGUMENT;
                    }

                    vertexDesc.attributes[attr.mLocation].format = format;
                    vertexDesc.attributes[attr.mLocation].offset = attr.mOffset;
                    vertexDesc.attributes[attr.mLocation].bufferIndex = element.mBinding;
                }
            }

            nativeDesc.vertexDescriptor = vertexDesc;
            [vertexDesc release];
        }

        // Create Rasterizer State
        nativeDesc.rasterizationEnabled = YES;

        NSError* error = nil;
        id<MTLRenderPipelineState> nativePipeline =
            [DEVICE(device)->pDevice newRenderPipelineStateWithDescriptor:nativeDesc error:&error];
        [nativeDesc release];

        if (nativePipeline == nil) {
            WriteNSError(log, "Failed to create Metal render pipeline", error);
            return RESULT_UNKNOWN_ERROR;
        }

        Metal_Pipeline* pipeline = new Metal_Pipeline(*desc);
        pipeline->pRenderPipeline = nativePipeline;
        pipeline->mType = PIPELINE_TYPE_GRAPHICS;
        pipeline->mCullMode = MetalCullMode(desc->mRasterizerState.mCullMode);
        pipeline->mWinding = MetalWinding(desc->mRasterizerState.mFrontFace);
        pipeline->mFillMode = MetalFillMode(desc->mRasterizerState.mFillMode);
        pipeline->mDepthBias = desc->mRasterizerState.mDepthBias;
        pipeline->mSlopeScale = desc->mRasterizerState.mSlopeScaledDepthBias;

        *out = pipeline;

        return RESULT_SUCCESS;
    } catch (const std::bad_alloc&) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to allocate Metal pipeline wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
}

void Metal_ReleasePipeline(Pipeline* pipeline) {
    if (pipeline == nullptr) return;

    Metal_Pipeline* metalPipeline = PIPELINE(pipeline);
    if (metalPipeline->pRenderPipeline != nil) {
        [metalPipeline->pRenderPipeline release];
        metalPipeline->pRenderPipeline = nil;
    }
    if (metalPipeline->pComputePipeline != nil) {
        [metalPipeline->pComputePipeline release];
        metalPipeline->pComputePipeline = nil;
    }
    if (metalPipeline->pDepthStencil != nil) {
        [metalPipeline->pDepthStencil release];
        metalPipeline->pDepthStencil = nil;
    }
    delete metalPipeline;
}

#endif
