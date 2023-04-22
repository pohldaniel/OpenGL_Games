#include "Cloth.h"
#include "Constants.h"

Cloth::Cloth() : m_position(Vector3f(1.0f, 1.0f, 1.0f)) {

}

Cloth::~Cloth() {

}

void Cloth::Configure(float w, float h, int totalParticlesW, int totalParticlesH) {
	// Create shader program	
	m_shader = new Shader("res/Shaders/Cloth Shaders/VertexShader.vs", "res/Shaders/Cloth Shaders/FragmentShader.fs");
	// Create texture
	m_textureComponent = Globals::textureManager.get("clothTex");

	// Allocate enough room for particles of cloth in the vector
	m_particles.resize(totalParticlesW * totalParticlesH);
	m_numParticlesWidth = totalParticlesW;
	m_numParticlesHeight = totalParticlesH;

	for (unsigned int i = 0; i < totalParticlesW; ++i) {
		for (unsigned int j = 0; j < totalParticlesH; ++j) {
			Vector3f pos = Vector3f(w * (i / (float)m_numParticlesWidth), -h * (j / (float)totalParticlesH), 0);

			// Add particle at element (i, j)
			m_particles[j * totalParticlesW + i] = ClothParticle(pos);
		}
	}

	// Connect close neighbours with constraints
	for (unsigned int i = 0; i < totalParticlesW; ++i) {

		for (unsigned int j = 0; j < totalParticlesH; ++j) {

			if (i < totalParticlesW - 1)
				CreateConstraint(GetParticle(i, j), GetParticle(i + 1, j));

			if (j < totalParticlesH - 1)
				CreateConstraint(GetParticle(i, j), GetParticle(i, j + 1));

			if (i < totalParticlesW - 1 && j < totalParticlesH - 1)
				CreateConstraint(GetParticle(i, j), GetParticle(i + 1, j + 1));

			if (i < totalParticlesW - 1 && j < totalParticlesH - 1)
				CreateConstraint(GetParticle(i + 1, j), GetParticle(i, j + 1));
		}
	}

	// Connect secondary neighbours with constraints
	for (unsigned int i = 0; i < totalParticlesW; ++i) {

		for (unsigned int j = 0; j < totalParticlesH; ++j) {

			if (i < totalParticlesW - 2)
				CreateConstraint(GetParticle(i, j), GetParticle(i + 2, j));

			if (j < totalParticlesH - 2)
				CreateConstraint(GetParticle(i, j), GetParticle(i, j + 2));

			if (i < totalParticlesW - 2 && j < totalParticlesH - 2)
				CreateConstraint(GetParticle(i, j), GetParticle(i + 2, j + 2));

			if (i < totalParticlesW - 2 && j < totalParticlesH - 2)
				CreateConstraint(GetParticle(i + 2, j), GetParticle(i, j + 2));
		}
	}

	// Pin particles
	for (unsigned int i = 0; i < 3; ++i) {

		// Top left particles
		GetParticle(i, 0)->Pin();

		for (unsigned int j = 0; j < totalParticlesH; ++j) {

			if (j >= totalParticlesH - 3) {
				// Bottom left particles
				GetParticle(i, j)->Pin();
			}
		}
	}
}

// -------------------
// Author: Rony Hanna
// Description: Function that renders the cloth (sets vertex buffers and sends it to GPU for rendering)
// -------------------
void Cloth::Draw(const Camera& cam) {

	m_shader->use();
	m_textureComponent.bind(0);

	// Reset normals
	for (auto p = m_particles.begin(); p != m_particles.end(); ++p)
		(*p).ZeroNormal();

	for (unsigned int i = 0; i < m_numParticlesWidth - 1; ++i) {

		for (unsigned int j = 0; j < m_numParticlesHeight - 1; ++j) {

			Vector3f normal = CalculateTriNormal(GetParticle(i + 1, j), GetParticle(i, j), GetParticle(i, j + 1));
			GetParticle(i + 1, j)->AddToNormal(normal);
			GetParticle(i, j)->AddToNormal(normal);
			GetParticle(i, j + 1)->AddToNormal(normal);

			normal = CalculateTriNormal(GetParticle(i + 1, j + 1), GetParticle(i + 1, j), GetParticle(i, j + 1));
			GetParticle(i + 1, j + 1)->AddToNormal(normal);
			GetParticle(i + 1, j)->AddToNormal(normal);
			GetParticle(i, j + 1)->AddToNormal(normal);
		}
	}

	static GLuint vertexArrayObject = 0;
	static GLuint vertexBuffer = 0;
	static GLuint texture;
	static int elementSize;

	if (vertexArrayObject == 0) {

		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (const GLvoid *)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (const GLvoid *)sizeof(Vector3f));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (const GLvoid *)(sizeof(Vector3f) + sizeof(Vector2f)));

		std::vector<int> indices;

		for (int j = 0; j < m_numParticlesHeight - 1; ++j) {
			int index;

			if (j > 0)
				indices.push_back(j * m_numParticlesWidth);

			for (int i = 0; i <= m_numParticlesWidth - 1; ++i) {
				index = j * m_numParticlesWidth + i;
				indices.push_back(index);
				indices.push_back(index + m_numParticlesWidth);
			}

			if (j + 1 < m_numParticlesHeight - 1)
				indices.push_back(index + m_numParticlesWidth);
		}

		elementSize = indices.size();

		GLuint elementArrayBuffer;
		glGenBuffers(1, &elementArrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementSize * sizeof(int), &indices[0], GL_STATIC_DRAW);
	}

	std::vector<Vert> vertexData;

	for (int j = 0; j < m_numParticlesHeight; ++j) {
		for (int i = 0; i < m_numParticlesWidth; ++i) {
			Vector2f uv(i / (m_numParticlesWidth - 1.0f), j / (m_numParticlesHeight - 1.0f));
			AddTriangle(GetParticle(i, j), uv, vertexData);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vert), &vertexData[0], GL_STREAM_DRAW);

	Matrix4f view = cam.getViewMatrix();
	Matrix4f model = Matrix4f::Translate(m_position);
	Matrix4f mvp = cam.getPerspectiveMatrix() * view * model;
	m_shader->loadMatrix("mvp", mvp);
	m_shader->loadMatrix("view", view);

	glBindVertexArray(vertexArrayObject);
	glDrawElements(GL_TRIANGLE_STRIP, elementSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	m_shader->unuse();
}

// -------------------
// Author: Rony Hanna
// Description: Function that updates the particles of a cloth each frame
// -------------------
void Cloth::Update() {

	for (unsigned int i = 0; i < 3; ++i) {

		for (auto iter = m_constraints.begin(); iter != m_constraints.end(); ++iter)
			(*iter).SatisfyConstraint();
	}

	// Calculate the position of each particle  
	for (auto p = m_particles.begin(); p != m_particles.end(); ++p)
		(*p).VerletIntegration();
}

// -------------------
// Author: Rony Hanna
// Description: Function that adds a directional force to all particles
// -------------------
void Cloth::AddForce(const Vector3f& dir) {
	for (auto p = m_particles.begin(); p != m_particles.end(); ++p)
		(*p).AddForce(dir);
}

// -------------------
// Author: Rony Hanna
// Description: Function that adds a wind force to all particles
// -------------------
void Cloth::WindForce(const Vector3f& dir) {

	for (unsigned int i = 0; i < m_numParticlesWidth - 1; ++i) {

		for (unsigned int j = 0; j < m_numParticlesHeight - 1; ++j) {

			AddWindForce(GetParticle(i + 1, j), GetParticle(i, j), GetParticle(i, j + 1), dir);
			AddWindForce(GetParticle(i + 1, j + 1), GetParticle(i + 1, j), GetParticle(i, j + 1), dir);
		}
	}
}

// -------------------
// Author: Rony Hanna
// Description: Function that calculates the normal vector of triangle where the normal vector is equal to the area of the parallelogram defined by the particles
// -------------------
Vector3f Cloth::CalculateTriNormal(ClothParticle* p1, ClothParticle* p2, ClothParticle* p3) {

	Vector3f pos1 = p1->GetPos();
	Vector3f pos2 = p2->GetPos();
	Vector3f pos3 = p3->GetPos();

	Vector3f v1 = pos2 - pos1;
	Vector3f v2 = pos3 - pos1;

	return Vector3f::Cross(v1, v2);
}

// -------------------
// Author: Rony Hanna
// Description: Function that calculates the wind force for a triangle
// -------------------
void Cloth::AddWindForce(ClothParticle* p1, ClothParticle* p2, ClothParticle* p3, const Vector3f& windDir) {
	Vector3f normal = CalculateTriNormal(p1, p2, p3);
	Vector3f normalFinal = Vector3f::Normalize(normal);
	Vector3f force = normal * Vector3f::Dot(normalFinal, windDir);

	p1->AddForce(force);
	p2->AddForce(force);
	p3->AddForce(force);
}

// -------------------
// Author: Rony Hanna
// Description: Function that adds a vertex into a vector of vertex data in order to form and draw a triangle
// -------------------
void Cloth::AddTriangle(ClothParticle* p1, const Vector2f& uv, std::vector<Vert>& vertexData) {
	Vert v1 = { p1->GetPos(), uv, p1->GetNormal() };
	vertexData.push_back(v1);
}