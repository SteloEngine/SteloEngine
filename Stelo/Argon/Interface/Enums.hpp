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
#include <cstdint>

constexpr bool HasFlag(uint32_t value, uint32_t flag) {
    return (value & flag) == flag;
}

#define X(api, _) GRAPHICS_API_##api,
enum GraphicsApi : uint8_t {
    GRAPHICS_API_UNKNOWN = 0,
    GRAPHICS_API_LIST(X, _) GRAPHICS_API_COUNT,
    GRAPHICS_API_AUTO = 0xFF
};
#undef X

enum Result : uint32_t {
    RESULT_SUCCESS = 0,
    RESULT_INVALID_ARGUMENT = 1,
    RESULT_UNSUPPORTED_API = 2,
    RESULT_OUT_OF_HOST_MEMORY = 3,
    RESULT_OUT_OF_DEVICE_MEMORY = 4,
    RESULT_ALREADY_EXISTS = 5,
    RESULT_NOT_FOUND = 6,
    RESULT_NOT_READY = 7,
    RESULT_INVALID_STATE = 8,
    RESULT_INVALID_ACCESS = 9,
    RESULT_UNKNOWN_ERROR = 10
};

enum ResourceType : uint8_t { RESOURCE_BUFFER, RESOURCE_TEXTURE, RESOURCE_SAMPLER, RESOURCE_COUNT };

using BufferCreateFlags = uint8_t;
static constexpr BufferCreateFlags BUFFER_CREATE_NONE = 0;
static constexpr BufferCreateFlags BUFFER_CREATE_BINDING = 1u << 0;
static constexpr BufferCreateFlags BUFFER_CREATE_RESIDENCY = 1u << 1;
static constexpr BufferCreateFlags BUFFER_CREATE_ALIAS = 1u << 2;

static constexpr BufferCreateFlags BUFFER_CREATE_PROTECTED = 1u << 3;
static constexpr BufferCreateFlags BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY = 1u << 4;
static constexpr BufferCreateFlags BUFFER_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY = 1u << 5;
static constexpr BufferCreateFlags BUFFER_CREATE_VIDEO_PROFILE_INDEPENDENT = 1u << 6;

using BufferUsageFlags = uint32_t;
static constexpr BufferUsageFlags BUFFER_USAGE_NONE = 0;
static constexpr BufferUsageFlags BUFFER_USAGE_TRANSFER_SRC = 1u << 0;
static constexpr BufferUsageFlags BUFFER_USAGE_TRANSFER_DST = 1u << 1;
static constexpr BufferUsageFlags BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = 1u << 2;
static constexpr BufferUsageFlags BUFFER_USAGE_STORAGE_TEXEL_BUFFER = 1u << 3;
static constexpr BufferUsageFlags BUFFER_USAGE_UNIFORM_BUFFER = 1u << 4;
static constexpr BufferUsageFlags BUFFER_USAGE_STORAGE_BUFFER = 1u << 5;
static constexpr BufferUsageFlags BUFFER_USAGE_INDEX_BUFFER = 1u << 6;
static constexpr BufferUsageFlags BUFFER_USAGE_VERTEX_BUFFER = 1u << 7;
static constexpr BufferUsageFlags BUFFER_USAGE_INDIRECT_BUFFER = 1u << 8;
static constexpr BufferUsageFlags BUFFER_USAGE_SHADER_DEVICE_ADDRESS = 1u << 9;
static constexpr BufferUsageFlags BUFFER_USAGE_VIDEO_DECODE_SRC = 1u << 10;
static constexpr BufferUsageFlags BUFFER_USAGE_VIDEO_DECODE_DST = 1u << 11;
static constexpr BufferUsageFlags BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER = 1u << 12;
static constexpr BufferUsageFlags BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER = 1u << 13;
static constexpr BufferUsageFlags BUFFER_USAGE_CONDITIONAL_RENDERING = 1u << 14;
static constexpr BufferUsageFlags BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY = 1u << 15;
static constexpr BufferUsageFlags BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE = 1u << 16;
static constexpr BufferUsageFlags BUFFER_USAGE_SHADER_BINDING_TABLE = 1u << 17;
static constexpr BufferUsageFlags BUFFER_USAGE_VIDEO_ENCODE_DST = 1u << 18;
static constexpr BufferUsageFlags BUFFER_USAGE_VIDEO_ENCODE_SRC = 1u << 19;
static constexpr BufferUsageFlags BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER = 1u << 20;
static constexpr BufferUsageFlags BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER = 1u << 21;
static constexpr BufferUsageFlags BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER = 1u << 22;
static constexpr BufferUsageFlags BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY = 1u << 23;
static constexpr BufferUsageFlags BUFFER_USAGE_MICROMAP_STORAGE = 1u << 24;
static constexpr BufferUsageFlags BUFFER_USAGE_TILE_MEMORY_QCOM = 1u << 25;
static constexpr BufferUsageFlags BUFFER_USAGE_DESCRIPTOR_HEAP = 1u << 28;
static constexpr BufferUsageFlags BUFFER_USAGE_RAY_TRACING_NV = BUFFER_USAGE_SHADER_BINDING_TABLE;

enum SharingMode : uint8_t { SHARING_EXCLUSIVE = 0, SHARING_CONCURRENT = 1, SHARING_COUNT = 2 };

enum MemoryUsage : uint8_t { MEMORY_USAGE_GPU_ONLY = 0, MEMORY_USAGE_CPU_TO_GPU = 1, MEMORY_USAGE_GPU_TO_CPU = 2 };

enum QueueType : uint8_t { QUEUE_TYPE_GRAPHICS = 0, QUEUE_TYPE_COMPUTE = 1, QUEUE_TYPE_TRANSFER = 2 };

enum Format : uint8_t {
    FORMAT_UNDEFINED = 0,
    FORMAT_R8_UNORM,
    FORMAT_RG8_UNORM,
    FORMAT_RGBA8_UNORM,
    FORMAT_BGRA8_UNORM,

    FORMAT_RGBA8_SRGB,
    FORMAT_BGRA8_SRGB,

    FORMAT_RGBA16_FLOAT,
    FORMAT_RGB16_FLOAT,
    FORMAT_RG16_FLOAT,
    FORMAT_R16_FLOAT,
    FORMAT_RGBA32_FLOAT,
    FORMAT_RGB32_FLOAT,
    FORMAT_RG32_FLOAT,
    FORMAT_R32_FLOAT,

    FORMAT_D16_UNORM,
    FORMAT_D32_FLOAT,
    FORMAT_D24_UNORM_S8_UINT
};

enum TextureDimension : uint8_t { TEXTURE_DIMENSION_1D = 0, TEXTURE_DIMENSION_2D = 1, TEXTURE_DIMENSION_3D = 2 };

using TextureUsageFlags = uint8_t;
static constexpr TextureUsageFlags TEXTURE_USAGE_NONE = 0;
static constexpr TextureUsageFlags TEXTURE_USAGE_TRANSFER_SRC = 1u << 0;
static constexpr TextureUsageFlags TEXTURE_USAGE_TRANSFER_DST = 1u << 1;
static constexpr TextureUsageFlags TEXTURE_USAGE_SAMPLED = 1u << 2;
static constexpr TextureUsageFlags TEXTURE_USAGE_STORAGE = 1u << 3;
static constexpr TextureUsageFlags TEXTURE_USAGE_COLOR_ATTACHMENT = 1u << 4;
static constexpr TextureUsageFlags TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1u << 5;
static constexpr TextureUsageFlags TEXTURE_USAGE_PRESENT = 1u << 6;

enum SampleCount : uint8_t { SAMPLE_COUNT_1 = 1, SAMPLE_COUNT_2 = 2, SAMPLE_COUNT_4 = 4, SAMPLE_COUNT_8 = 8 };

using ResourceStateFlags = uint8_t;
static constexpr ResourceStateFlags RESOURCE_STATE_UNDEFINED = 0;
static constexpr ResourceStateFlags RESOURCE_STATE_COPY_SRC = 1u << 0;
static constexpr ResourceStateFlags RESOURCE_STATE_COPY_DST = 1u << 1;
static constexpr ResourceStateFlags RESOURCE_STATE_VERTEX_BUFFER = 1u << 2;
static constexpr ResourceStateFlags RESOURCE_STATE_INDEX_BUFFER = 1u << 3;
static constexpr ResourceStateFlags RESOURCE_STATE_RENDER_TARGET = 1u << 4;
static constexpr ResourceStateFlags RESOURCE_STATE_DEPTH_WRITE = 1u << 5;
static constexpr ResourceStateFlags RESOURCE_STATE_SHADER_RESOURCE = 1u << 6;
static constexpr ResourceStateFlags RESOURCE_STATE_PRESENT = 1u << 7;

enum LoadOp : uint8_t { LOAD_OP_LOAD = 0, LOAD_OP_CLEAR = 1, LOAD_OP_DONT_CARE = 2 };
enum StoreOp : uint8_t { STORE_OP_STORE = 0, STORE_OP_DONT_CARE = 1 };

using ShaderStage = uint8_t;
static constexpr ShaderStage SHADER_STAGE_UNKNOWN = 0;
static constexpr ShaderStage SHADER_STAGE_VERTEX = 1u << 0;
static constexpr ShaderStage SHADER_STAGE_FRAGMENT = 1u << 1;
static constexpr ShaderStage SHADER_STAGE_COMPUTE = 1u << 2;

enum ShaderSourceType : uint8_t {
    SHADER_SOURCE_TYPE_UNKNOWN = 0,
    SHADER_SOURCE_TYPE_BINARY = 1,
    SHADER_SOURCE_TYPE_SOURCE = 2
};

enum PrimitiveTopology : uint8_t {
    PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4
};

enum VertexStepRate : uint8_t { VERTEX_STEP_RATE_VERTEX = 0, VERTEX_STEP_RATE_INSTANCE = 1 };

enum CmdState : uint8_t {
    CMD_STATE_IDLE = 0,
    CMD_STATE_RECORDING = 1,
    CMD_STATE_EXECUTABLE = 2,
    CMD_STATE_SUBMITTED = 3
};

enum PresentMode : uint8_t { PRESENT_MODE_FIFO = 0, PRESENT_MODE_MAILBOX = 1, PRESENT_MODE_IMMEDIATE = 2 };

enum CullMode : uint8_t {
    CULL_MODE_NONE,
    CULL_MODE_FRONT,
    CULL_MODE_BACK,
};

enum FillMode : uint8_t {
    FILL_MODE_WIREFRAME,
    FILL_MODE_SOLID,
};

enum FrontFace : uint8_t {
    FRONT_FACE_CCW,
    FRONT_FACE_CW,
};

enum ShaderSemantic : uint8_t {
    SHADER_SEMANTIC_POSITION,
    SHADER_SEMANTIC_NORMAL,
    SHADER_SEMANTIC_TANGENT,
    SHADER_SEMANTIC_BITANGENT,
    SHADER_SEMANTIC_TEXCOORD0,
    SHADER_SEMANTIC_TEXCOORD1,
    SHADER_SEMANTIC_TEXCOORD2,
    SHADER_SEMANTIC_TEXCOORD3,
    SHADER_SEMANTIC_TEXCOORD4,
    SHADER_SEMANTIC_TEXCOORD5,
    SHADER_SEMANTIC_TEXCOORD6,
    SHADER_SEMANTIC_TEXCOORD7,
    SHADER_SEMANTIC_COLOR0,
    SHADER_SEMANTIC_COLOR1,
    SHADER_SEMANTIC_COLOR2,
    SHADER_SEMANTIC_COLOR3,
    SHADER_SEMANTIC_JOINTS0,
    SHADER_SEMANTIC_WEIGHTS0,
};

enum CompareOp : uint8_t {
    COMPARE_OP_NEVER,
    COMPARE_OP_LESS,
    COMPARE_OP_EQUAL,
    COMPARE_OP_LEQUAL,
    COMPARE_OP_GREATER,
    COMPARE_OP_NOT_EQUAL,
    COMPARE_OP_GEQUAL,
    COMPARE_OP_ALWAYS,
};

enum BlendOp : uint8_t {
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REVERSE_SUBTRACT,
    BLEND_OP_MIN,
    BLEND_OP_MAX,
    BLEND_OP_COUNT
};

enum StencilOp : uint8_t {
    STENCIL_OP_KEEP,
    STENCIL_OP_SET_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCR,
    STENCIL_OP_DECR,
    STENCIL_OP_INCR_SAT,
    STENCIL_OP_DECR_SAT,
};

enum BlendFactor : uint8_t {
    BLEND_FACTOR_ZERO = 0,
    BLEND_FACTOR_ONE,
    BLEND_FACTOR_SRC_COLOR,
    BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    BLEND_FACTOR_DST_COLOR,
    BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    BLEND_FACTOR_SRC_ALPHA,
    BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    BLEND_FACTOR_DST_ALPHA,
    BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    BLEND_FACTOR_SRC_ALPHA_SATURATE,
    BLEND_FACTOR_BLEND_FACTOR,
    BLEND_FACTOR_ONE_MINUS_BLEND_FACTOR,
};

enum BlendMode : uint8_t {
    BLEND_MODE_ADD,
    BLEND_MODE_SUBTRACT,
    BLEND_MODE_REVERSE_SUBTRACT,
    BLEND_MODE_MIN,
    BLEND_MODE_MAX,
};

enum PipelineType : uint8_t {
    PIPELINE_TYPE_GRAPHICS,
    PIPELINE_TYPE_COMPUTE,
};

enum DescriptorType : uint8_t {
    DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    DESCRIPTOR_TYPE_STORAGE_BUFFER,
    DESCRIPTOR_TYPE_TEXTURE,
    DESCRIPTOR_TYPE_SAMPLER
};

enum IndexType : uint8_t {
    INDEX_TYPE_UINT16,
    INDEX_TYPE_UINT32
};