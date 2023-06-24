#include "MousePicker.h"
#include "Application.h"

std::unique_ptr<Shader> MousePicker::s_shader = nullptr;

MousePicker::MousePicker() : m_callback(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)){
	
	if (!s_shader) {
		s_shader = std::unique_ptr<Shader>(new Shader(MOUSEPICKER_VERTEX, MOUSEPICKER_FRGAMENT, false));
	}
}

void MousePicker::updatePosition(unsigned int posX, unsigned int posY, const Camera& camera) {
	float mouseXndc = (2.0f * posX) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * posY) / static_cast<float>(Application::Height);

	Vector4f rayStartEye = Vector4f(mouseXndc, mouseYndc, -1.0f, 1.0f) ^ camera.getInvPerspectiveMatrix();
	Vector4f rayEndEye = Vector4f(mouseXndc, mouseYndc, 1.0f, 1.0f) ^ camera.getInvPerspectiveMatrix();
	rayStartEye = rayStartEye * (1.0f / rayStartEye[3]);
	rayEndEye = rayEndEye * (1.0f / rayEndEye[3]);

	Vector3f rayStartWorld = rayStartEye * camera.getInvViewMatrix();
	Vector3f rayEndWorld = rayEndEye * camera.getInvViewMatrix();

	Vector3f rayDirection = rayEndWorld - rayStartWorld;
	Vector3f::Normalize(rayDirection);

	m_callback = MousePickCallback(Physics::VectorFrom(rayStartWorld), Physics::VectorFrom(rayEndWorld), Physics::MOUSEPICKER, Physics::PICKABLE_OBJECT);
	Globals::physics->GetDynamicsWorld()->rayTest(m_callback.m_origin, m_callback.m_target, m_callback);

	if (m_callback.hasHit()) {
		updateBuffer(Physics::VectorFrom(m_callback.m_hitPointWorld));
	}
}

bool MousePicker::click(unsigned int posX, unsigned int posY, const Camera& camera) {
	float mouseXndc = (2.0f * posX) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * posY) / static_cast<float>(Application::Height);

	Vector4f rayStartEye = Vector4f(mouseXndc, mouseYndc, -1.0f, 1.0f) ^ camera.getInvPerspectiveMatrix();
	Vector4f rayEndEye = Vector4f(mouseXndc, mouseYndc, 1.0f, 1.0f) ^ camera.getInvPerspectiveMatrix();
	rayStartEye = rayStartEye * (1.0f / rayStartEye[3]);
	rayEndEye = rayEndEye * (1.0f / rayEndEye[3]);

	Vector3f campos = camera.getPosition();

	Vector3f rayStartWorld = rayStartEye * camera.getInvViewMatrix();
	Vector3f rayEndWorld = rayEndEye * camera.getInvViewMatrix();

	Vector3f rayDirection = rayEndWorld - rayStartWorld;
	Vector3f::Normalize(rayDirection);

	m_callback = MousePickCallback(Physics::VectorFrom(rayStartWorld), Physics::VectorFrom(rayEndWorld), Physics::MOUSEPICKER, Physics::PICKABLE_OBJECT);
	Globals::physics->GetDynamicsWorld()->rayTest(m_callback.m_origin, m_callback.m_target, m_callback);

	return m_callback.hasHit();
}

void MousePicker::updateBuffer(const Vector3f& pos) {
	if (m_debug) {

		std::vector<float> vertex;
		vertex.push_back(pos[0] - 0.5f); vertex.push_back(pos[1]); vertex.push_back(pos[2] - 0.5f); vertex.push_back(0.0f); vertex.push_back(0.0f);
		vertex.push_back(pos[0] - 0.5f); vertex.push_back(pos[1]); vertex.push_back(pos[2] + 0.5f); vertex.push_back(0.0f); vertex.push_back(1.0f);
		vertex.push_back(pos[0] + 0.5f); vertex.push_back(pos[1]); vertex.push_back(pos[2] + 0.5f); vertex.push_back(1.0f); vertex.push_back(1.0f);
		vertex.push_back(pos[0] + 0.5f); vertex.push_back(pos[1]); vertex.push_back(pos[2] - 0.5f); vertex.push_back(1.0f); vertex.push_back(0.0f);

		if (!m_vao) {
			const unsigned short indices[] = {
				0, 1, 2, 3
			};

			unsigned int ibo;
			glGenBuffers(1, &ibo);
			glGenBuffers(1, &m_vbo);

			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_DYNAMIC_DRAW);

			//Position
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

			//Texture Coordinates
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

			//indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glBindVertexArray(0);
			glDeleteBuffers(1, &ibo);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertex.size() * sizeof(float), &vertex[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void MousePicker::drawPicker(const Camera& camera) {

	if (!(m_debug && m_vao)) return;
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	
	glUseProgram(s_shader->m_program);
	s_shader->loadMatrix("u_transform", camera.getPerspectiveMatrix() * camera.getViewMatrix());
	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

}

const MousePickCallback& MousePicker::getCallback() {
	return m_callback;
}