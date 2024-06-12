#include "Background.h"

unsigned int BackgroundLayer::Vao = 0;
unsigned int BackgroundLayer::Vbo = 0;
std::unique_ptr<Shader> Background::s_shader = nullptr;

BackgroundLayer::BackgroundLayer(Texture* texture, unsigned int zoomX, float speed, float offset, Vector2f textureOffsetY) {
	init(texture, zoomX, speed, offset, textureOffsetY);
}

BackgroundLayer::BackgroundLayer(BackgroundLayer const& rhs) {
    m_texture = rhs.m_texture;
    m_zoomX = rhs.m_zoomX;
    m_speed = rhs.m_speed;
    m_offset = rhs.m_offset;
    m_textureRects = rhs.m_textureRects;
}

BackgroundLayer& BackgroundLayer::operator=(const BackgroundLayer& rhs) {
	m_texture = rhs.m_texture;
	m_zoomX = rhs.m_zoomX;
	m_speed = rhs.m_speed;
	m_offset = rhs.m_offset;
	m_textureRects = rhs.m_textureRects;
	return *this;
}

void BackgroundLayer::init(Texture* texture, unsigned int zoomX, float speed, float offset, Vector2f textureOffsetY) {
	m_zoomX = zoomX;
	m_texture = texture;
	m_offset = offset;
	m_speed = speed;

	for (unsigned int i = 0; i < m_zoomX; i++) {
		m_textureRects.push_back(Vector4f((static_cast<float>(i) / zoomX), textureOffsetY[0], (static_cast<float>(i+1) / zoomX), textureOffsetY[1]));
	}
	m_textureRects.push_back(Vector4f((0.0f / zoomX), textureOffsetY[0], (1.0f / zoomX), textureOffsetY[1]));
}

float BackgroundLayer::clampOffset(float input){
	if (input < 0){
		float mod = -input;
		mod = fmod(mod, 1.0f);
		mod = -mod + 1.0f;
		return mod;
	}
	return fmod(input, 1.0f);
}

void BackgroundLayer::draw(Shader* shader, unsigned int width) {
	
	m_offset = clampOffset(m_offset);	
	float start = width * 2.0f * m_zoomX * m_offset;
	
	m_texture->bind(0);

	for (unsigned int i = 0; i < m_zoomX + 1; i++) {
		shader->loadMatrix("u_transform", Matrix4f::Translate(i * 2.0f - (start / width), 0.0f, 0.0f));
		//shader->loadVector("u_color", Vector4f(static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), 1.0f));
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		shader->loadVector("u_texRect", m_textureRects[i]);
		glBindVertexArray(Vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}	
}

void BackgroundLayer::addOffset(float offset) {	
	m_offset = m_offset + offset * m_speed;
}

void BackgroundLayer::Init() {
	float data[] = {
		-1.0, -1.0, 0.0f, 0.0f, 0.0f,
		-1.0,  1.0, 0.0f, 0.0f, 1.0f,
		1.0,  1.0, 0.0f, 1.0f, 1.0f,
		1.0,-1.0, 0.0f, 1.0f, 0.0f
	};

	const GLushort index[] = {
		0, 2, 1,
		0, 3, 2
	};

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &Vbo);

	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////

Background::Background() : m_speed(1.0f), m_width(0u), m_height(0u){
	if (!s_shader) {
		s_shader = std::unique_ptr<Shader>(new Shader(BACKGROUND_VERTEX, BACKGROUND_FRGAMENT, false));
		s_shader->use();
		s_shader->loadInt("u_texture", 0);
		s_shader->unuse();

		BackgroundLayer::Init();
	}
}

Background::Background(Background const& rhs) {
	m_layer = rhs.m_layer;	
}

Background& Background::operator=(const Background& rhs) {
	m_layer = rhs.m_layer;
	return *this;
}

void Background::addLayer(BackgroundLayer layer) {
	m_layer.push_back(layer);
}

void Background::setLayer(std::vector<BackgroundLayer>& layer) {
	m_layer = layer;
}

void Background::update(float dt) {
	addOffset(dt * m_speed);
}

void Background::draw() {
	glEnable(GL_BLEND);

	s_shader->use();
	for (auto&& layer : m_layer){
		layer.draw(s_shader.get(), m_width);
	}
	s_shader->unuse();
	//glDisable(GL_BLEND);
}

void Background::addOffset(float offset) {
	for (auto&& layer : m_layer) {
		layer.addOffset(offset);
	}
}

void Background::setSpeed(float speed) {
	m_speed = speed;
}

void Background::resize(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;
}