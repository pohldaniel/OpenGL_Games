#include "Frustum.h"

std::unique_ptr<Shader> Frustum::s_shaderFrustum = nullptr;

void SATData::calculate(const Frustum& frustum) {
	size_t idx = 0;
	axes[idx++] = Vector3f::RIGHT;
	axes[idx++] = Vector3f::UP;
	axes[idx++] = Vector3f::FORWARD;

	// Add frustum normals. Disregard the near plane as it only points the other way from the far plane
	for (size_t i = 1; i < 6; ++i)
		axes[idx++] = frustum.m_planes[i];

	// Finally add cross product axes
	Vector3f frustumEdges[6] = {
		frustum.m_vertices[0] - frustum.m_vertices[2],
		frustum.m_vertices[0] - frustum.m_vertices[1],
		frustum.m_vertices[4] - frustum.m_vertices[0],
		frustum.m_vertices[5] - frustum.m_vertices[1],
		frustum.m_vertices[6] - frustum.m_vertices[2],
		frustum.m_vertices[7] - frustum.m_vertices[3]
	};
	
	for (size_t i = 0; i < 3; ++i){
		for (size_t j = 0; j < 6; ++j)
			axes[idx++] = Vector3f::Cross(axes[i], frustumEdges[j]);
	}

	// Now precalculate the projections of the frustum on each axis
	for (size_t i = 0; i < NUM_SAT_AXES; ++i)
		frustumProj[i] = frustum.projected(axes[i]);
}

Frustum::Frustum() {
	
}

Frustum::~Frustum() {
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vboPos)
		glDeleteBuffers(1, &m_vboPos);

	if (m_vboCount)
		glDeleteBuffers(1, &m_vboCount);
}

void Frustum::init() {
	if (!s_shaderFrustum) {
		s_shaderFrustum = std::unique_ptr<Shader>(new Shader(FRUSTUM_VERTEX, FRUSTUM_FRGAMENT, false));
	}
}

bool Frustum::aabbVisible(const Vector3f* AABBVertices) {
	for (int i = 0; i < 6; i++) {
		if (Vector4f::Dot(m_planes[i], AABBVertices[m_origins[i]]) + m_planes[i][3] < 0) {
			return false;
		}
	}

	return true;
}

float Frustum::aabbDistance(const Vector3f* AABBVertices) {
	return Vector4f::Dot(m_planes[5], AABBVertices[m_origins[5]]);
}

void Frustum::updatePlane(const Matrix4f& perspective, const Matrix4f& view, const Matrix4f& model) {
	Matrix4f mvp = perspective * view * model;

	//Left
	m_planes[0][0] = mvp[0][3] + mvp[0][0];
	m_planes[0][1] = mvp[1][3] + mvp[1][0];
	m_planes[0][2] = mvp[2][3] + mvp[2][0];
	m_planes[0][3] = mvp[3][3] + mvp[3][0];
	//m_planes[0].normalize3();
	m_origins[0] = m_planes[0][2] < 0.0f ? (m_planes[0][1] < 0.0f ? (m_planes[0][0] < 0.0f ? 0 : 1) : (m_planes[0][0] < 0.0f ? 2 : 3)) : (m_planes[0][1] < 0.0f ? (m_planes[0][0] < 0.0f ? 4 : 5) : (m_planes[0][0] < 0.0f ? 6 : 7));

	//Right
	m_planes[1][0] = mvp[0][3] - mvp[0][0];
	m_planes[1][1] = mvp[1][3] - mvp[1][0];
	m_planes[1][2] = mvp[2][3] - mvp[2][0];
	m_planes[1][3] = mvp[3][3] - mvp[3][0];
	//m_planes[1].normalize3();
	m_origins[1] = m_planes[1][2] < 0.0f ? (m_planes[1][1] < 0.0f ? (m_planes[1][0] < 0.0f ? 0 : 1) : (m_planes[1][0] < 0.0f ? 2 : 3)) : (m_planes[1][1] < 0.0f ? (m_planes[1][0] < 0.0f ? 4 : 5) : (m_planes[1][0] < 0.0f ? 6 : 7));
	
	//Bottom
	m_planes[2][0] = mvp[0][3] + mvp[0][1];
	m_planes[2][1] = mvp[1][3] + mvp[1][1];
	m_planes[2][2] = mvp[2][3] + mvp[2][1];
	m_planes[2][3] = mvp[3][3] + mvp[3][1];
	//m_planes[2].normalize3();
	m_origins[2] = m_planes[2][2] < 0.0f ? (m_planes[2][1] < 0.0f ? (m_planes[2][0] < 0.0f ? 0 : 1) : (m_planes[2][0] < 0.0f ? 2 : 3)) : (m_planes[2][1] < 0.0f ? (m_planes[2][0] < 0.0f ? 4 : 5) : (m_planes[2][0] < 0.0f ? 6 : 7));

	//Top
	m_planes[3][0] = mvp[0][3] - mvp[0][1];
	m_planes[3][1] = mvp[1][3] - mvp[1][1];
	m_planes[3][2] = mvp[2][3] - mvp[2][1];
	m_planes[3][3] = mvp[3][3] - mvp[3][1];
	//m_planes[3].normalize3();
	m_origins[3] = m_planes[3][2] < 0.0f ? (m_planes[3][1] < 0.0f ? (m_planes[3][0] < 0.0f ? 0 : 1) : (m_planes[3][0] < 0.0f ? 2 : 3)) : (m_planes[3][1] < 0.0f ? (m_planes[3][0] < 0.0f ? 4 : 5) : (m_planes[3][0] < 0.0f ? 6 : 7));

	//Far
	m_planes[4][0] = mvp[0][3] - mvp[0][2];
	m_planes[4][1] = mvp[1][3] - mvp[1][2];
	m_planes[4][2] = mvp[2][3] - mvp[2][2];
	m_planes[4][3] = mvp[3][3] - mvp[3][2];
	//m_planes[4].normalize3();
	m_origins[4] = m_planes[4][2] < 0.0f ? (m_planes[4][1] < 0.0f ? (m_planes[4][0] < 0.0f ? 0 : 1) : (m_planes[4][0] < 0.0f ? 2 : 3)) : (m_planes[4][1] < 0.0f ? (m_planes[4][0] < 0.0f ? 4 : 5) : (m_planes[4][0] < 0.0f ? 6 : 7));

	//Near
	m_planes[5][0] = mvp[0][3] + mvp[0][2];
	m_planes[5][1] = mvp[1][3] + mvp[1][2];
	m_planes[5][2] = mvp[2][3] + mvp[2][2];
	m_planes[5][3] = mvp[3][3] + mvp[3][2];
	//m_planes[5].normalize3();
	m_origins[5] = m_planes[5][2] < 0.0f ? (m_planes[5][1] < 0.0f ? (m_planes[5][0] < 0.0f ? 0 : 1) : (m_planes[5][0] < 0.0f ? 2 : 3)) : (m_planes[5][1] < 0.0f ? (m_planes[5][0] < 0.0f ? 4 : 5) : (m_planes[5][0] < 0.0f ? 6 : 7));	
}

void Frustum::updatePlane(const Matrix4f& perspective, const Matrix4f& view) {
	Matrix4f mvp = perspective * view;
	float inset = 1.0f;

	//Near
	m_planes[0][0] = mvp[0][3] + mvp[0][2];
	m_planes[0][1] = mvp[1][3] + mvp[1][2];
	m_planes[0][2] = mvp[2][3] + mvp[2][2];
	m_planes[0][3] = (mvp[3][3] + mvp[3][2]) * inset;
	//m_planes[0].normalize3();

	//Left
	m_planes[1][0] = mvp[0][3] + mvp[0][0];
	m_planes[1][1] = mvp[1][3] + mvp[1][0];
	m_planes[1][2] = mvp[2][3] + mvp[2][0];
	m_planes[1][3] = (mvp[3][3] + mvp[3][0]) * inset;
	//m_planes[1].normalize3();

	//Right
	m_planes[2][0] = mvp[0][3] - mvp[0][0];
	m_planes[2][1] = mvp[1][3] - mvp[1][0];
	m_planes[2][2] = mvp[2][3] - mvp[2][0];
	m_planes[2][3] = (mvp[3][3] - mvp[3][0]) * inset;
	//m_planes[2].normalize3();

	//Top
	m_planes[3][0] = mvp[0][3] - mvp[0][1];
	m_planes[3][1] = mvp[1][3] - mvp[1][1];
	m_planes[3][2] = mvp[2][3] - mvp[2][1];
	m_planes[3][3] = (mvp[3][3] - mvp[3][1]) * inset;
	//m_planes[3].normalize3();

	//Bottom
	m_planes[4][0] = mvp[0][3] + mvp[0][1];
	m_planes[4][1] = mvp[1][3] + mvp[1][1];
	m_planes[4][2] = mvp[2][3] + mvp[2][1];
	m_planes[4][3] = (mvp[3][3] + mvp[3][1]) * inset;
	//m_planes[4].normalize3();
	
	//Far
	m_planes[5][0] = mvp[0][3] - mvp[0][2];
	m_planes[5][1] = mvp[1][3] - mvp[1][2];
	m_planes[5][2] = mvp[2][3] - mvp[2][2];
	m_planes[5][3] = (mvp[3][3] - mvp[3][2]) * inset;
	//m_planes[5].normalize3();	
}

void Frustum::updateVertices(const Matrix4f& perspective, const Matrix4f& view) {
	float aspect = perspective[1][1] / perspective[0][0];
	float halfViesize = 1.0f / perspective[1][1];

	float near = perspective[3][2] / (perspective[2][2] - 1);
	float heightNear = halfViesize * near;
	float widthNear = heightNear * aspect;

	float far = perspective[3][2] / (perspective[2][2] + 1);
	float heightFar = halfViesize * far;
	float widthFar = heightFar * aspect;

	Matrix4f world = Matrix4f::ViewToWorldMatrix(view);
	m_vertices[0] = world ^ Vector4f(widthNear, heightNear, near, 1.0f);
	m_vertices[1] = world ^ Vector4f(widthNear, -heightNear, near, 1.0f);
	m_vertices[2] = world ^ Vector4f(-widthNear, -heightNear, near, 1.0f);
	m_vertices[3] = world ^ Vector4f(-widthNear, heightNear, near, 1.0f);

	m_vertices[4] = world ^ Vector4f(widthFar, heightFar, far, 1.0f);
	m_vertices[5] = world ^ Vector4f(widthFar, -heightFar, far, 1.0f);
	m_vertices[6] = world ^ Vector4f(-widthFar, -heightFar, far, 1.0f);
	m_vertices[7] = world ^ Vector4f(-widthFar, heightFar, far, 1.0f);
}

void Frustum::updateVbo(const Matrix4f& perspective, const Matrix4f& view) {
	if (m_debug) {
		float aspect = perspective[1][1] / perspective[0][0];
		float halfViesize = 1.0f / perspective[1][1];

		float near = perspective[3][2] / (perspective[2][2] - 1);
		float heightNear = halfViesize * near;
		float widthNear = heightNear *  aspect;

		float far = perspective[3][2] / (perspective[2][2] + 1);
		float heightFar = halfViesize * far;
		float widthFar = heightFar * aspect;
		
		Matrix4f world = Matrix4f::ViewToWorldMatrix(view);
		m_vertices[0] = world ^ Vector4f(widthNear, heightNear, near, 1.0f);
		m_vertices[1] = world ^ Vector4f(widthNear, -heightNear, near, 1.0f);
		m_vertices[2] = world ^ Vector4f(-widthNear, -heightNear, near, 1.0f);
		m_vertices[3] = world ^ Vector4f(-widthNear, heightNear, near, 1.0f);

		m_vertices[4] = world ^ Vector4f(widthFar, heightFar, far, 1.0f);
		m_vertices[5] = world ^ Vector4f(widthFar, -heightFar, far, 1.0f);
		m_vertices[6] = world ^ Vector4f(-widthFar, -heightFar, far, 1.0f);
		m_vertices[7] = world ^ Vector4f(-widthFar, heightFar, far, 1.0f);

		/*const Vector3f right = Vector3f(view[0][0], view[1][0], view[2][0]);
		const Vector3f up = Vector3f(view[0][1], view[1][1], view[2][1]);
		const Vector3f viewDirection = Vector3f(-view[0][2], -view[1][2], -view[2][2]);
		const Vector3f pos = Vector3f(-(view[3][0] * view[0][0] + view[3][1] * view[0][1] + view[3][2] * view[0][2]),
			-(view[3][0] * view[1][0] + view[3][1] * view[1][1] + view[3][2] * view[1][2]),
			-(view[3][0] * view[2][0] + view[3][1] * view[2][1] + view[3][2] * view[2][2]));

		//worldSpace
		Vector3f centerNear = pos + viewDirection * near;
		Vector3f centerFar = pos + viewDirection * far;

		m_vertices[0] = centerNear + right * widthNear + up * heightNear; //nearRightTop
		m_vertices[1] = centerNear + right * widthNear - up * heightNear; //nearRightBottom
		m_vertices[2] = centerNear - right * widthNear - up * heightNear; //nearLeftBottom
		m_vertices[3] = centerNear - right * widthNear + up * heightNear; //nearLeftTop
		
		m_vertices[4] = centerFar + right * widthFar + up * heightFar; //farRightTop
		m_vertices[5] = centerFar + right * widthFar - up * heightFar; //farRightBottom
		m_vertices[6] = centerFar - right * widthFar - up * heightFar; //farLeftBottom
		m_vertices[7] = centerFar - right * widthFar + up * heightFar; //farLeftTop*/
		
		if (!m_vao) {
			const unsigned short indicesFrustum[] = {
				0, 1, 2, 3,
				4, 5, 6, 7,
				1, 2, 6, 5,
				0, 3, 7, 4,
				0, 1, 5, 4,
				2, 3, 7, 6
			};

			const unsigned int vertexCount[] = {
				0, 1, 2, 3,
				4, 5, 6, 7
			};

			unsigned int iboFrustum;
			glGenBuffers(1, &iboFrustum);
			glGenBuffers(1, &m_vboPos);
			glGenBuffers(1, &m_vboCount);

			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			//Position
			glBindBuffer(GL_ARRAY_BUFFER, m_vboPos);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vector3f), nullptr, GL_DYNAMIC_DRAW);


			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			//Count
			glBindBuffer(GL_ARRAY_BUFFER, m_vboCount);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(unsigned int), vertexCount, GL_STATIC_DRAW);

			glEnableVertexAttribArray(1);
			glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboFrustum);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFrustum), indicesFrustum, GL_STATIC_DRAW);
			
			glBindVertexArray(0);
			glDeleteBuffers(1, &iboFrustum);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vboPos);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(Vector3f), m_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Frustum::drawFrustum(const Matrix4f& projection, const Matrix4f& view, float distance, const Vector4f& colorNear, const Vector4f& colorFar) {
	if (!(m_debug && m_vao)) return;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", projection * view * Matrix4f::Translate(0.0f, 0.0f, distance));	
	s_shaderFrustum->loadVector("u_colorFar", colorFar);
	s_shaderFrustum->loadVector("u_colorNear", colorNear);
	

	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

bool Frustum::intersectAABBFrustum(const Vector3f& position, const Vector3f& size) {
	Vector3f max = position + size;
	Vector3f min = position;

	// check box outside/inside of frustum
	for (int i = 0; i < 6; i++) {
		int out = 0;
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], min[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], min[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], max[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], max[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], min[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], min[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], max[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], max[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		if (out == 8) return false;
	}

	return true;
}

bool Frustum::IntersectAABBPlane(const Vector3f& position, const Vector3f& size, const Vector4f& plane) {

	Vector3f extents = size * 0.5f;
	Vector3f center = position + extents;

	float r = extents[0] * abs(plane[0]) + extents[1] * abs(plane[1]) + extents[2] * abs(plane[2]);
	float s = Vector4f::Dot(plane, center) - plane[3];
	return abs(s) <= r;
}

bool& Frustum::getDebug() {
	return m_debug;
}

unsigned char Frustum::isInsideMasked(const BoundingBox& box, unsigned char planeMask) const {
	Vector3f center = box.getCenter();
	Vector3f edge = center - box.min;

	for (size_t i = 0; i < NUM_FRUSTUM_PLANES; ++i){
		unsigned char bit = 1 << i;
		
		if (planeMask & bit){
			const Vector4f& plane = m_planes[i];
			float dist = Vector4f::Dot(m_planes[i], center) + plane[3];
			float absDist = Vector4f::DotAbs(m_planes[i], edge);

			//std::cout << "Dist: " << dist << "  " << absDist << std::endl;

			if (dist < -absDist)
				return 0xff;
			else if (dist >= absDist)
				planeMask &= ~bit;
		}
	}

	return planeMask;
}

std::pair<float, float> Frustum::projected(const Vector3f& axis) const{
	std::pair<float, float> ret;
	ret.first = ret.second = Vector3f::Dot(axis, m_vertices[0]);

	for (size_t i = 1; i < NUM_FRUSTUM_VERTICES; ++i){
		float proj = Vector3f::Dot(axis, m_vertices[i]);
		ret.first  = std::min(proj, ret.first);
		ret.second = std::max(proj, ret.second);
	}
	return ret;
}

BoundingBox::Intersection Frustum::isInsideSAT(const BoundingBox& box, const SATData& data) const{
	for (size_t i = 0; i < NUM_SAT_AXES; ++i){
		std::pair<float, float> boxProj = box.projected(data.axes[i]);
		if (data.frustumProj[i].second < boxProj.first || boxProj.second < data.frustumProj[i].first)
			return BoundingBox::OUTSIDE;
	}

	return BoundingBox::INSIDE;
}