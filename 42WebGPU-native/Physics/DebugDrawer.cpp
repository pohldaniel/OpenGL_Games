#include <WebGPU/WgpContext.h>
#include "DebugDrawer.h"
#include <iostream>

DebugDrawer::DebugDrawer() : m_debugMode(btIDebugDraw::DBG_DrawWireframe), m_wireframeMode(true) {
   
}

void DebugDrawer::init() {
    initPipelines();
    std::fill(std::begin(m_viewProjection), std::end(m_viewProjection), 0.0f);
}

DebugDrawer::~DebugDrawer() {
    shutDown();
}

void DebugDrawer::toggleWireframe() {
    m_wireframeMode = !m_wireframeMode;
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
    m_lineVertices.push_back({ (float)from.getX(), (float)from.getY(), (float)from.getZ(), (float)fromColor.getX(), (float)fromColor.getY(), (float)fromColor.getZ(), 1.0f });
    m_lineVertices.push_back({ (float)to.getX(), (float)to.getY(), (float)to.getZ(), (float)toColor.getX(), (float)toColor.getY(), (float)toColor.getZ(), 1.0f });
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    drawLine(from, to, color, color);
}

void DebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) {  
    if (m_wireframeMode) {
        drawLine(a, b, color);
        drawLine(b, c, color);
        drawLine(c, a, color);
    }else {
        m_triangleVertices.push_back({ (float)a.getX(), (float)a.getY(), (float)a.getZ(), (float)color.getX(), (float)color.getY(), (float)color.getZ(), (float)alpha });
        m_triangleVertices.push_back({ (float)b.getX(), (float)b.getY(), (float)b.getZ(), (float)color.getX(), (float)color.getY(), (float)color.getZ(), (float)alpha });
        m_triangleVertices.push_back({ (float)c.getX(), (float)c.getY(), (float)c.getZ(), (float)color.getX(), (float)color.getY(), (float)color.getZ(), (float)alpha });
    }
}

void DebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color) {
    const float step = 45.0f;
    const float DEG_TO_RAD = SIMD_PI / 180.0f;

    for (float j = 0.0f; j < 180.0f; j += step) {
        for (float i = 0.0f; i < 360.0f; i += step) {
            float rad_i = i * DEG_TO_RAD;
            float rad_j = j * DEG_TO_RAD;
            float rad_i_next = (i + step) * DEG_TO_RAD;
            float rad_j_next = (j + step) * DEG_TO_RAD;

            btVector3 v1 = p + btVector3(radius * std::sinf(rad_i) * std::sinf(rad_j),
                radius * std::cosf(rad_j),
                radius * std::cosf(rad_i) * std::sinf(rad_j));

            btVector3 v2 = p + btVector3(radius * std::sinf(rad_i_next) * std::sinf(rad_j),
                radius * std::cosf(rad_j),
                radius * std::cosf(rad_i_next) * std::sinf(rad_j));

            btVector3 v3 = p + btVector3(radius * std::sinf(rad_i) * std::sinf(rad_j_next),
                radius * std::cosf(rad_j_next),
                radius * std::cosf(rad_i) * std::sinf(rad_j_next));

            btVector3 v4 = p + btVector3(radius * std::sinf(rad_i_next) * std::sinf(rad_j_next),
                radius * std::cosf(rad_j_next),
                radius * std::cosf(rad_i_next) * std::sinf(rad_j_next));

            drawTriangle(v1, v2, v3, color, 1.0f);
            drawTriangle(v3, v2, v4, color, 1.0f);
        }
    }
}

void DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
    btVector3 to = pointOnB + normalOnB * 10.0f;
    drawLine(pointOnB, to, color);
}

void DebugDrawer::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

    wgpuQueueWriteBuffer(wgpContext.queue, m_unifromBuffer, 0u, m_viewProjection, 64u);

    WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
    renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;
    WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
    WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);

    if (!m_lineVertices.empty()) {
        /*if (m_lineVertices.size() > m_maxLineVertices) {
            m_maxLineVertices = m_lineVertices.size() * 2; // Verdoppeln für Puffer
            wgpuBufferDestroy(m_lineBuffer);
            wgpuBufferRelease(m_lineBuffer);

            WGPUBufferDescriptor desc = {};
            desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
            desc.size = m_maxLineVertices * sizeof(DebugVertex);
            m_lineBuffer = wgpuDeviceCreateBuffer(m_device, &desc);
        }*/

        uint64_t writeSize = m_lineVertices.size() * sizeof(DebugVertex);
        wgpuQueueWriteBuffer(wgpContext.queue, m_lineBuffer, 0, m_lineVertices.data(), writeSize);

        wgpuRenderPassEncoderSetPipeline(renderPassEncoder, m_linePipeline);
        wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, m_bindGroup, 0, NULL);
        wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, m_lineBuffer, 0, writeSize);
        wgpuRenderPassEncoderDraw(renderPassEncoder, (uint32_t)m_lineVertices.size(), 1, 0, 0);

    }

    if (!m_triangleVertices.empty()) {
        /*if (m_triangleVertices.size() > m_maxTriangleVertices) {
            m_maxTriangleVertices = m_triangleVertices.size() * 2;
            wgpuBufferDestroy(m_triangleBuffer);
            wgpuBufferRelease(m_triangleBuffer);

            WGPUBufferDescriptor desc = {};
            desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
            desc.size = m_maxTriangleVertices * sizeof(DebugVertex);
            m_triangleBuffer = wgpuDeviceCreateBuffer(m_device, &desc);
        }*/

        uint64_t writeSize = m_triangleVertices.size() * sizeof(DebugVertex);
        wgpuQueueWriteBuffer(wgpContext.queue, m_triangleBuffer, 0, m_triangleVertices.data(), writeSize);

        wgpuRenderPassEncoderSetPipeline(renderPassEncoder, m_trianglePipeline);
        wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, m_bindGroup, 0, NULL);
        wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, m_triangleBuffer, 0, writeSize);
        wgpuRenderPassEncoderDraw(renderPassEncoder, (uint32_t)m_triangleVertices.size(), 1, 0, 0);
    }
    wgpuRenderPassEncoderEnd(renderPassEncoder);
    wgpuRenderPassEncoderRelease(renderPassEncoder);

    m_lineVertices.clear();
    m_triangleVertices.clear();
}

void DebugDrawer::setDebugMode(int debugMode) {

}

int DebugDrawer::getDebugMode() const { 
    return m_debugMode; 
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString) {

}

void DebugDrawer::reportErrorWarning(const char* warningString) {

}

void DebugDrawer::shutDown() {
    m_lineVertices.clear();
    m_triangleVertices.clear();

    if (m_lineBuffer) {
        wgpuBufferDestroy(m_lineBuffer);
        wgpuBufferRelease(m_lineBuffer);
        m_lineBuffer = NULL;
    }

    if (m_triangleBuffer) {
        wgpuBufferDestroy(m_triangleBuffer);
        wgpuBufferRelease(m_triangleBuffer);
        m_triangleBuffer = NULL;
    }

    if (m_unifromBuffer) {
        wgpuBufferDestroy(m_unifromBuffer);
        wgpuBufferRelease(m_unifromBuffer);
        m_unifromBuffer = NULL;
    }

    if (m_bindGroup) {
        wgpuBindGroupRelease(m_bindGroup);
        m_bindGroup = NULL;
    }

    if (m_linePipeline) {
        wgpuRenderPipelineRelease(m_linePipeline);
        m_linePipeline = NULL;
    }

    if (m_trianglePipeline) {
        wgpuRenderPipelineRelease(m_trianglePipeline);
        m_trianglePipeline = NULL;
    }
}

void DebugDrawer::initPipelines() {

    WGPUBufferDescriptor lineDescriptor = {};
    lineDescriptor.label = WGPU_STR("DebugDrawer Line Vertex Buffer");
    lineDescriptor.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    lineDescriptor.size = m_maxLineVertices * sizeof(DebugVertex);
    lineDescriptor.mappedAtCreation = false;
    m_lineBuffer = wgpuDeviceCreateBuffer(wgpContext.device, &lineDescriptor);

    WGPUBufferDescriptor triangleDesc = {};
    triangleDesc.label = WGPU_STR("DebugDrawer Triangle Vertex Buffer");
    triangleDesc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    triangleDesc.size = m_maxTriangleVertices * sizeof(DebugVertex);
    triangleDesc.mappedAtCreation = false;
    m_triangleBuffer = wgpuDeviceCreateBuffer(wgpContext.device, &triangleDesc);

    std::vector<WGPUVertexAttribute> vertexAttribute = std::vector<WGPUVertexAttribute>(2);

    vertexAttribute[0].shaderLocation = 0u;
    vertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
    vertexAttribute[0].offset = 0u;

    vertexAttribute[1].shaderLocation = 1u;
    vertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x4;
    vertexAttribute[1].offset = 3 * sizeof(float);

    WGPUVertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributeCount = (uint32_t)vertexAttribute.size();
    vertexBufferLayout.attributes = vertexAttribute.data();
    vertexBufferLayout.arrayStride = 28u;
    vertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
    
    WGPUShaderModule shaderModule = wgpCreateShaderFromFile("res/shader/physics_debug.wgsl");

    WGPUBindGroupLayoutEntry bindGroupLayoutEntry = {};
    bindGroupLayoutEntry.binding = 0;
    bindGroupLayoutEntry.visibility = WGPUShaderStage_Vertex;
    bindGroupLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
    bindGroupLayoutDescriptor.entryCount = 1;
    bindGroupLayoutDescriptor.entries = &bindGroupLayoutEntry;
    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

    WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
    pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
    pipelineLayoutDescriptor.bindGroupLayouts = &bindGroupLayout;
    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

    WGPUVertexState vertexState = {};
    vertexState.module = shaderModule;
    vertexState.entryPoint = WGPU_STR("vs_main");
    vertexState.bufferCount = 1u;
    vertexState.buffers = &vertexBufferLayout;

    WGPUBlendState blendState = {};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;
    blendState.alpha.srcFactor = WGPUBlendFactor_One;
    blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
    blendState.alpha.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTargetState = {};
    colorTargetState.format = wgpContext.colorFormat;
    colorTargetState.blend = &blendState;
    colorTargetState.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState = {};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = WGPU_STR("fs_main");
    fragmentState.targetCount = 1u;
    fragmentState.targets = &colorTargetState;

    WGPUDepthStencilState depthStencilState = {};
    depthStencilState.nextInChain = NULL;
    depthStencilState.format = wgpContext.depthFormat;
    depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
    depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

    depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
    depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
    depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
    depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
    depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
    depthStencilState.stencilReadMask = 0u;
    depthStencilState.stencilWriteMask = 0u;
    depthStencilState.depthBias = 0;
    depthStencilState.depthBiasSlopeScale = 0.0f;
    depthStencilState.depthBiasClamp = 0.0f;

    WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
    renderPipelineDescriptor.layout = pipelineLayout;
    renderPipelineDescriptor.vertex = vertexState;
    renderPipelineDescriptor.fragment = &fragmentState;
    renderPipelineDescriptor.depthStencil = &depthStencilState;
    renderPipelineDescriptor.multisample.count = 1u;
    renderPipelineDescriptor.multisample.mask = ~0u;
    renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
    renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

    renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_LineList;  
    m_linePipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);

    renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    m_trianglePipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);

    WGPUBufferDescriptor bufferDesc = {};
    bufferDesc.label = WGPU_STR("uniform_buf");
    bufferDesc.size = 16u * sizeof(float);;
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    bufferDesc.mappedAtCreation = false;

    m_unifromBuffer = wgpuDeviceCreateBuffer(wgpContext.device, &bufferDesc);

    std::vector<WGPUBindGroupEntry> bindGroupEntries(1);
    bindGroupEntries[0].binding = 0u;
    bindGroupEntries[0].buffer = m_unifromBuffer;
    bindGroupEntries[0].offset = 0u;
    bindGroupEntries[0].size = 16u * sizeof(float);

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.layout = bindGroupLayout;
    bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
    bindGroupDesc.entries = bindGroupEntries.data();

    m_bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

    wgpuBindGroupLayoutRelease(bindGroupLayout);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuShaderModuleRelease(shaderModule);
}

float(&DebugDrawer::getViewProjection())[16]{
    return m_viewProjection;
}