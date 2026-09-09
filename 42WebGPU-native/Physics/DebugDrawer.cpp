#include <WebGPU/WgpContext.h>
#include "DebugDrawer.h"

DebugDrawer::DebugDrawer() : m_debugMode(0) {
   
}

void DebugDrawer::init() {
    WGPUBufferDescriptor lineDesc = {};
    lineDesc.label = WGPU_STR("DebugDrawer Line Vertex Buffer");
    lineDesc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    lineDesc.size = m_maxLineVertices * sizeof(DebugVertex);
    lineDesc.mappedAtCreation = false;
    m_lineBuffer = wgpuDeviceCreateBuffer(wgpContext.device, &lineDesc);

    WGPUBufferDescriptor triDesc = {};
    triDesc.label = WGPU_STR("DebugDrawer Triangle Vertex Buffer");
    triDesc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    triDesc.size = m_maxTriangleVertices * sizeof(DebugVertex);
    triDesc.mappedAtCreation = false;
    m_triangleBuffer = wgpuDeviceCreateBuffer(wgpContext.device, &triDesc);
    initPipelines();
}

DebugDrawer::~DebugDrawer() {
    clear();
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
    m_lineVertices.push_back({ (float)from.getX(), (float)from.getY(), (float)from.getZ(), (float)fromColor.getX(), (float)fromColor.getY(), (float)fromColor.getZ(), 1.0f });
    m_lineVertices.push_back({ (float)to.getX(), (float)to.getY(), (float)to.getZ(), (float)toColor.getX(), (float)toColor.getY(), (float)toColor.getZ(), 1.0f });
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    drawLine(from, to, color, color);
}

void DebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) {
    m_triangleVertices.push_back({ (float)a.getX(), (float)a.getY(), (float)a.getZ(), (float)color.getX(), (float)color.getY(), (float)color.getZ(), (float)alpha });
    m_triangleVertices.push_back({ (float)b.getX(), (float)b.getY(), (float)b.getZ(), (float)color.getX(), (float)color.getY(), (float)color.getZ(), (float)alpha });
    m_triangleVertices.push_back({ (float)c.getX(), (float)c.getY(), (float)c.getZ(), (float)color.getX(), (float)color.getY(), (float)color.getZ(), (float)alpha });
}

void DebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color) {
    int lats = 5;
    int longs = 5;

    for (int i = 0; i <= lats; i++) {
        btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar)(i - 1) / lats);
        btScalar z0 = radius * sinf(lat0);
        btScalar zr0 = radius * cosf(lat0);

        btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar)i / lats);
        btScalar z1 = radius * sinf(lat1);
        btScalar zr1 = radius * cosf(lat1);

        for (int j = 0; j <= longs; j++) {
            btScalar lng0 = 2 * SIMD_PI * (btScalar)(j - 1) / longs;
            btScalar x0 = cosf(lng0);
            btScalar y0 = sinf(lng0);

            btScalar lng1 = 2 * SIMD_PI * (btScalar)j / longs;
            btScalar x1 = cosf(lng1);
            btScalar y1 = sinf(lng1);

            btVector3 v1 = p + btVector3(x0 * zr0, y0 * zr0, z0);
            btVector3 v2 = p + btVector3(x0 * zr1, y0 * zr1, z1);
            btVector3 v3 = p + btVector3(x1 * zr0, y1 * zr0, z0);
            btVector3 v4 = p + btVector3(x1 * zr1, y1 * zr1, z1);

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

void DebugDrawer::clear() {
    m_lineVertices.clear();
    m_triangleVertices.clear();

    if (m_lineBuffer) {
        wgpuBufferDestroy(m_lineBuffer);
        wgpuBufferRelease(m_lineBuffer);
    }
    if (m_triangleBuffer) {
        wgpuBufferDestroy(m_triangleBuffer);
        wgpuBufferRelease(m_triangleBuffer);
    }
}

void DebugDrawer::initPipelines() {
 /*
    WGPUShaderModuleWGSLDescriptor wgslDesc = {};
    wgslDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    wgslDesc.code = shaderCode;

    WGPUShaderModuleDescriptor shaderDesc = {};
    shaderDesc.nextInChain = &wgslDesc.chain;
    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    WGPUBindGroupLayoutEntry bglEntry = {};
    bglEntry.binding = 0;
    bglEntry.visibility = WGPUShaderStage_Vertex;
    bglEntry.buffer.type = WGPUBufferBindingType_Uniform;

    WGPUBindGroupLayoutDescriptor bglDesc = {};
    bglDesc.entryCount = 1;
    bglDesc.entries = &bglEntry;
    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bglDesc);

    WGPUPipelineLayoutDescriptor layoutDesc = {};
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &bindGroupLayout;
    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &layoutDesc);

    WGPUVertexState vertexState = {};
    vertexState.module = shaderModule;
    vertexState.entryPoint = "vs_main";
    vertexState.bufferCount = 1;
    vertexState.buffers = &vertexBufferLayout;

    WGPUBlendState blendState = {};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;
    blendState.alpha.srcFactor = WGPUBlendFactor_One;
    blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
    blendState.alpha.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget = {};
    colorTarget.format = renderTargetFormat;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState = {};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    WGPUDepthStencilState depthStencil = {};
    depthStencil.format = WGPUTextureFormat_Depth24Plus;
    depthStencil.depthWriteEnabled = true;
    depthStencil.depthCompare = WGPUCompareFunction_Less;

    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.layout = pipelineLayout;
    pipelineDesc.vertex = vertexState;
    pipelineDesc.fragment = &fragmentState;
    pipelineDesc.depthStencil = &depthStencil;
    pipelineDesc.multisample.count = 1;


    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_LineList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    m_linePipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);


    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;
    m_trianglePipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

    wgpuBindGroupLayoutRelease(bindGroupLayout);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuShaderModuleRelease(shaderModule);
 */
}