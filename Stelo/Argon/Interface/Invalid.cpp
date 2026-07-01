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

#include "DeclareTypes.hpp"
#include "Enums.hpp"
#include "ALog.hpp"
#include <string_view>

namespace {
void WriteInvalid(ALog* log, ALog::Level level, std::string_view message) {
    if (log) log->Write(level, "Argon.Invalid", message);
}
} // namespace

Result Invalid_CreateDevice(const DeviceDesc*, Device** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateDevice called for an unsupported graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateQueue(Device*, const QueueDesc*, Queue** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateQueue is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateBuffer(Device*, const BufferDesc*, Buffer** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateBuffer is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateTexture(Device*, const TextureDesc*, Texture** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateTexture is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreatePipeline(Device*, const PipelineDesc*, Pipeline** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreatePipeline is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateSwapChain(Device*, const SwapChainDesc*, SwapChain** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateSwapChain is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateCmd(Queue*, const CmdDesc*, Cmd** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateCmd is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateCmdPool(Queue*, const CmdPoolDesc*, CmdPool** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateCmdPool is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreatePipelineLayout(Device*, const PipelineLayoutDesc*, PipelineLayout** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreatePipelineLayout is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateDescriptorSetLayout(Device*, const DescriptorSetLayoutDesc*, DescriptorSetLayout** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateDescriptorSetLayout is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CreateDescriptorSet(Device*, DescriptorSetLayout*, DescriptorSet** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CreateDescriptorSet is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_AllocateDescriptorSet(Device*, DescriptorSetLayout*, DescriptorSet** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "AllocateDescriptorSet is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_MapBuffer(Device*, MapBufferDesc*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "MapBuffer is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_UnmapBuffer(Device*, MapBufferDesc*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "UnmapBuffer is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

void Invalid_UpdateDescriptorSet(DescriptorSet*, const DescriptorUpdateInfo*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "UpdateDescriptorSet is not supported by this graphics API");
}

void Invalid_ReleaseDevice(Device*) {}
void Invalid_ReleaseQueue(Queue*) {}
void Invalid_ReleaseBuffer(Buffer*) {}
void Invalid_ReleaseTexture(Texture*) {}
void Invalid_ReleasePipeline(Pipeline*) {}
void Invalid_ReleaseSwapChain(SwapChain*) {}
void Invalid_ReleaseCmd(Cmd*) {}
void Invalid_ReleaseCmdPool(CmdPool*) {}
void Invalid_ReleaseDescriptorSetLayout(DescriptorSetLayout* layout) {}
void Invalid_ReleaseDescriptorSet(DescriptorSet* set) {}

Result Invalid_CmdBegin(Cmd*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdBegin is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdActiveRenderPass(Cmd*, RenderPass*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdActiveRenderPass is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdActiveComputePass(Cmd*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdActiveComputePass is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdActiveTransferPass(Cmd*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdActiveTransferPass is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdBindPipeline(Cmd*, Pipeline*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdBindPipeline is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdBindIndexBuffer(Cmd*, Buffer*, IndexType, uint64_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdBindIndexBuffer is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdDraw(Cmd*, uint32_t, uint32_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdDraw is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdDrawInstanced(Cmd*, uint32_t, uint32_t, uint32_t, uint32_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdDrawInstanced is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdDrawIndexed(Cmd*, uint32_t, uint32_t, uint32_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdDrawIndexed is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdDrawIndexedInstanced(Cmd*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdDrawIndexedInstanced is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdBindVertexBuffers(Cmd*, uint32_t, Buffer**, const uint32_t*, const uint64_t*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdBindVertexBuffers is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdBindDescriptorSets(Cmd*, PipelineLayout*, uint32_t, uint32_t, const DescriptorSet**, const uint32_t*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdBindDescriptorSets is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdBindResource(Cmd*, uint32_t, const BindResourceDesc*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdBindResource is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdEnd(Cmd*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdEnd is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdPoolAcquireCmd(CmdPool*, Cmd** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "CmdPoolAcquireCmd is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_QueueSubmitCmd(Queue*, Cmd*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "QueueSubmitCmd is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_QueueSubmitCmds(Queue*, Cmd* const*, uint32_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "QueueSubmitCmds is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_QueueSubmitCmdPool(Queue*, CmdPool*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "QueueSubmitCmdPool is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_SwapChainAcquireTexture(SwapChain*, Texture** out, ALog* log) {
    if (out) *out = nullptr;
    WriteInvalid(log, ALog::Level::Error, "SwapChainAcquireTexture is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_SwapChainResize(SwapChain*, uint32_t, uint32_t, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "SwapChainResize is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_QueuePresent(Queue*, SwapChain*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "QueuePresent is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdSetVertexBuffer(Cmd*, uint32_t, Buffer**, const uint32_t*, const uint64_t*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdSetVertexBuffer is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdResourceBarrier(Cmd*, uint32_t, const BufferBarrier*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdResourceBarrier is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}

Result Invalid_CmdUpdateBuffer(Cmd*, const BufferUpdateDesc*, ALog* log) {
    WriteInvalid(log, ALog::Level::Error, "CmdUpdateBuffer is not supported by this graphics API");
    return RESULT_UNSUPPORTED_API;
}
