#pragma once
#include <list>
#include <vector>
#include <memory>
#include <functional>

#include <webgpu.h>

#include "../scene/Node.h"
#include "../Vector.h"
#include "../Object.h"

enum class UiPipelineType {
	Standard,
	MaskWrite,
	OutlineRead
};

struct UiInstance {
	Matrix4f transform;
	Vector4f color;
	float textureRect[4];
	float textureLayer;
	float flipAndTile[3];
};

struct UiBatch {
	UiPipelineType pipelineType;
	uint32_t startIndex;     // Wo im großen StorageBuffer fängt dieser Batch an?
	uint32_t instanceCount;  // Wie viele Elemente nutzen diese Pipeline am Stück?
};

class Widget : public Node, public Object2D {

public:

	Widget();
	Widget(const Widget& rhs);
	Widget(Widget&& rhs) noexcept;
	virtual ~Widget();

	virtual void draw();

	void setScale(float sx, float sy) override;
	void setScale(const Vector2f& scale) override;
	void setScale(float s) override;

	void setScaleAbsolute(float sx, float sy);
	void setScaleAbsolute(const Vector2f& scale);
	void setScaleAbsolute(float s);

	void setPosition(float x, float y) override;
	void setPosition(const Vector2f& position) override;

	void setOrigin(float x, float y) override;
	void setOrigin(const Vector2f& origin);

	void setOrientation(float degrees) override;

	void translate(const Vector2f& trans) override;
	void translate(float dx, float dy) override;

	void translateRelative(const Vector2f& trans) override;
	void translateRelative(float dx, float dy) override;

	void scale(const Vector2f& scale) override;
	void scale(float sx, float sy) override;
	void scale(float s) override;

	void rotate(float degrees) override;

	const Matrix4f& getWorldTransformation() const;
	const Vector2f& getWorldPosition(bool update = true) const;
	const Vector2f& getWorldScale(bool update = true) const;
	const float getWorldOrientation(bool update = true) const;
	void updateWorldTransformation() const;

	void setDrawFunction(std::function<void()> fun);

	static void Init(float width, float height);
	static void Draw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	static void Resize(float width, float height);

	static std::vector<UiInstance> Instances;
	static std::vector<UiBatch> Batches;

protected:

	void OnTransformChanged();
	void drawTree();
	std::function<void()> m_draw;

private:

	virtual void drawDefault() = 0;

	mutable Matrix4f m_modelMatrix;
	mutable bool m_isDirty;

	static void CreateRenderPipeline(WGPURenderPipeline& renderPipeline);
	static void CreateRenderPipelineMask(WGPURenderPipeline& renderPipeline);
	static void CreateRenderPipelineRead(WGPURenderPipeline& renderPipeline);

	static Vector2f WorldPosition;
	static Vector2f WorldScale;
	static float WorldOrientation;
};

