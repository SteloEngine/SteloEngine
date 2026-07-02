#include "../Stelo/Stelo.hpp"

#include <chrono>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <thread>

#ifndef STELO_DESIGN_PHYSICS_SHADER_PATH
#define STELO_DESIGN_PHYSICS_SHADER_PATH "Shaders/DesignPhysicsTriangle.metal"
#endif

using namespace Stelo;

namespace {
constexpr float kRenderTimeStep = 1.0f / 60.0f;
const char* ResultName(Result result) {
    switch (result) {
        case RESULT_SUCCESS: return "RESULT_SUCCESS";
        case RESULT_INVALID_ARGUMENT: return "RESULT_INVALID_ARGUMENT";
        case RESULT_UNSUPPORTED_API: return "RESULT_UNSUPPORTED_API";
        case RESULT_OUT_OF_HOST_MEMORY: return "RESULT_OUT_OF_HOST_MEMORY";
        case RESULT_OUT_OF_DEVICE_MEMORY: return "RESULT_OUT_OF_DEVICE_MEMORY";
        case RESULT_ALREADY_EXISTS: return "RESULT_ALREADY_EXISTS";
        case RESULT_NOT_FOUND: return "RESULT_NOT_FOUND";
        case RESULT_NOT_READY: return "RESULT_NOT_READY";
        case RESULT_INVALID_STATE: return "RESULT_INVALID_STATE";
        case RESULT_INVALID_ACCESS: return "RESULT_INVALID_ACCESS";
        case RESULT_UNKNOWN_ERROR: return "RESULT_UNKNOWN_ERROR";
    }
    return "RESULT_UNKNOWN";
}
bool Check(Result result, const char* operation, ALog& log) {
    if (result == RESULT_SUCCESS) {
        if (log.Size() != 0) {
            std::cout << log.ToString();
            log.Reset();
        }
        return true;
    }

    std::cerr << operation << " failed: " << ResultName(result) << '\n';
    if (log.Size() != 0) {
        std::cerr << log.ToString();
        log.Reset();
    }
    return false;
}
}

struct Vertex {
    Vector4 pos;  // x, y, z, w
    Vector4 color;

};

Vertex squareVertices[] = {
    { {-0.16f,  0.16f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
    { { 0.16f,  0.16f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
    { { 0.16f, -0.16f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
};

uint32_t squareIndices[] = {
    0, 1, 2,
};

struct Test : public Component {
    inline void Update(float dt) {
        squareVertices[GetID() % 3].pos.x += 0.01f;
    }
};
template<>
struct CompStorageConfig<Test> : public CompStorageConfig<void> {
    static constexpr uint32_t InitialPackCount = 1;
    static constexpr uint8_t PackSizeShift = 24;
};
REGISTER_COMPONENT(Test)

uint32_t vertexStrides = sizeof(Vertex);
uint64_t vertexOffsets = 0;

Pipeline* CreateObjectPipeline(Device* device, ALog& log) {
    ShaderDesc vertexShader{
        .pSource = STELO_DESIGN_PHYSICS_SHADER_PATH,
        .pEntryPoint = "design_physics_vertex",
        .mStage = SHADER_STAGE_VERTEX,
        .mSourceType = SHADER_SOURCE_TYPE_SOURCE
    };

    ShaderDesc fragmentShader{
        .pSource = STELO_DESIGN_PHYSICS_SHADER_PATH,
        .pEntryPoint = "design_physics_fragment",
        .mStage = SHADER_STAGE_FRAGMENT,
        .mSourceType = SHADER_SOURCE_TYPE_SOURCE
    };

    InputAttribute vertexAttributes[2] = {
        {.mSemanticName = "POSITION", .mLocation = 0, .mOffset = 0, .mFormat = FORMAT_RGBA32_FLOAT, .mSemantic = SHADER_SEMANTIC_POSITION},
        {.mSemanticName = "COLOR", .mLocation = 1, .mOffset = sizeof(float) * 4, .mFormat = FORMAT_RGBA32_FLOAT, .mSemantic = SHADER_SEMANTIC_COLOR0}
    };
    InputElement inputElements[1] = {
        {
            .pAttribute = vertexAttributes,
            .numAttribute = 2,
            .mBinding = 0,
            .mStride = sizeof(Vertex),
            .mRate = VERTEX_STEP_RATE_VERTEX
        }
    };

    RasterizerStateDesc rasterizerStateDesc{.mCullMode = CULL_MODE_NONE, .mFillMode = FILL_MODE_SOLID, .mFrontFace = FRONT_FACE_CW};

    Pipeline* pipeline = nullptr;
    PipelineDesc pipelineDesc;
    pipelineDesc.mVertexShader = &vertexShader;
    pipelineDesc.mFragmentShader = &fragmentShader;
    pipelineDesc.mColorFormat = FORMAT_BGRA8_UNORM;
    pipelineDesc.mTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineDesc.mRasterizerState = rasterizerStateDesc;
    pipelineDesc.pInputElements = inputElements;
    pipelineDesc.mInputElementCount = 1;
    pipelineDesc.pDebugName = "DesignPhysicsObjectPipeline";

    Check(device->CreatePipeline(&pipelineDesc, &pipeline, &log), "CreatePipeline", log);
    return pipeline;
}

Buffer* CreateCPUBufferData(Device* device, void* data, BufferDesc desc, ALog& log, const char* name) {
    Buffer* buffer = nullptr;
    desc.mMemoryUsage = MEMORY_USAGE_CPU_TO_GPU;
    Check(device->CreateBuffer(&desc, &buffer, &log), name, log);
    MapBufferDesc mapDesc{.pBuffer = buffer, .mOffset = 0, .mSize = desc.mSize};
    device->MapBuffer(&mapDesc, &log);
    std::memcpy(mapDesc.pMapped, data, desc.mSize);
    device->UnmapBuffer(&mapDesc, &log);
    return buffer;
}

void UpdateCPUBufferData(Device* device, void* data, Buffer* buffer, ALog& log) {
    MapBufferDesc mapDesc{.pBuffer = buffer, .mOffset = 0, .mSize = buffer->GetSize()};
    device->MapBuffer(&mapDesc, &log);
    std::memcpy(mapDesc.pMapped, data, buffer->GetSize());
    device->UnmapBuffer(&mapDesc, &log);
}

int main() {
    ALog log;
    Window* window = new Window("DesignPhysics Triangle", {80.0f, 80.0f}, {800.0f, 600.0f});

    GraphicManager::Initialize(window);

    Pipeline* pipeline = CreateObjectPipeline(GraphicManager::GetDevice(), log);

    BufferDesc vboDesc{
        .mSize = sizeof(Vertex) * 3,
        .mUsage = BUFFER_USAGE_VERTEX_BUFFER | BUFFER_USAGE_TRANSFER_DST,
        .mFlags = BUFFER_CREATE_BINDING,
        .mSharingMode = SHARING_EXCLUSIVE,
    };
    Buffer* VBO = CreateCPUBufferData(GraphicManager::GetDevice(), squareVertices, vboDesc, log, "CreateVBO");

    BufferDesc iboDesc {
        .mSize = sizeof(uint32_t) * 3,
        .mUsage = BUFFER_USAGE_INDEX_BUFFER | BUFFER_USAGE_TRANSFER_DST,
        .mFlags = BUFFER_CREATE_BINDING,
        .mSharingMode = SHARING_EXCLUSIVE,
    };
    Buffer* IBO = CreateCPUBufferData(GraphicManager::GetDevice(), squareIndices, iboDesc, log, "CreateIBO");

    if (!pipeline || !VBO || !IBO) return 1;

    ColorAttachment colorAttachment;
    colorAttachment.pTexture = nullptr;
    colorAttachment.mLoadOp = LOAD_OP_CLEAR;
    colorAttachment.mStoreOp = STORE_OP_STORE;
    colorAttachment.mClearColor = {0.1f, 0.2f, 0.3f, 1.0f};

    RenderPass renderInfo{
        .pColorAttachments = &colorAttachment, .mColorAttachmentCount = 1, .mWidth = GraphicManager::GetSwapChain()->GetWidth(), .mHeight = GraphicManager::GetSwapChain()->GetHeight()
    };

    GameObject* go = new GameObject();
    go->AddComponent<Transform>();
    go->_components.reserve(1<<24);
    for (int i = 0; i < (1<<24); i++) {
       go->AddComponent<Test>();
    }

    GameObject* go2 = new GameObject();
    go2->AddComponent<Transform>();
    go2->AddComponent<Camera>();

    auto lastTime = std::chrono::high_resolution_clock::now();

    float renderAccumulator = 0.0f, fpsTimer = 0.0f;
    int frameCount = 0;
    try {
        float accStartMs = 0.0f;
        float accMoveMs = 0.0f;
        float accPreUpdateMs = 0.0f;
        float accUpdateMs = 0.0f;
        float accPostUpdateMs = 0.0f;
        float accRenderMs = 0.0f;
        float accPollEventMs = 0.0f;
        float accTotalLoopMs = 0.0f;

        using MsDuration = std::chrono::duration<float, std::milli>;
        auto lastTime = std::chrono::high_resolution_clock::now();

        while (App::IsRunning()) {
            auto loopStart = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> deltaTime = currentTime - lastTime;
            lastTime = currentTime;
            Time::logicTime = deltaTime.count();

            renderAccumulator += Time::logicTime;
            fpsTimer += Time::logicTime;
            frameCount++;

            auto recordStart = std::chrono::high_resolution_clock::now();
            CompManager::Start();
            accStartMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - recordStart).count();

            recordStart = std::chrono::high_resolution_clock::now();
            CompManager::ExecuteMoveAction();
            accMoveMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - recordStart).count();

            recordStart = std::chrono::high_resolution_clock::now();
            CompManager::PreUpdate();
            accPreUpdateMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - recordStart).count();

            recordStart = std::chrono::high_resolution_clock::now();
            CompManager::Update();
            accUpdateMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - recordStart).count();

            recordStart = std::chrono::high_resolution_clock::now();
            CompManager::PostUpdate();
            accPostUpdateMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - recordStart).count();

            if (renderAccumulator > kRenderTimeStep) {
                auto renderStart = std::chrono::high_resolution_clock::now();

                Texture* frameTexture = nullptr;
                Result acquireResult = GraphicManager::GetSwapChain()->AcquireTexture(&frameTexture, &log);
                if (acquireResult == RESULT_NOT_READY) { 
                    log.Reset(); 
                    accTotalLoopMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - loopStart).count();
                    continue; 
                }
                if (!Check(acquireResult, "SwapChainAcquireTexture", log)) break;

                colorAttachment.pTexture = frameTexture;

                GraphicManager::Cmd()->Begin();
                GraphicManager::Cmd()->ActiveRenderPass(&renderInfo);
                GraphicManager::Cmd()->BindPipeline(pipeline);

                CompManager::PreRender();
                CompManager::Render();
                CompManager::PostRender();
                
                GraphicManager::Cmd()->BindVertexBuffers(1, &VBO, &vertexStrides, &vertexOffsets);
                GraphicManager::Cmd()->BindIndexBuffer(IBO, INDEX_TYPE_UINT32, 0);
                GraphicManager::Cmd()->DrawIndexed(3, 0, 0);

                GraphicManager::Cmd()->End();
                
                GraphicManager::Queue()->Submit(GraphicManager::Cmd());
                GraphicManager::Queue()->Present(GraphicManager::GetSwapChain());

                renderAccumulator -= kRenderTimeStep;

                accRenderMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - renderStart).count();
            }

            recordStart = std::chrono::high_resolution_clock::now();
            App::PollEvent(PollType::Sandbox);
            accPollEventMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - recordStart).count();

            accTotalLoopMs += std::chrono::duration_cast<MsDuration>(std::chrono::high_resolution_clock::now() - loopStart).count();

            if (fpsTimer >= 1.0f) {
                float div = frameCount > 0 ? static_cast<float>(frameCount) : 1.0f;

                printf("[Avg per Frame] Start: %.3f ms, Move: %.3f ms, PreUpdate: %.3f ms, Update: %.3f ms, PostUpdate: %.3f ms, Render: %.3f ms, PollEvent: %.3f ms | TOTAL_LOOP: %.3f ms\n", 
                    accStartMs / div, 
                    accMoveMs / div, 
                    accPreUpdateMs / div, 
                    accUpdateMs / div, 
                    accPostUpdateMs / div,
                    accRenderMs / div,
                    accPollEventMs / div,
                    accTotalLoopMs / div);
                    
                printf("FPS: %d\n", frameCount);

                frameCount = 0;
                fpsTimer -= 1.0f;
                
                accStartMs = 0.0f;
                accMoveMs = 0.0f;
                accPreUpdateMs = 0.0f;
                accUpdateMs = 0.0f;
                accPostUpdateMs = 0.0f;
                accRenderMs = 0.0f;
                accPollEventMs = 0.0f;
                accTotalLoopMs = 0.0f;
            }
        }
    }
    catch (const std::exception& e) {
        printf("Exception: %s\n", e.what());
    }

    VBO->Release();
    IBO->Release();
    pipeline->Release();
    GraphicManager::Shutdown();

    return 0;
}
