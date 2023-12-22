
#include <vector>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "DebugDrawService.h"
#include "Constants.h"
#include "Maths.h"
#include "Globals.h"

DebugDrawService::DebugDrawService(glm::mat4 viewMat, glm::mat4 projMat)
:	m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag"),
	m_shaderLaser("res/shaders/basic/basic.vert", "res/shaders/basic/laser.frag"),
	m_shaderCircleWithGlow("res/shaders/basic/basic.vert", "res/shaders/basic/circle-with-glow.frag"),
	m_shaderCircleExplosion("res/shaders/basic/basic.vert", "res/shaders/basic/circle-explosion.frag"),
	m_vbMaxSize(32),
	m_viewMat(viewMat),
	m_projMat(projMat),
	m_color(255., 255., 255., 1.)
{

	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,  2 * sizeof(float), (const void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

DebugDrawService::~DebugDrawService() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}
}

/* ------------------------------- BOX2D API ------------------------------- */

void DebugDrawService::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    assert(vertexCount < m_vbMaxSize * 2);
    
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Update
    std::vector<float> array(vertexCount * 2);
    unsigned int dataIndex = 0;
    for (int i = 0; i < vertexCount; i++) {
        array[dataIndex] = vertices[i].x;
        array[dataIndex + 1] = vertices[i].y;
        dataIndex += 2;
	}
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW);
    
    // Render
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", color.r, color.g, color.b, color.a);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
    
    // Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();
}

void DebugDrawService::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    assert(vertexCount < m_vbMaxSize * 2);
    
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Update
    std::vector<float> array(vertexCount * 2);
    unsigned int dataIndex = 0;
    for (int i = 0; i < vertexCount; i++) {
        array[dataIndex] = vertices[i].x;
        array[dataIndex + 1] = vertices[i].y;
        dataIndex += 2;
	}
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW);
    
    // Render outline
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", color.r, color.g, color.b, color.a );
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
    
    // Render inside TODO use a tesselation shader to fill the inside completely no matter the shape
    // polygon triangulation
	m_shaderBasic.loadVector("u_color", color.r * 0.5f, color.g * 0.5f, color.b * 0.5f, color.a * 0.5f);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    
    // Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();
}

void DebugDrawService::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Update buffer
    unsigned int segmentNumber = 17; // TODO define segment number based on radius
    //const float TWO_PI = (float) PI * 2;
    std::vector<float> array;
    for (unsigned int i = 0; i <= segmentNumber; i++) {
		array.push_back(radius * cos(i * TWO_PI / segmentNumber) + center.x);
		array.push_back(radius * sin(i * TWO_PI / segmentNumber) + center.y);
	}
    glBufferData(GL_ARRAY_BUFFER, array.size() * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW);
    
    // Update
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", color.r, color.g, color.b, color.a);
    glDrawArrays(GL_LINE_STRIP, 0, segmentNumber + 1);

    // Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();
}

void DebugDrawService::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Update buffer
    unsigned int segmentNumber = 17; // TODO define segment number based on radius
    //const float TWO_PI = (float) PI * 2;
    std::vector<float> array;
	array.push_back(center.x);
	array.push_back(center.y);
    for (unsigned int i = 0; i <= segmentNumber; i++) {
		array.push_back(radius * cos(i * TWO_PI / segmentNumber) + center.x);
		array.push_back(radius * sin(i * TWO_PI / segmentNumber) + center.y);
	}
    glBufferData(GL_ARRAY_BUFFER, array.size() * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW);
    
    // Update
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", color.r * 0.5f, color.g * 0.5f, color.b * 0.5f, color.a * 0.5f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segmentNumber + 1);

    // Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();

    // Draw a line fixed in the circle to animate rotation.
	b2Vec2 endpoint = center + radius * axis;
    DrawSegment(center, endpoint, color);
}

void DebugDrawService::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Update
    float data[] = {
        p1.x, p1.y,
        p2.x, p2.y
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &data, GL_DYNAMIC_DRAW);
    
    // Render
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", color.r, color.g, color.b, color.a);
    glDrawArrays(GL_LINES, 0, 2);
    
    // Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();
}

void DebugDrawService::DrawTransform(const b2Transform& xf) {
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Update Y axis
    float axisScale = 5.0f;
    b2Vec2 origin = xf.p;
    b2Vec2 endPoint = origin + axisScale * xf.q.GetYAxis();
    float yAxis[] = {
        origin.x,   origin.y,
        endPoint.x, endPoint.y
    };
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &yAxis, GL_DYNAMIC_DRAW);
    
    // Render Y axis
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", 0.0f, 1.0f, 0.0f, 1.0f); // Y axis in Green
   glDrawArrays(GL_LINES, 0, 4);

    // Update X axis
    endPoint = origin + axisScale * xf.q.GetXAxis();
    float xAxis[] = {
        origin.x,   origin.y,
        endPoint.x, endPoint.y
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), &xAxis);

    // Render X axis
	m_shaderBasic.loadVector("u_color", 1.0f, 0.0f, 0.0f, 1.0f); // X axis in Red
    glDrawArrays(GL_LINES, 0, 2);
    
    // Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();
}

void DebugDrawService::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
    // Binding
    m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glPointSize(size);

    // Update buffer
    float data[] = {p.x,  p.y};
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &data, GL_DYNAMIC_DRAW);
    
    // Update
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
    m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", color.r, color.g, color.b, color.a );
    glDrawArrays(GL_POINTS, 0, 1);

    // Unbinding
    glPointSize(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    m_shaderBasic.unuse();
}

/* ---------------------------- PROCESSING-LIKE API --------------------------- */

void DebugDrawService::triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	// Binding
	m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Update
	float data[] = {
		 x1, y1,
		 x2, y2,
		 x3, y3
	};
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &data, GL_DYNAMIC_DRAW);

	// Render
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
	m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", m_color.r / 255.0f, m_color.g / 255.0f, m_color.b / 255.0f, m_color.a);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shaderBasic.unuse();
}

void DebugDrawService::rect(float x1, float y1, float x2, float y2, int zIndex) {
	// Binding
	m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Update
	float data[] = {
		 x1, y1,	// Bottom left
		 x1, y2,	// Top left
		 x2, y1,	// Bottom right --- First triangle done
		 x2, y2,	// Top right
		 x1, y2,	// Top left
		 x2, y1,	// Bottom right --- Second triangle done
		 
	};
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &data, GL_DYNAMIC_DRAW);

	// Render
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, zIndex));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
	m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", m_color.r / 255.0f, m_color.g / 255.0f, m_color.b / 255.0f, m_color.a);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shaderBasic.unuse();
}

void DebugDrawService::square(float x, float y, float extent) {

}

void DebugDrawService::ellipse(float a, float b, float c, float d) {

}

void DebugDrawService::circleExplosion(float x, float y, float r, float maxR) {
	// Binding
	m_shaderCircleExplosion.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Update buffer
	unsigned int segmentNumber = imaths::max(floor(r),35);
	std::vector<float> array;
	array.push_back(x);
	array.push_back(y);
	for (unsigned int i = 0; i <= segmentNumber; i++) {
		array.push_back(x + r * cos(i * imaths::TAU / segmentNumber));
		array.push_back(y + r * sin(i * imaths::TAU / segmentNumber));
	}
	glBufferData(GL_ARRAY_BUFFER, array.size() * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW);

	// Update
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
	m_shaderCircleExplosion.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderCircleExplosion.loadVector("u_pos", x / 100 / WIN_RATIO * WIN_WIDTH, y / 100 * WIN_HEIGHT);
	m_shaderCircleExplosion.loadFloat("u_radius", r / 100 / WIN_RATIO * WIN_WIDTH);
	m_shaderCircleExplosion.loadFloat("u_maxR", maxR / 100 / WIN_RATIO * WIN_WIDTH);
	glDrawArrays(GL_TRIANGLE_FAN, 0, segmentNumber + 2);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shaderCircleExplosion.unuse();
}

void DebugDrawService::circleWithGlow(float x, float y, float r) {
	// Binding
	m_shaderCircleWithGlow.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Update buffer
	unsigned int segmentNumber = 17; // TODO define segment number based on radius
	std::vector<float> array;
	array.push_back(x);
	array.push_back(y);
	for (unsigned int i = 0; i <= segmentNumber; i++) {
		array.push_back(x + r * cos(i * imaths::TAU / segmentNumber));
		array.push_back(y + r * sin(i * imaths::TAU / segmentNumber));
	}
	glBufferData(GL_ARRAY_BUFFER, array.size() * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW);

	// Update
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
	m_shaderCircleWithGlow.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderCircleWithGlow.loadVector("u_color", m_color.r/255, m_color.g/255, m_color.b/255, m_color.a);
	m_shaderCircleWithGlow.loadVector("u_pos", x/100/WIN_RATIO*WIN_WIDTH, y/100*WIN_HEIGHT);
	m_shaderCircleWithGlow.loadFloat("u_radius", r/100/WIN_RATIO*WIN_WIDTH);
	m_shaderCircleWithGlow.loadFloat("u_waveR", imaths::rangeMapping(Globals::clock.getElapsedTimeSinceRestartMilli()%500,0,500,0,r / 100 / WIN_RATIO * WIN_WIDTH));
	glDrawArrays(GL_TRIANGLE_FAN, 0, segmentNumber + 2);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shaderCircleWithGlow.unuse();
}

void DebugDrawService::quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

}

void DebugDrawService::line(float x1, float y1, float x2, float y2, BasicShaderType shaderType) {
	Shader& shader = getShader(shaderType);
	// Binding
	shader.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Update
	float data[] = {
		x1, y1,
		x2, y2
	};
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &data, GL_DYNAMIC_DRAW);

	// Render
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
	shader.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	shader.loadVector("u_color", m_color.r/255.0f, m_color.g / 255.0f, m_color.b / 255.0f, m_color.a);
	if (shaderType == BasicShaderType::LASER) {
		float halfWidth = LASER_WIDTH/2;
		float dist = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
		shader.loadVector("normal", (y1-y2)/ dist, (x2-x1)/ dist);
		shader.loadVector("startPos", x1/100/WIN_RATIO* WIN_WIDTH,y1/100*WIN_HEIGHT);
		shader.loadFloat("halfWidth", halfWidth);
	}
	glDrawArrays(GL_LINES, 0, 2);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	shader.unuse();
}

void DebugDrawService::point(float x, float y) {
	// Binding
	m_shaderBasic.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Update buffer
	float data[] = { x * WIN_RATIO,  y };
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &data, GL_DYNAMIC_DRAW);

	// Update
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, Z_INDEX_DEBUG_DRAW));
	glm::mat4 mvp = m_projMat * m_viewMat * model;
	m_shaderBasic.loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
	m_shaderBasic.loadVector("u_color", m_color.r / 255.0f, m_color.g / 255.0f, m_color.b / 255.0f, m_color.a);
	glDrawArrays(GL_POINTS, 0, 1);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shaderBasic.unuse();
}

void DebugDrawService::shape(glm::vec2* vertices) {
	// TODO
}

// ------------------------------ SETTERS ---------------------------
void DebugDrawService::setProjMat(glm::mat4 mat) { m_projMat = mat; }
void DebugDrawService::setViewMat(glm::mat4 mat) { m_viewMat = mat; }
void DebugDrawService::setColor(glm::vec4 color) { m_color = color; }
void DebugDrawService::setColor(float r, float g, float b, float a) { m_color = glm::vec4(r, g, b, a); }

//Shaders

Shader& DebugDrawService::getShader(BasicShaderType shaderType) {
	switch (shaderType) {
	default: case BasicShaderType::BASIC:
		return m_shaderBasic;
		break;
	case BasicShaderType::LASER:
		return m_shaderLaser;
		break;
	}
}