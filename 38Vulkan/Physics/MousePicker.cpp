#include "MousePicker.h"
#include "Application.h"

std::unique_ptr<Shader> MousePicker::s_shader = nullptr;

MousePicker::MousePicker() : m_callback(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)), m_hasPicked(false), m_isActivated(false){

	if (!s_shader) {
		s_shader = std::unique_ptr<Shader>(new Shader(MOUSEPICKER_VERTEX, MOUSEPICKER_FRGAMENT, false));		
	}
	createBuffer();
}

MousePicker::~MousePicker() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
}

void MousePicker::updatePosition(unsigned int posX, unsigned int posY, const Camera& camera) {
	float mouseXndc = (2.0f * posX) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * posY) / static_cast<float>(Application::Height);

	//Vector4f rayStartEye = Vector4f(mouseXndc, mouseYndc, -1.0f, 1.0f) ^ camera.getInvPerspectiveMatrix();
	//Vector4f rayEndEye = Vector4f(mouseXndc, mouseYndc, 1.0f, 1.0f) ^ camera.getInvPerspectiveMatrix();
	//rayStartEye = rayStartEye * (1.0f / rayStartEye[3]);
	//rayEndEye = rayEndEye * (1.0f / rayEndEye[3]);
	//
	//Vector3f rayStartWorld = rayStartEye * camera.getInvViewMatrix();
	//Vector3f rayEndWorld = rayEndEye * camera.getInvViewMatrix();

	float tanfov = camera.getInvPerspectiveMatrixNew()[1][1];
	float aspect = (static_cast<float>(Application::Width) / static_cast<float>(Application::Height));

	Vector3f rayStartWorld = camera.getPosition() + (camera.getCamX() * mouseXndc * tanfov * aspect + camera.getCamY() * mouseYndc * tanfov + camera.getViewDirection()) * camera.getNear();
	//Vector3f rayStartWorld = camera.getPosition();
	Vector3f rayEndWorld = camera.getPosition() + (camera.getCamX() * mouseXndc * tanfov * aspect + camera.getCamY() * mouseYndc * tanfov + camera.getViewDirection()) * camera.getFar();
	Vector3f rayDirection = rayEndWorld - rayStartWorld;
	Vector3f::Normalize(rayDirection);

	m_callback = MousePickCallback(Physics::VectorFrom(rayStartWorld), Physics::VectorFrom(rayEndWorld), Physics::MOUSEPICKER, Physics::PICKABLE_OBJECT);
	Physics::GetDynamicsWorld()->rayTest(m_callback.m_origin, m_callback.m_target, m_callback);

	if (m_callback.hasHit()) {
		if (!m_hasPicked) {
			Vector3f normal = Vector3f(m_callback.m_hitNormalWorld[0], m_callback.m_hitNormalWorld[1], m_callback.m_hitNormalWorld[2]);
			Vector3f tangent = fabsf(Vector3f::Dot(normal, Vector3f(0.0f, 0.0f, 1.0f))) > 0.9f ? Vector3f::Cross(normal, Vector3f(0.0f, 1.0f, 0.0f)) : Vector3f::Cross(normal, Vector3f(0.0f, 0.0f, 1.0f));
			Vector3f bitangent = Vector3f::Cross(normal, tangent);

			m_model[0][0] = tangent[0];
			m_model[0][1] = tangent[1];
			m_model[0][2] = tangent[2];
			m_model[0][3] = 0.0f;

			m_model[1][0] = normal[0];
			m_model[1][1] = normal[1];
			m_model[1][2] = normal[2];
			m_model[1][3] = 0.0f;

			m_model[2][0] = bitangent[0];
			m_model[2][1] = bitangent[1];
			m_model[2][2] = bitangent[2];
			m_model[2][3] = 0.0f;
		}

		m_model[3][0] = m_callback.m_hitPointWorld[0];
		m_model[3][1] = m_callback.m_hitPointWorld[1];
		m_model[3][2] = m_callback.m_hitPointWorld[2];
		m_model[3][3] = 1.0f;
		m_isActivated = true;
	}else {
		m_isActivated = false;
	}
}

bool MousePicker::click(unsigned int posX, unsigned int posY, const Camera& camera) {
	float mouseXndc = (2.0f * posX) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * posY) / static_cast<float>(Application::Height);

	float tanfov = camera.getInvPerspectiveMatrixNew()[1][1];
	float aspect = (static_cast<float>(Application::Width) / static_cast<float>(Application::Height));

	Vector3f rayStartWorld = camera.getPosition() + (camera.getCamX() * mouseXndc * tanfov * aspect + camera.getCamY() * mouseYndc * tanfov + camera.getViewDirection()) * camera.getNear();
	Vector3f rayEndWorld = camera.getPosition() + (camera.getCamX() * mouseXndc * tanfov * aspect + camera.getCamY() * mouseYndc * tanfov + camera.getViewDirection()) * camera.getFar();

	Vector3f rayDirection = rayEndWorld - rayStartWorld;
	Vector3f::Normalize(rayDirection);

	m_callback = MousePickCallback(Physics::VectorFrom(rayStartWorld), Physics::VectorFrom(rayEndWorld), Physics::MOUSEPICKER, Physics::PICKABLE_OBJECT);
	Physics::GetDynamicsWorld()->rayTest(m_callback.m_origin, m_callback.m_target, m_callback);

	if (m_callback.hasHit()) {
		m_pickingDistance = (m_callback.m_hitPointWorld - m_callback.m_origin).length();	
		return true;
	} else {
		return false;
	}
}

void MousePicker::createBuffer() {
	if (m_debug) {

		std::vector<float> vertex;
		vertex.push_back(-0.5f); vertex.push_back(0.0f); vertex.push_back(-0.5f); vertex.push_back(0.0f); vertex.push_back(0.0f);
		vertex.push_back(-0.5f); vertex.push_back(0.0f); vertex.push_back( 0.5f); vertex.push_back(0.0f); vertex.push_back(1.0f);
		vertex.push_back( 0.5f); vertex.push_back(0.0f); vertex.push_back( 0.5f); vertex.push_back(1.0f); vertex.push_back(1.0f);
		vertex.push_back( 0.5f); vertex.push_back(0.0f); vertex.push_back(-0.5f); vertex.push_back(1.0f); vertex.push_back(0.0f);

		if (!m_vao) {
			const unsigned short indices[] = {
				3, 2, 1, 0
			};

			unsigned int ibo;
			glGenBuffers(1, &ibo);
			glGenBuffers(1, &m_vbo);

			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

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
	}
}

void MousePicker::drawPicker(const Camera& camera) {

	if (!(m_debug && m_vao) || !m_isActivated) return;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	
	s_shader->use();
	s_shader->loadMatrix("u_transform", camera.getPerspectiveMatrix() * camera.getViewMatrix() * m_model);
	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	s_shader->unuse();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

}

const MousePickCallback& MousePicker::getCallback() {
	return m_callback;
}

float MousePicker::getPickingDistance() {
	return m_pickingDistance;
}

void MousePicker::setHasPicked(bool value) {
	m_hasPicked = value;
}

void MousePicker::setPosition(const Vector3f& pos) {
	m_model[3][0] = pos[0];
	m_model[3][1] = pos[1];
	m_model[3][2] = pos[2];
	m_model[3][3] = 1.0f;
}

void MousePicker::setIsActivated(bool isactivated) {
	m_isActivated = isactivated;
}