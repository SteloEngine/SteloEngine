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

#include "ALog.hpp"
#include "Config.hpp"
#include "DeclareTypes.hpp"
#include "Enums.hpp"

DECLARE_GRAPHICS_API(Result, CreateDevice(const DeviceDesc*, Device**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateQueue(Device*, const QueueDesc*, Queue**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateBuffer(Device*, const BufferDesc*, Buffer**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateTexture(Device*, const TextureDesc*, Texture**, ALog*))
DECLARE_GRAPHICS_API(Result, CreatePipeline(Device*, const PipelineDesc*, Pipeline**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateSwapChain(Device*, const SwapChainDesc*, SwapChain**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateDescriptorSetLayout(Device*, const DescriptorSetLayoutDesc*, DescriptorSetLayout**, ALog*))
DECLARE_GRAPHICS_API(Result, CreatePipelineLayout(Device*, const PipelineLayoutDesc*, PipelineLayout**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateHeap(Device*, const HeapDesc*, Heap**, ALog*))

DECLARE_GRAPHICS_API(Result, CreateCmd(Queue*, const CmdDesc*, Cmd**, ALog*))
DECLARE_GRAPHICS_API(Result, CreateCmdPool(Queue*, const CmdPoolDesc*, CmdPool**, ALog*))

DECLARE_GRAPHICS_API(Result, AllocateDescriptorSet(Device*, DescriptorSetLayout*, DescriptorSet**, ALog*))
DECLARE_GRAPHICS_API(Result, MapBuffer(Device*, MapBufferDesc*, ALog*))
DECLARE_GRAPHICS_API(Result, UnmapBuffer(Device*, MapBufferDesc*, ALog*))

DECLARE_GRAPHICS_API(Result, CmdBegin(Cmd*, ALog*))

DECLARE_GRAPHICS_API(Result, CmdActiveRenderPass(Cmd*, RenderPass*, ALog*))
DECLARE_GRAPHICS_API(Result, CmdActiveComputePass(Cmd*, ALog*))
DECLARE_GRAPHICS_API(Result, CmdActiveTransferPass(Cmd*, ALog*))

DECLARE_GRAPHICS_API(Result, CmdSetViewport(Cmd*, float, float, float, float, float, float, ALog*))
DECLARE_GRAPHICS_API(Result, CmdSetScissor(Cmd*, uint32_t, uint32_t, uint32_t, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdSetStencilRef(Cmd*, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdBindPipeline(Cmd*, Pipeline*, ALog*))
DECLARE_GRAPHICS_API(Result, CmdBindIndexBuffer(Cmd*, Buffer*, IndexType, uint64_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdBindVertexBuffers(Cmd*, uint32_t, Buffer**, const uint32_t*, const uint64_t*, ALog*))
DECLARE_GRAPHICS_API(Result, CmdBindDescriptorSets(Cmd*, PipelineLayout*, uint32_t, uint32_t, const DescriptorSet**, const uint32_t*, ALog*))
DECLARE_GRAPHICS_API(Result, CmdBindResource(Cmd*, uint32_t, const BindResourceDesc*, ALog*))

DECLARE_GRAPHICS_API(Result, CmdResourceBarrier(Cmd*, uint32_t, const BufferBarrier*, ALog*))
DECLARE_GRAPHICS_API(Result, CmdUpdateBuffer(Cmd*, const BufferUpdateDesc*, ALog*))

DECLARE_GRAPHICS_API(Result, CmdDraw(Cmd*, uint32_t, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdDrawInstanced(Cmd*, uint32_t, uint32_t, uint32_t, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdDrawIndexed(Cmd*, uint32_t, uint32_t, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdDrawIndexedInstanced(Cmd*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdDispatch(Cmd*, uint32_t, uint32_t, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, CmdEnd(Cmd*, ALog*))

DECLARE_GRAPHICS_API(Result, CmdPoolAcquireCmd(CmdPool*, Cmd**, ALog*))

DECLARE_GRAPHICS_API(Result, QueueSubmitCmd(Queue*, Cmd*, ALog*))
DECLARE_GRAPHICS_API(Result, QueueSubmitCmds(Queue*, Cmd* const*, uint32_t, ALog*))
DECLARE_GRAPHICS_API(Result, QueueSubmitCmdPool(Queue*, CmdPool*, ALog*))
DECLARE_GRAPHICS_API(Result, QueuePresent(Queue*, SwapChain*, ALog*))

DECLARE_GRAPHICS_API(Result, SwapChainAcquireTexture(SwapChain*, Texture**, ALog*))
DECLARE_GRAPHICS_API(Result, SwapChainResize(SwapChain*, uint32_t, uint32_t, ALog*))

DECLARE_GRAPHICS_API(void, DescriptorSetUpdate(DescriptorSet*, uint32_t, const DescriptorUpdateInfo*))

DECLARE_GRAPHICS_API(Result, HeapCreateBuffer(Heap*, const BufferDesc*, Buffer**, ALog*))
DECLARE_GRAPHICS_API(Result, HeapCreateTexture(Heap*, const TextureDesc*, Texture**, ALog*))

DECLARE_GRAPHICS_API(void, ReleaseDevice(Device*))
DECLARE_GRAPHICS_API(void, ReleaseQueue(Queue*))
DECLARE_GRAPHICS_API(void, ReleaseBuffer(Buffer*))
DECLARE_GRAPHICS_API(void, ReleaseTexture(Texture*))
DECLARE_GRAPHICS_API(void, ReleasePipeline(Pipeline*))
DECLARE_GRAPHICS_API(void, ReleaseSwapChain(SwapChain*))
DECLARE_GRAPHICS_API(void, ReleaseCmd(Cmd*))
DECLARE_GRAPHICS_API(void, ReleaseCmdPool(CmdPool*))
DECLARE_GRAPHICS_API(void, ReleaseDescriptorSetLayout(DescriptorSetLayout*))
DECLARE_GRAPHICS_API(void, ReleasePipelineLayout(PipelineLayout*))
DECLARE_GRAPHICS_API(void, ReleaseDescriptorSet(DescriptorSet*))
DECLARE_GRAPHICS_API(void, ReleaseHeap(Heap*))
 