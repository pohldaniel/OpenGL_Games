#include "MousePicker.h"
#include "Application.h"

MousePicker::MousePicker() : m_callback(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)){
	/*m_cursor = new MeshQuad(100, 100, 0);
	m_cursor->setPrecision(1, 1);
	m_cursor->m_offset = Vector3f(50.0f, 50.0f, 0.0f);
	m_cursor->buildMesh();
	m_cursor->setTexture(&Globals::textureManager.get("null"));
	m_cursor->setShader(Globals::shaderManager.getAssetPointer("ring"));*/

	m_mousePickBuffer.create(WIDTH, HEIGHT);
	m_mousePickBuffer.attachTexture2D(AttachmentTex::RGBA);
	m_mousePickBuffer.attachRenderbuffer(AttachmentRB::DEPTH24);

	const int DATA_SIZE = WIDTH * HEIGHT * 4;

	glGenBuffers(PBO_COUNT, pboIds);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	m_quad = RenderableObject("quad", "ring", "null");
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

	btVector3 origin = btVector3(rayStartWorld[0], rayStartWorld[1], rayStartWorld[2]);
	btVector3 target = btVector3(rayEndWorld[0], rayEndWorld[1], rayEndWorld[2]);

	m_callback = MousePickCallback(origin, target, Physics::MOUSEPICKER, Physics::PICKABLE_OBJECT);
	
	Globals::physics->GetDynamicsWorld()->rayTest(origin, target, m_callback);

	if (m_callback.hasHit()) {
		m_quad.setPosition(Physics::VectorFrom(m_callback.m_hitPointWorld));
	}
}

void MousePicker::draw(const Camera& camera){
	m_quad.draw(camera);
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

	origin = btVector3(rayStartWorld[0], rayStartWorld[1], rayStartWorld[2]);
	target = btVector3(rayEndWorld[0], rayEndWorld[1], rayEndWorld[2]);

	m_callback = MousePickCallback(origin, target, Physics::MOUSEPICKER, Physics::PICKABLE_OBJECT);
	Globals::physics->GetDynamicsWorld()->rayTest(origin, target, m_callback);

	return m_callback.hasHit();
}

const btVector3& MousePicker::getOrigin() {
	return origin;
}

const btVector3& MousePicker::getTarget() {
	return target;
}

Framebuffer MousePicker::getBuffer() const {
	return m_mousePickBuffer;
}

const MousePickCallback& MousePicker::getCallback() {
	return m_callback;
}