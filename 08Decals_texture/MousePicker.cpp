#include "MousePicker.h"

MousePicker::MousePicker() {
	m_cursor = new MeshQuad(100, 100, 0);
	m_cursor->setPrecision(1, 1);
	m_cursor->m_offset = Vector3f(50.0f, 50.0f, 0.0f);
	m_cursor->buildMesh();
	m_cursor->setTexture(&Globals::textureManager.get("null"));
	m_cursor->setShader(Globals::shaderManager.getAssetPointer("ring"));

	m_mousePickBuffer.create(WIDTH, HEIGHT);
	m_mousePickBuffer.attachTexture(Framebuffer::Attachments::COLOR);
	m_mousePickBuffer.attachRenderbuffer(Framebuffer::Attachments::DEPTH24);

	const int DATA_SIZE = WIDTH * HEIGHT * 4;

	glGenBuffers(PBO_COUNT, pboIds);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void MousePicker::updateObjectId(unsigned int posX, unsigned int posY) {
	
	index = (index + 1) % 2;
	nextIndex = (index + 1) % 2;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mousePickBuffer.getFramebuffer());
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[index]);
	glReadPixels(posX, HEIGHT - posY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
	GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (src) {
		m_pickedId = src[0] + src[1] * 256 + src[2] * 256 * 256 /*+ src[3] * 256 * 256 * 256*/;
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

unsigned int MousePicker::getPickedId() const {
	return m_pickedId;
}

void MousePicker::update(float dt) {
	if (m_fadeIn) {
		m_radius = m_radius < 0.7f ? m_radius + m_transitionSpeed * dt : 0.7f;
		m_fadeIn = m_radius < 0.7f;
		m_transitionEnd = !m_fadeIn;
	}

	if (m_fadeOut) {
		m_radius = m_radius >= 0.0f ? m_radius - m_transitionSpeed * dt : 0.0f;
		m_fadeOut = m_radius >= 0.0f;
		m_transitionEnd = m_fadeOut;
	}
}

void MousePicker::updatePosition(unsigned int posX, unsigned int posY, const Camera& camera) {
	float mouseXndc = (2.0f * posX) / (float)WIDTH - 1.0f;
	float mouseYndc = 1.0f - (2.0f * posY) / (float)HEIGHT;

	Vector4f rayStartEye = Vector4f(mouseXndc, mouseYndc, -1.0f, 1.0f) ^ Globals::invProjection;
	Vector4f rayEndEye = Vector4f(mouseXndc, mouseYndc, 1.0f, 1.0f) ^ Globals::invProjection;
	rayEndEye = rayEndEye * (1.0f / rayEndEye[3]);


	Vector3f rayStartWorld = rayStartEye * camera.getInvViewMatrix();
	Vector3f rayEndWorld = rayEndEye * camera.getInvViewMatrix();

	Vector3f rayDirection = rayEndWorld - rayStartWorld;
	Vector3f::Normalize(rayDirection);


	btVector3 origin = btVector3(rayStartWorld[0], rayStartWorld[1], rayStartWorld[2]);
	btVector3 target = btVector3(rayEndWorld[0], rayEndWorld[1], rayEndWorld[2]);

	MousePickCallback callback(origin, target, Physics::collisiontypes::RAY, Physics::collisiontypes::TERRAIN | Physics::collisiontypes::RENDERABLE_OBJECT);
	
	Globals::physics->GetDynamicsWorld()->rayTest(origin, target, callback);

	if (callback.hasHit()) {

		Vector3f normal = Vector3f(callback.m_hitNormalWorld[0], callback.m_hitNormalWorld[1], callback.m_hitNormalWorld[2]);
		Vector3f tangent = Vector3f::Cross(camera.getViewDirection(), normal);
		//Vector3f::Normalize(tangent);
		Vector3f bitangent = Vector3f::Cross(normal, tangent);
		//Vector3f::Normalize(bitangent);


		Matrix4f rotation;

		rotation[0][0] = tangent[0];
		rotation[1][0] = normal[0];
		rotation[2][0] = bitangent[0];
		rotation[3][0] = 0.0f;

		rotation[0][1] = tangent[1];
		rotation[1][1] = normal[1];
		rotation[2][1] = bitangent[1];
		rotation[3][1] = 0.0f;

		rotation[0][2] = tangent[2];
		rotation[1][2] = normal[2];
		rotation[2][2] = bitangent[2];
		rotation[3][2] = 0.0f;

		rotation[0][3] = 0.0f;
		rotation[1][3] = 0.0f;
		rotation[2][3] = 0.0f;
		rotation[3][3] = 1.0f;

		Matrix4f trans;
		trans.translate(callback.m_hitPointWorld[0], callback.m_hitPointWorld[1], callback.m_hitPointWorld[2]);
		m_cursor->setTransformation(trans * rotation);
	}
}

void MousePicker::draw(const Camera& camera){
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("ring");
	glUseProgram(shader->m_program);
	shader->loadFloat("u_radius", m_radius);
	glUseProgram(0);
	m_cursor->draw(camera);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void MousePicker::click() {
	if (!m_transitionEnd) {
		m_fadeOut = m_fadeIn;
		m_fadeIn = !m_fadeIn;
	}else {
		m_fadeIn = m_fadeOut;
		m_fadeOut = !m_fadeOut;
	}
}

Framebuffer MousePicker::getBuffer() const {
	return m_mousePickBuffer;
}