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

#include "Shader.hpp"
#include "Device.hpp"
#include "MetalUtils.hpp"

#include <fstream>
#include <new>
#include <string>

namespace {
bool IsValidShaderStage(ShaderStage stage) {
    return stage == SHADER_STAGE_VERTEX || stage == SHADER_STAGE_FRAGMENT || stage == SHADER_STAGE_COMPUTE;
}

NSString* MakeNSString(const char* data, size_t size) {
    if (data == nullptr) return nil;
    if (size == 0) return [[NSString alloc] initWithUTF8String:data];
    return [[NSString alloc] initWithBytes:data length:size encoding:NSUTF8StringEncoding];
}
} // namespace

id<MTLFunction> CompileShader(const ShaderDesc* desc, id<MTLDevice> device, ALog* log) {
    std::ifstream file(desc->pSource, std::ios::binary | std::ios::ate);
    if (!file) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to open shader file");
        return nil;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to read shader file");
        return nil;
    }

    id<MTLLibrary> library = nil;
    NSError* error = nil;
    NSString* entry = [[NSString alloc] initWithUTF8String:desc->pEntryPoint];

    if (desc->mSourceType == SHADER_SOURCE_TYPE_SOURCE) {
        NSString* source = [[NSString alloc] initWithBytes:buffer.data() length:size encoding:NSUTF8StringEncoding];
        MTLCompileOptions* options = [[MTLCompileOptions alloc] init];

        library = [device newLibraryWithSource:source options:options error:&error];
        [source release];
        [options release];
    } else if (desc->mSourceType == SHADER_SOURCE_TYPE_BINARY) {
        dispatch_data_t data = dispatch_data_create(buffer.data(), size, dispatch_get_global_queue(0, 0),
                                                    DISPATCH_DATA_DESTRUCTOR_DEFAULT);
        library = [device newLibraryWithData:data error:&error];
        dispatch_release(data);
    }

    id<MTLFunction> function = nil;
    if (error) {
        WriteMetalLog(log, ALog::Level::Error, [[error localizedDescription] UTF8String]);
    } else if (library) {
        function = [library newFunctionWithName:entry];
        if (!function) {
            WriteMetalLog(log, ALog::Level::Error, "Failed to find entry point in library");
        }
    }

    [entry release];
    return function;
}

#endif
