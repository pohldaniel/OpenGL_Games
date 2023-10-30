#include <GL/glew.h>
#include <engine/Camera.h>

#include "Renderer.h"
#include "Math.hpp"
#include "Map.h"
#include "Application.h"

Renderer Renderer::s_instance;

Renderer& Renderer::Get() {
	return s_instance;
}

void Renderer::drawIsometricRect(float posX, float posY, Vector4f sizeOffset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;
	fPoints[0] = Vector2f(sizeOffset[2], sizeOffset[3]);
	fPoints[1] = Vector2f(sizeOffset[2], sizeOffset[3] + sizeOffset[1]);
	fPoints[2] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3] + sizeOffset[1]);
	fPoints[3] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera->getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&m_camera->getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	posX = posX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor);
	posY = posY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor);

	glVertex3f(posX + fPoints[0][0] * m_zoomFactor, posY - fPoints[0][1] * m_zoomFactor, 0.0f);
	glVertex3f(posX + fPoints[1][0] * m_zoomFactor, posY - fPoints[1][1] * m_zoomFactor, 0.0f);
	glVertex3f(posX + fPoints[2][0] * m_zoomFactor, posY - fPoints[2][1] * m_zoomFactor, 0.0f);
	glVertex3f(posX + fPoints[3][0] * m_zoomFactor, posY - fPoints[3][1] * m_zoomFactor, 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::drawIsometricRect(float posX, float posY, Vector4f bounds, Vector2f offset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;

	fPoints[0] = Vector2f(bounds[0] + offset[0], bounds[1] + offset[1]);
	fPoints[1] = Vector2f(bounds[0] + offset[0], bounds[3] + offset[1]);
	fPoints[2] = Vector2f(bounds[2] + offset[0], bounds[3] + offset[1]);
	fPoints[3] = Vector2f(bounds[2] + offset[0], bounds[1] + offset[1]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera->getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&m_camera->getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	posX = posX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor);
	posY = posY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor);

	glVertex3f(posX + fPoints[0][0] * m_zoomFactor, posY - fPoints[0][1] * m_zoomFactor, 0.0f);
	glVertex3f(posX + fPoints[1][0] * m_zoomFactor, posY - fPoints[1][1] * m_zoomFactor, 0.0f);
	glVertex3f(posX + fPoints[2][0] * m_zoomFactor, posY - fPoints[2][1] * m_zoomFactor, 0.0f);
	glVertex3f(posX + fPoints[3][0] * m_zoomFactor, posY - fPoints[3][1] * m_zoomFactor, 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::drawIsometricRect(int posX, int posY, Vector4f color) {
	float rowMin = posX - 0.5f;
	float rowMax = posX + 0.5f;
	float colMin = posY - 1.5f;
	float colMax = posY - 0.5f;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera->getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&m_camera->getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = rowMax;
	float cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, m_map->getCellWidth(), m_map->getCellHeight());
	glVertex3f(cartX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, m_map->getCellWidth(), m_map->getCellHeight());
	glVertex3f(cartX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, m_map->getCellWidth(), m_map->getCellHeight());
	glVertex3f(cartX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

	cartX = rowMax;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, m_map->getCellWidth(), m_map->getCellHeight());
	glVertex3f(cartX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::drawIsometricLine(const Vector2f& start, const Vector2f& end, const Vector2f& offset, const Vector4f& color) {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera->getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&m_camera->getViewMatrix()[0][0]);

	glBegin(GL_LINES);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = start[0] + (offset[0] - offset[1]);
	float cartY = -start[1] + 0.5f * (offset[0] + offset[1]);
	glVertex3f(cartX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);


	cartX = end[0] + (offset[0] - offset[1]);
	cartY = -end[1] + 0.5f * (offset[0] + offset[1]);
	glVertex3f(cartX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);
	glEnd();
}

void Renderer::drawClickBox(float posX, float posY, float width, float height) {

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera->getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&m_camera->getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);

	posX = posX * m_zoomFactor + (m_camera->getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor);
	posY = posY * m_zoomFactor + (m_camera->getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(posX, posY, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(posX, (posY + height * m_zoomFactor), 0.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(posX + width * m_zoomFactor, (posY + height * m_zoomFactor), 0.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(posX + width * m_zoomFactor, posY, 0.0f);
	glEnd();

}

void Renderer::setCamera(Camera* camera) {
	m_camera = camera;
}

void Renderer::setMap(Map* map) {
	m_map = map;
}

void Renderer::setZoomfactor(float zoomFactor) {
	m_zoomFactor = zoomFactor;
}

void Renderer::resize(int deltaW, int deltaH) {
	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);
}