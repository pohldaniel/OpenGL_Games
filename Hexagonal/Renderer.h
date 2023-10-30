#pragma once

#include <array>
#include <engine/Vector.h>


class Map;
class Camera;

class Renderer {
	
public:
	void drawIsometricRect(float posX, float posY, Vector4f sizeOffset, Vector4f color);
	void drawIsometricRect(float posX, float posY, Vector4f bounds, Vector2f offset, Vector4f color);
	void drawIsometricRect(int posX, int posY, Vector4f color);
	void drawIsometricLine(const Vector2f& start, const Vector2f& end, const Vector2f& offset, const Vector4f& color);
	void drawClickBox(float posX, float posY, float width, float height);

	void resize(int deltaW, int deltaH);

	void setCamera(Camera* camera);
	void setMap(Map* map);
	void setZoomfactor(float zoomFactor);

	static Renderer& Get();

private:

	Renderer() = default;

	const Camera* m_camera;
	const Map* m_map;

	float m_zoomFactor;
	float m_focusPointY;
	float m_focusPointX;

	static Renderer s_instance;
};