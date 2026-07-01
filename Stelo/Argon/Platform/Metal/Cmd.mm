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

#include "Buffer.hpp"
#include "Cmd.hpp"
#include "MetalUtils.hpp"
#include "Pipeline.hpp"
#include "Texture.hpp"
#include "PipelineLayout.hpp"

#include "../../Interface/CmdPass.hpp"
#include "../../Interface/ResourceBarrier.hpp"

#include <new>

constexpr uint32_t MAX_METAL_BINDINGS = 31;

namespace {
void EndActiveEncoders(Metal_Cmd* cmd) {
    if (cmd->pRenderEncoder != nil) {
        [cmd->pRenderEncoder endEncoding];
        cmd->pRenderEncoder = nil;
    }
    if (cmd->pComputeEncoder != nil) {
        [cmd->pComputeEncoder endEncoding];
        cmd->pComputeEncoder = nil;
    }
    if (cmd->pBlitEncoder != nil) {
        [cmd->pBlitEncoder endEncoding];
        cmd->pBlitEncoder = nil;
    }
}

void ReleaseRecordedObjects(Metal_Cmd* cmd) {
    if (cmd->pRenderEncoder != nil) {
        cmd->pRenderEncoder = nil;
    }
    if (cmd->pCommandBuffer != nil) {
        [cmd->pCommandBuffer release];
        cmd->pCommandBuffer = nil;
    }
}

NSUInteger IndexBufferOffset(const Metal_Cmd* cmd, uint32_t firstIndex) {
    const uint64_t indexSize = (cmd->mIndexType == MTLIndexTypeUInt16) ? sizeof(uint16_t) : sizeof(uint32_t);
    return static_cast<NSUInteger>(cmd->mIndexOffset + static_cast<uint64_t>(firstIndex) * indexSize);
}
} // namespace

Result Metal_CreateCmd(Queue* queue, const CmdDesc* desc, Cmd** out, ALog* log) {
    if (out) *out = nullptr;
    if (queue == nullptr || out == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "CreateCmd received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (queue->GetAPI() != GRAPHICS_API_METAL) {
        WriteMetalLog(log, ALog::Level::Error, "CreateCmd received queue from a different backend");
        return RESULT_INVALID_ARGUMENT;
    }

    try {
        Metal_Cmd* cmd = new Metal_Cmd();
        cmd->pQueue = QUEUE(queue);
        *out = cmd;

        if (desc != nullptr && desc->mBeginImmediately) {
            Result result = Metal_CmdBegin(cmd, log);
            if (result != RESULT_SUCCESS) {
                Metal_ReleaseCmd(cmd);
                *out = nullptr;
                return result;
            }
        }
        return RESULT_SUCCESS;
    } catch (const std::bad_alloc&) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to allocate Metal command wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
}
Result Metal_CmdBegin(Cmd* cmd, ALog* log) {
    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->GetState() == CMD_STATE_RECORDING) return RESULT_SUCCESS;
    if (metalCmd->GetState() == CMD_STATE_SUBMITTED && metalCmd->pCommandBuffer != nil) {
        [metalCmd->pCommandBuffer waitUntilCompleted];
    }
    ReleaseRecordedObjects(metalCmd);
    metalCmd->pCommandBuffer = [[metalCmd->pQueue->pCommandQueue commandBuffer] retain];
    if (metalCmd->pCommandBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal command buffer");
        return RESULT_OUT_OF_DEVICE_MEMORY;
    }

    metalCmd->SetBackendState(CMD_STATE_RECORDING);
    return RESULT_SUCCESS;
}
Result Metal_CmdBindPipeline(Cmd* cmd, Pipeline* pipeline, ALog* log) {
    if (cmd == nullptr || pipeline == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetPipeline received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetPipeline requires a recording command");
        return RESULT_INVALID_STATE;
    }
    if (pipeline->GetAPI() != GRAPHICS_API_METAL) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetPipeline received pipeline from a different backend");
        return RESULT_INVALID_ARGUMENT;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetPipeline requires an active render pass");
        return RESULT_INVALID_STATE;
    }

    Metal_Pipeline* metalPipeline = PIPELINE(pipeline);
    metalCmd->mPrimitiveType = MetalPrimitiveType(metalPipeline->GetTopology());

    if (metalPipeline->pRenderPipeline) {
        [metalCmd->pRenderEncoder setRenderPipelineState:metalPipeline->pRenderPipeline];
        [metalCmd->pRenderEncoder setFrontFacingWinding:metalPipeline->mWinding];
        [metalCmd->pRenderEncoder setCullMode:metalPipeline->mCullMode];
        [metalCmd->pRenderEncoder setTriangleFillMode:metalPipeline->mFillMode];
    } else
        WriteMetalLog(log, ALog::Level::Warning,
                      "CmdSetPipeline: Pipeline has NO native render pipeline "
                      "state binded (Skipping binding)");

    if (metalPipeline->pDepthStencil) {
        [metalCmd->pRenderEncoder setDepthStencilState:metalPipeline->pDepthStencil];
        [metalCmd->pRenderEncoder setDepthClipMode:MTLDepthClipModeClamp];
    } else
        WriteMetalLog(log, ALog::Level::Warning,
                      "CmdSetPipeline: Pipeline has NO native depth stencil state "
                      "binded (Skipping binding)");
    return RESULT_SUCCESS;
}
Result Metal_CmdBindVertexBuffers(Cmd* pCmd, uint32_t bufferCount, Buffer** ppBuffers, const uint32_t* pStrides,
                                const uint64_t* pOffsets, ALog* log) {
    if (pCmd == nullptr || ppBuffers == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetVertexBuffer received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (pCmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetVertexBuffer requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(pCmd);
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdSetVertexBuffer requires an active render pass");
        return RESULT_INVALID_STATE;
    }

    std::vector<id<MTLBuffer>> mtlBuffers(bufferCount);
    std::vector<NSUInteger> offsets(bufferCount);

    for (uint32_t i = 0; i < bufferCount; ++i) {
        mtlBuffers[i] = BUFFER(ppBuffers[i])->pBuffer;
        offsets[i] = pOffsets ? pOffsets[i] : 0;
    }

    [metalCmd->pRenderEncoder setVertexBuffers:mtlBuffers.data()
                                       offsets:offsets.data()
                                     withRange:NSMakeRange(0, bufferCount)];
    return RESULT_SUCCESS;
}
Result Metal_CmdBindIndexBuffer(Cmd* cmd, Buffer* pBuffer, IndexType indexType, uint64_t offset, ALog* log) {
    Metal_Cmd* metalCmd = (Metal_Cmd*)cmd;
    id<MTLRenderCommandEncoder> renderEncoder = metalCmd->pRenderEncoder;
    
    if (renderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdBindIndexBuffer requires an active render pass");
        return RESULT_INVALID_STATE;
    }
    
    Metal_Buffer* metalBuffer = (Metal_Buffer*)pBuffer;
    MTLIndexType mtlIndexType = (indexType == INDEX_TYPE_UINT16) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
    
    metalCmd->mIndexType = mtlIndexType;
    metalCmd->pIndexBuffer = metalBuffer->pBuffer;
    metalCmd->mIndexOffset = offset;

    return RESULT_SUCCESS;
}
// NOT SUPPORTED FOR COMPUTE YET
Result Metal_CmdBindDescriptorSets(Cmd* cmd, PipelineLayout* pLayout, uint32_t firstSet, uint32_t setCount, const DescriptorSet** ppSets, const uint32_t* pDynamicOffsets, ALog* log) {
    Metal_Cmd* metalCmd = (Metal_Cmd*)cmd;
    Metal_PipelineLayout* metalLayout = (Metal_PipelineLayout*)pLayout;
    id<MTLRenderCommandEncoder> renderEncoder = metalCmd->pRenderEncoder;

    for (uint32_t i = 0; i < setCount; ++i) {
        uint32_t setIdx = firstSet + i;
        auto* metalSet = (Metal_DescriptorSet*)ppSets[i];
        const auto& mapping = metalLayout->mSetToSlotMappings[setIdx];
        
        if (mapping.vertexSlot != 0xFFFFFFFF) {
            [renderEncoder setVertexBuffer:metalSet->pArgumentBuffer offset:0 atIndex:mapping.vertexSlot];
        }
        if (mapping.fragmentSlot != 0xFFFFFFFF) {
            [renderEncoder setFragmentBuffer:metalSet->pArgumentBuffer offset:0 atIndex:mapping.fragmentSlot];
        }
    }
    return RESULT_SUCCESS;
}
Result Metal_CmdBindResource(Cmd* cmd, uint32_t numResources, const BindResourceDesc* pResources, ALog* log) {
    if (cmd == nullptr || pResources == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "BindResource received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "BindResource requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    id<MTLRenderCommandEncoder> encoder = metalCmd->pRenderEncoder;
    if (encoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "BindResource requires a render encoder");
        return RESULT_INVALID_STATE;
    }

    for (uint32_t i = 0; i < numResources; ++i) {
        const BindResourceDesc& resource = pResources[i];

        if (resource.mCount == 0 || resource.mCount > MAX_METAL_BINDINGS) {
            WriteMetalLog(log, ALog::Level::Warning, "Resource count is out of valid bounds (1-31)");
            continue;
        }

        NSRange range = NSMakeRange(resource.mStartBinding, resource.mCount);

        switch (resource.mResourceType) {
            case RESOURCE_BUFFER: {
                id<MTLBuffer> mtlBuffers[MAX_METAL_BINDINGS];
                NSUInteger mtlOffsets[MAX_METAL_BINDINGS];
                
                for (uint32_t b = 0; b < resource.mCount; ++b) {
                    mtlBuffers[b] = BUFFER(resource.ppBuffers[b])->pBuffer;
                    mtlOffsets[b] = (resource.pOffsets != nullptr) ? resource.pOffsets[b] : 0; 
                }
                
                if (HasFlag(resource.mShaderStage, SHADER_STAGE_VERTEX)) {
                    [encoder setVertexBuffers:mtlBuffers offsets:mtlOffsets withRange:range];
                }
                if (HasFlag(resource.mShaderStage, SHADER_STAGE_FRAGMENT)) {
                    [encoder setFragmentBuffers:mtlBuffers offsets:mtlOffsets withRange:range];
                }
                break;
            }
            case RESOURCE_TEXTURE: {
                id<MTLTexture> mtlTextures[MAX_METAL_BINDINGS];
                
                for (uint32_t b = 0; b < resource.mCount; ++b) {
                    mtlTextures[b] = TEXTURE(resource.ppTextures[b])->pTexture;
                }
                
                if (HasFlag(resource.mShaderStage, SHADER_STAGE_VERTEX)) {
                    [encoder setVertexTextures:mtlTextures withRange:range];
                }
                if (HasFlag(resource.mShaderStage, SHADER_STAGE_FRAGMENT)) {
                    [encoder setFragmentTextures:mtlTextures withRange:range];
                }
                break;
            }
            default:
                WriteMetalLog(log, ALog::Level::Error, "BindResourceGroups received invalid resource type");
                return RESULT_INVALID_ARGUMENT;
        }
    }
    
    return RESULT_SUCCESS;
}
Result Metal_CmdActiveRenderPass(Cmd* cmd, RenderPass* info, ALog* log) {
    if (cmd == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "ActiveRenderPass received invalid command");
        return RESULT_INVALID_ARGUMENT;
    }
    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "ActiveRenderPass called while command is not in recording state");
        return RESULT_INVALID_STATE;
    }
    if (info == nullptr || info->mColorAttachmentCount == 0) return RESULT_SUCCESS;
    if (info->pColorAttachments == nullptr || info->mColorAttachmentCount > 8) {
        WriteMetalLog(log, ALog::Level::Error, "CmdBegin received invalid color attachments");
        return RESULT_INVALID_ARGUMENT;
    }

    EndActiveEncoders(metalCmd);

    MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
    for (uint32_t i = 0; i < info->mColorAttachmentCount; ++i) {
        const ColorAttachment& attachment = info->pColorAttachments[i];
        if (attachment.pTexture == nullptr || attachment.pTexture->GetAPI() != GRAPHICS_API_METAL) {
            WriteMetalLog(log, ALog::Level::Error, "CmdBegin received invalid color attachment texture");
            return RESULT_INVALID_ARGUMENT;
        }

        Metal_Texture* texture = TEXTURE(attachment.pTexture);
        if (texture->pTexture == nil) {
            WriteMetalLog(log, ALog::Level::Error, "CmdBegin received empty Metal texture");
            return RESULT_INVALID_ARGUMENT;
        }

        pass.colorAttachments[i].texture = texture->pTexture;
        pass.colorAttachments[i].loadAction = MetalLoadAction(attachment.mLoadOp);
        pass.colorAttachments[i].storeAction = MetalStoreAction(attachment.mStoreOp);
        pass.colorAttachments[i].clearColor = MTLClearColorMake(attachment.mClearColor.r, attachment.mClearColor.g, 
                                                                attachment.mClearColor.b, attachment.mClearColor.a);
    }

    if (info->pDepthStencilAttachment != nullptr) {
        const DepthStencilAttachment& attachment = *info->pDepthStencilAttachment;
        if (attachment.pTexture == nullptr || attachment.pTexture->GetAPI() != GRAPHICS_API_METAL) {
            WriteMetalLog(log, ALog::Level::Error, "CmdBegin received invalid depth texture");
            return RESULT_INVALID_ARGUMENT;
        }

        Metal_Texture* texture = TEXTURE(attachment.pTexture);
        pass.depthAttachment.texture = texture->pTexture;
        pass.depthAttachment.loadAction = MetalLoadAction(attachment.mDepthLoadOp);
        pass.depthAttachment.storeAction = MetalStoreAction(attachment.mDepthStoreOp);
        pass.depthAttachment.clearDepth = attachment.mClearValue.mDepth;
        pass.stencilAttachment.texture = texture->pTexture;
        pass.stencilAttachment.loadAction = MetalLoadAction(attachment.mStencilLoadOp);
        pass.stencilAttachment.storeAction = MetalStoreAction(attachment.mStencilStoreOp);
        pass.stencilAttachment.clearStencil = attachment.mClearValue.mStencil;
    }

    metalCmd->pRenderEncoder = [metalCmd->pCommandBuffer renderCommandEncoderWithDescriptor:pass];
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal render command encoder");
        return RESULT_UNKNOWN_ERROR;
    }

    return RESULT_SUCCESS;
}
Result Metal_CmdActiveComputePass(Cmd* cmd, ALog* log) {
    Metal_Cmd* metalCmd = static_cast<Metal_Cmd*>(cmd);
    EndActiveEncoders(metalCmd);

    metalCmd->pComputeEncoder = [metalCmd->pCommandBuffer computeCommandEncoder];
    if (metalCmd->pComputeEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal compute command encoder");
        return RESULT_UNKNOWN_ERROR;
    }

    return RESULT_SUCCESS;
}
Result Metal_CmdActiveTransferPass(Cmd* cmd, ALog* log) {
    Metal_Cmd* metalCmd = static_cast<Metal_Cmd*>(cmd);
    if (metalCmd->pBlitEncoder != nil) {
        return RESULT_SUCCESS; 
    }
    EndActiveEncoders(metalCmd);
    metalCmd->pBlitEncoder = [metalCmd->pCommandBuffer blitCommandEncoder];
    if (metalCmd->pBlitEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal blit command encoder");
        return RESULT_UNKNOWN_ERROR;
    }
    return RESULT_SUCCESS;
}
Result Metal_CmdResourceBarrier(Cmd* cmd, uint32_t barrierCount, const BufferBarrier* pBarriers, ALog* log) {
    if (cmd == nullptr || pBarriers == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "CmdResourceBarrier received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    std::vector<id<MTLResource>> resources(barrierCount);
    
    for (uint32_t i = 0; i < barrierCount; ++i) {
        const BufferBarrier& barrier = pBarriers[i];
        Metal_Buffer* metalBuffer = BUFFER(barrier.pBuffer);
        
        if (metalBuffer == nullptr || metalBuffer->pBuffer == nil) {
            WriteMetalLog(log, ALog::Level::Error, "CmdResourceBarrier received invalid buffer");
            return RESULT_INVALID_ARGUMENT;
        }

        resources[i] = metalBuffer->pBuffer;
        metalBuffer->SetState(barrier.mNewState);
    }

    if (metalCmd->pRenderEncoder != nil) {
        [metalCmd->pRenderEncoder memoryBarrierWithResources:resources.data() count:barrierCount
                                      afterStages:MTLRenderStageVertex | MTLRenderStageFragment
                                     beforeStages:MTLRenderStageVertex | MTLRenderStageFragment];
    } else if (metalCmd->pComputeEncoder != nil) {
        [metalCmd->pComputeEncoder memoryBarrierWithResources:resources.data() count:barrierCount];
    }

    return RESULT_SUCCESS;
}
Result Metal_CmdUpdateBuffer(Cmd* cmd, const BufferUpdateDesc* desc, ALog* log) {
    if (cmd == nullptr || desc == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pBlitEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer requires a blit encoder");
        return RESULT_INVALID_STATE;
    }

    Metal_Buffer* dstBuffer = BUFFER(desc->pDstBuffer);
    Metal_Buffer* srcBuffer = BUFFER(desc->pSrcBuffer);
    
    if (dstBuffer == nullptr || dstBuffer->pBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer received invalid destination buffer");
        return RESULT_INVALID_ARGUMENT;
    }
    if (!HasFlag(dstBuffer->GetState(), RESOURCE_STATE_COPY_DST)) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer destination buffer must be in COPY_DST state");
        return RESULT_INVALID_STATE;
    }
    if (srcBuffer == nullptr || srcBuffer->pBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer received invalid source buffer");
        return RESULT_INVALID_ARGUMENT;
    }
    if (!HasFlag(srcBuffer->GetState(), RESOURCE_STATE_COPY_SRC)) {
        WriteMetalLog(log, ALog::Level::Error, "UpdateBuffer source buffer must be in COPY_SRC state");
        return RESULT_INVALID_STATE;
    }

    [metalCmd->pBlitEncoder copyFromBuffer:srcBuffer->pBuffer
                                sourceOffset:desc->mSrcOffset
                                   toBuffer:dstBuffer->pBuffer
                              destinationOffset:desc->mDstOffset
                                        size:desc->mSize];

    return RESULT_SUCCESS;
}
Result Metal_CmdDraw(Cmd* cmd, uint32_t vertexCount, uint32_t firstVertex, ALog* log) {
    if (cmd == nullptr || vertexCount == 0) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDraw received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDraw requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDraw requires an active render pass");
        return RESULT_INVALID_STATE;
    }

    [metalCmd->pRenderEncoder drawPrimitives:metalCmd->mPrimitiveType vertexStart:firstVertex vertexCount:vertexCount];
    return RESULT_SUCCESS;
}
Result Metal_CmdDrawInstanced(Cmd* cmd, uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount,
                              uint32_t firstInstance, ALog* log) {
    if (cmd == nullptr || vertexCount == 0 || instanceCount == 0) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawInstanced received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawInstanced requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawInstanced requires an active render pass");
        return RESULT_INVALID_STATE;
    }

    [metalCmd->pRenderEncoder drawPrimitives:metalCmd->mPrimitiveType
                                 vertexStart:firstVertex
                                 vertexCount:vertexCount
                               instanceCount:instanceCount
                                baseInstance:firstInstance];
    return RESULT_SUCCESS;
}
Result Metal_CmdDrawIndexed(Cmd* cmd, uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex, ALog* log) {
    if (cmd == nullptr || indexCount == 0) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexed received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexed requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexed requires an active render pass");
        return RESULT_INVALID_STATE;
    }
    if (metalCmd->pIndexBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexed requires an index buffer");
        return RESULT_INVALID_STATE;
    }

    [metalCmd->pRenderEncoder drawIndexedPrimitives:metalCmd->mPrimitiveType
                                         indexCount:indexCount
                                          indexType:metalCmd->mIndexType
                                        indexBuffer:metalCmd->pIndexBuffer
                                  indexBufferOffset:IndexBufferOffset(metalCmd, firstIndex)
                                         instanceCount:1
                                           baseVertex:firstVertex
                                           baseInstance:0];
    return RESULT_SUCCESS;
}
Result Metal_CmdDrawIndexedInstanced(Cmd* cmd, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount,
                                     uint32_t firstVertex, uint32_t firstInstance, ALog* log) {
    if (cmd == nullptr || indexCount == 0 || instanceCount == 0) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexedInstanced received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexedInstanced requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pRenderEncoder == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexedInstanced requires an active render pass");
        return RESULT_INVALID_STATE;
    }
    if (metalCmd->pIndexBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "CmdDrawIndexedInstanced requires an index buffer");
        return RESULT_INVALID_STATE;
    }

    [metalCmd->pRenderEncoder drawIndexedPrimitives:metalCmd->mPrimitiveType
                                         indexCount:indexCount
                                          indexType:metalCmd->mIndexType
                                        indexBuffer:metalCmd->pIndexBuffer
                                  indexBufferOffset:IndexBufferOffset(metalCmd, firstIndex)
                                      instanceCount:instanceCount
                                         baseVertex:firstVertex
                                       baseInstance:firstInstance];
    return RESULT_SUCCESS;
}
Result Metal_CmdEnd(Cmd* cmd, ALog* log) {
    if (cmd == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "CmdEnd received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetState() != CMD_STATE_RECORDING) {
        WriteMetalLog(log, ALog::Level::Error, "CmdEnd requires a recording command");
        return RESULT_INVALID_STATE;
    }

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pRenderEncoder != nil) {
        [metalCmd->pRenderEncoder endEncoding];
        metalCmd->pRenderEncoder = nil;
    }

    metalCmd->SetBackendState(CMD_STATE_EXECUTABLE);
    return RESULT_SUCCESS;
}

void Metal_ReleaseCmd(Cmd* cmd) {
    if (cmd == nullptr) return;

    Metal_Cmd* metalCmd = CMD(cmd);
    if (metalCmd->pCommandBuffer != nil && metalCmd->GetState() == CMD_STATE_SUBMITTED) {
        [metalCmd->pCommandBuffer waitUntilCompleted];
    }
    ReleaseRecordedObjects(metalCmd);
    delete metalCmd;
}

Result Metal_CreateCmdPool(Queue*, const CmdPoolDesc*, CmdPool** out, ALog* log) {
    if (out) *out = nullptr;
    WriteMetalLog(log, ALog::Level::Warning, "CmdPool is not implemented for Metal v0");
    return RESULT_UNSUPPORTED_API;
}

void Metal_ReleaseCmdPool(CmdPool*) {}

Result Metal_CmdPoolAcquireCmd(CmdPool*, Cmd** out, ALog* log) {
    if (out) *out = nullptr;
    WriteMetalLog(log, ALog::Level::Warning, "CmdPoolAcquireCmd is not implemented for Metal v0");
    return RESULT_UNSUPPORTED_API;
}

#endif
