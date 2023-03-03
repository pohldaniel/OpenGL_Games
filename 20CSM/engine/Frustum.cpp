#include "Frustum.h"

std::unique_ptr<Shader> Frustum::s_shaderFrustum = nullptr;

Frustum::Frustum(const Matrix4f& perspective, float scale) {
	createBuffer(perspective, scale);
	if (!s_shaderFrustum) {
		s_shaderFrustum = std::unique_ptr<Shader>(new Shader(FRUSTUM_VERTEX, FRUSTUM_FRGAMENT, false));
	}
}

Frustum::~Frustum() {

}

void Frustum::createBuffer(const Matrix4f& perspective, float scale) {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	float near = perspective[3][2] / (perspective[2][2] - 1);	
	float heightNear = (2.0f / perspective[1][1] ) * near;
	float widthNear = (heightNear *  perspective[1][1]) / perspective[0][0];

	float far = perspective[3][2] / (perspective[2][2] + 1) * scale;
	float heightFar = (2.0f / perspective[1][1]) * far;
	float widthFar = (heightFar  * perspective[1][1]) / perspective[0][0];
	
	Vector3f nearBottomLeft = Vector3f(-0.5f * widthNear, -0.5f * heightNear, near);
	Vector3f nearBottomRight = Vector3f(0.5f * widthNear, -0.5f * heightNear, near);
	Vector3f nearTopRight = Vector3f(0.5f * widthNear, 0.5f * heightNear, near);
	Vector3f farBottomLeft = Vector3f(-0.5f * widthFar, -0.5f * heightFar, far);
	Vector3f farBottomRight = Vector3f(0.5f * widthFar, -0.5f * heightFar, far);
	Vector3f farTopRight = Vector3f(0.5f * widthFar, 0.5f * heightFar, far);
	m_center = Vector3f(0.0f, 0.0f, (far + near) * 0.5f);
	

	m_planes[0].normal = Vector3f::Normalize(Vector3f::Cross(nearBottomRight - nearBottomLeft, nearTopRight - nearBottomLeft));

	//std::cout << m_planes[0].normal[0] << "  " << m_planes[0].normal[1] << "  " << m_planes[0].normal[2] << std::endl;

	m_planes[0].normal[0] = perspective[3][0] + perspective[2][0];
	m_planes[0].normal[1] = perspective[3][1] + perspective[2][1];
	m_planes[0].normal[2] = perspective[3][2] + perspective[2][2];
	m_planes[0].d = perspective[3][3] + perspective[2][3];
	m_planes[0].p1 = nearBottomLeft;
	m_planes[0].p2 = nearBottomRight;
	m_planes[0].normalize();


	m_planes[1].normal[0] = perspective[3][0] - perspective[2][0];
	m_planes[1].normal[1] = perspective[3][1] - perspective[2][1];
	m_planes[1].normal[2] = perspective[3][2] - perspective[2][2];
	m_planes[1].d = perspective[3][3] - perspective[2][3];
	m_planes[1].p1 = farBottomLeft;
	m_planes[1].p2 = farBottomRight;
	m_planes[1].normalize();

	//std::cout << m_planes[0].normal[0] << "  " << m_planes[0].normal[1] << "  " << m_planes[0].normal[2] << std::endl;

	Vector3f tangent = Vector3f::Normalize(m_planes[0].p2 - m_planes[0].p1);
	Vector3f bitangent = Vector3f::Cross(tangent, m_planes[0].normal);

	//std::cout << tangent[0] << "  " << tangent[1] << "  " << tangent[2] << std::endl;
	//std::cout << bitangent[0] << "  " << bitangent[1] << "  " << bitangent[2] << std::endl;

	const Vector3f v1 = -tangent - bitangent + m_planes[0].normal * m_planes[0].d;
	const Vector3f v2 = -tangent + bitangent + m_planes[0].normal * m_planes[0].d;
	const Vector3f v3 = tangent + bitangent + m_planes[0].normal * m_planes[0].d;
	const Vector3f v4 = tangent - bitangent + m_planes[0].normal * m_planes[0].d;

	std::cout << v1[0] << "  " << v1[1] << "  " << v1[2] << std::endl;
	std::cout << v2[0] << "  " << v2[1] << "  " << v2[2] << std::endl;
	std::cout << v3[0] << "  " << v3[1] << "  " << v3[2] << std::endl;
	std::cout << v4[0] << "  " << v4[1] << "  " << v4[2] << std::endl;
	std::cout << "########" << std::endl;

	std::vector<float> vertex;
	Vector3f pos = nearBottomLeft;
	Vector3f size = nearTopRight - nearBottomLeft;
	
	std::cout << pos[0] << "  " << pos[1] << "  " << pos[2] + size[2] << std::endl;

	vertex.push_back(pos[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2] + size[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2] + size[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2] + size[2]);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2] + size[2]);

	pos = farBottomLeft;
	size = farTopRight - farBottomLeft;

	vertex.push_back(pos[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2]);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2]);


	/*static const GLushort indices[] = {
		0, 2, 3,
		0, 1, 2,

		4, 6, 7,
		4, 5, 6,

		// right
		1, 5, 6,
		6, 2, 1,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};*/

	static const GLushort indices[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		1, 2, 6, 5,
		0, 3, 7, 4,
		0, 1, 5, 4,
		2, 3, 7, 6
	};

	short stride = 3;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();


	

}

void Frustum::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Frustum::draw(const Camera& camera, const Vector3f& position, const Vector3f& scale) {
	//Matrix4f lightView = Matrix4f::Rotate(camera.getViewDirection(), position) * Matrix4f::SIGN;

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 90.0f, position)  * Matrix4f::Scale(scale[0], scale[1], scale[2], position) * camera.getRotationMatrix(position) );
	s_shaderFrustum->loadVector("u_color", Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	drawRaw();
	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

void Frustum::drawPlane(const Camera& camera, const Vector3f& position, const Vector3f& scale, const Vector4f& color, const Plane& plane) {
	
	Vector3f tangent = Vector3f::Normalize(plane.p2 - plane.p1);
	Vector3f bitangent = Vector3f::Cross(tangent, plane.normal);

	const Vector3f v1 = -tangent - bitangent + plane.normal * plane.d;
	const Vector3f v2 = -tangent + bitangent + plane.normal * plane.d;
	const Vector3f v3 = tangent + bitangent + plane.normal * plane.d;
	const Vector3f v4 = tangent - bitangent + plane.normal * plane.d;

	/*std::cout << v1[0] << "  " << v1[1] << "  " << v1[2] << std::endl;
	std::cout << v2[0] << "  " << v2[1] << "  " << v2[2] << std::endl;
	std::cout << v3[0] << "  " << v3[1] << "  " << v3[2] << std::endl;
	std::cout << v4[0] << "  " << v4[1] << "  " << v4[2] << std::endl;
	std::cout << "------------" << std::endl;*/

	std::vector<float> vertex;
	vertex.push_back(v1[0]); vertex.push_back(v1[1]); vertex.push_back(v1[2]);
	vertex.push_back(v2[0]); vertex.push_back(v2[1]); vertex.push_back(v2[2]);
	vertex.push_back(v3[0]); vertex.push_back(v3[1]); vertex.push_back(v3[2]);
	vertex.push_back(v4[0]); vertex.push_back(v4[1]); vertex.push_back(v4[2]);

	const GLushort indices[] = { 0,1,2, 3 };

	unsigned int ibo2;
	glGenBuffers(1, &ibo2);
	glGenBuffers(1, &m_vbo2);

	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo2);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 90.0f, position) * Matrix4f::Scale(scale[0], scale[1], scale[2], position) * camera.getRotationMatrix(position) * Matrix4f::SIGN);
	s_shaderFrustum->loadVector("u_color", color);

	//glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
	glBindVertexArray(m_vao2);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}