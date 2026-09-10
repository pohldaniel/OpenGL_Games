#pragma once
#include <vector>
#include <webgpu.h>

#include <LinearMath/btIDebugDraw.h>

struct DebugVertex {
    float x, y, z;
    float r, g, b, a;
};

class DebugDrawer : public btIDebugDraw {
    uint64_t m_maxLineVertices = 100000u;
    uint64_t m_maxTriangleVertices = 100000u;

public:

    DebugDrawer();
    virtual ~DebugDrawer();
   

    void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
    void shutDown();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    void drawSphere(const btVector3& p, btScalar radius, const btVector3& color) override;
    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) override;
    void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const override;
    void draw3dText(const btVector3& location, const char* textString) override;
    void reportErrorWarning(const char* warningString) override;

    void init();  
    float(&getViewProjection())[16];
    void toggleWireframe();

private:

    void initPipelines();

    int m_debugMode;
    bool m_wireframeMode;

    std::vector<DebugVertex> m_lineVertices;
    std::vector<DebugVertex> m_triangleVertices;

    WGPURenderPipeline m_linePipeline = NULL;
    WGPURenderPipeline m_trianglePipeline = NULL;
    WGPUBindGroup m_bindGroup = NULL;
    WGPUBuffer m_lineBuffer = NULL;
    WGPUBuffer m_triangleBuffer = NULL;
    WGPUBuffer m_unifromBuffer = NULL;
    float m_viewProjection[16];
};