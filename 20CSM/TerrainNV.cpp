#include "TerrainNV.h"

float cam_pos[3] = { 75.5, 30.0f, -110 };
float cam_view[3] = { -0.7f, 0.0f, 0.7f };

int depth_size = 2048;

const float sensitivity = 0.005;
const float walk_speed = 0.5;

TerrainNV::TerrainNV() {
	tex = 0;
	heights = NULL;
	normals = NULL;
}

TerrainNV::~TerrainNV() {
	if (tex)
		glDeleteTextures(1, &tex);
	if (heights)
		delete[] heights;
	if (normals)
		delete[] normals;

	height = 0;
	width = 0;
}

bool TerrainNV::Load() {
	
	m_canyd.loadFromFile("res/gcanyond.png", true);
	m_ent.loadFromFile("res/entities.png", true);

	m_canyi.loadDDSRawFromFile("res/gcanyon.dds", GL_COMPRESSED_RGBA_S3TC_DXT1_ANGLE);
	m_canyi.setLinear(GL_LINEAR_MIPMAP_NEAREST);

	tex = m_canyi.getTexture();

	height = m_canyd.getHeight();
	width = m_canyd.getWidth();
	int size = height * width;

	heights = new float[size * sizeof(float)];

	GLubyte *src = m_canyd.readPixel();
	for (int i = 0; i < size; i++) {
		heights[i] = (float)src[3 * i] * SCALE;
	}

	normals = new float[3 * size * sizeof(float)];

	for (int z = 1; z<height - 1; z++) {
		for (int x = 1; x<width - 1; x++) {
			float dyx = heights[x + 1 + z*width]
				- heights[x - 1 + z*width];
			float dyz = heights[x + (z + 1)*width]
				- heights[x + (z - 1)*width];

			Vector3f vx(1, dyx, 0.0f);
			Vector3f vz(0.0f, -dyz, -1);
			Vector3f v = Vector3f::Normalize(Vector3f::Cross(vx, vz));
			normals[3 * (x + z*width)] = v[0];
			normals[3 * (x + z*width) + 1] = v[1];
			normals[3 * (x + z*width) + 2] = v[2];
		}
	}

	int e_width = m_ent.getWidth();
	int e_height = m_ent.getHeight();
	float e_ratio_x = (float)e_width / (float)width;
	float e_ratio_z = (float)e_height / (float)height;
	float half_width = 0.5f*(float)width;
	float half_height = 0.5f*(float)height;

	GLubyte *ent = m_ent.readPixel();
	for (int z = 0; z<e_height; z++) {
		for (int x = 0; x<e_width; x++) {
			if (ent[3 * (x + z*e_width)] == 255) {
				m_instances.push_back(Matrix4f::Translate(-half_width, 0, -half_height) * Matrix4f::Translate(e_ratio_x * (float)x, heights[x + z*width], e_ratio_z * (float)z));
			}
		}
	}

	LoadTree();
	MakeTerrain();

	return true;
}

void TerrainNV::Draw(const Camera& camera) {
	Draw(-10000.0f, camera.getPerspectiveMatrix(), camera.getViewMatrix());
}

void TerrainNV::Draw(float minCamZ, const Matrix4f& proj, const Matrix4f& view){

	auto shader = Globals::shaderManager.getAssetPointer("instance");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", proj);
	shader->loadMatrix("u_modelView", view);
	shader->loadMatrix("u_view", view);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(view));
	//shader->loadVector("u_viewPos", camera.getPosition());

	shader->loadVector("color", Vector4f(0.917647f, 0.776471f, 0.576471f, 0.0f));
	modelT.drawRawInstanced();

	shader->loadVector("color", Vector4f(0.301961f, 0.588235f, 0.309804f, 0.0f));
	modelL.drawRawInstanced();

	glUseProgram(0);
	
	shader = Globals::shaderManager.getAssetPointer("mesh");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", proj);
	shader->loadMatrix("u_modelView", view);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(view));
	//shader->loadVector("u_viewPos", camera.getPosition());

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);	
}

void TerrainNV::DrawRawTerrain() {
	

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void TerrainNV::DrawRawTrunk() {
	modelT.drawRawInstanced();
}

void TerrainNV::DrawRawLeave() {
	modelL.drawRawInstanced();
}

void TerrainNV::DrawCoarse() {
	float half_width = 0.5f*(float)width;
	float half_height = 0.5f*(float)height;

	const float inv_height = 1.0f / (float)height;
	const float inv_width = 1.0f / (float)width;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-half_width, 0, -half_height);

	glBindTexture(GL_TEXTURE_2D, tex);

	for (int z = 1; z<height - 2; z += 8) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 1; x<width - 1; x += 8) {
			float fx = (float)x;
			float fz = (float)z;
			glTexCoord2f(fx*inv_width, fz*inv_height);
			glNormal3fv(&normals[3 * (x + z*width)]);
			glVertex3f(fx, heights[x + z*width], fz);
			glTexCoord2f(fx*inv_width, (fz + 8.0f)*inv_height);
			glNormal3fv(&normals[3 * (x + (z + 8)*width)]);
			glVertex3f(fx, heights[x + (z + 8)*width], fz + 8.0f);
		}
		glEnd();
	}
	glPopMatrix();
}

void TerrainNV::MakeTerrain() {

	float half_width = 0.5f*(float)width;
	float half_height = 0.5f*(float)height;

	const float inv_height = 1.0f / (float)height;
	const float inv_width = 1.0f / (float)width;

	std::vector<float> vertexBuffer;
	for (int z = 1; z < height - 1; z++) {
		for (int x = 1; x < width - 1; x++) {
			float fx = (float)x;
			float fz = (float)z;
			vertexBuffer.push_back(fx - half_width); vertexBuffer.push_back(heights[x + z*width]); vertexBuffer.push_back(fz - half_height);
			vertexBuffer.push_back(fx*inv_width); vertexBuffer.push_back(fz*inv_height);
			vertexBuffer.push_back(normals[3 * (x + z*width)]); vertexBuffer.push_back(normals[3 * (x + z*width) + 1]); vertexBuffer.push_back(normals[3 * (x + z*width) + 2]);		
		}
	}

	int resolutionZ = (height - 2); //from the veretex for loop
	int resolutionX = (width - 2); //from the veretex for loop

	std::vector<unsigned int> indexBuffer;
	for (int z = 0; z < resolutionZ - 1; ++z) {
		
		if (z % 2 == 0) {

			for (int x = resolutionX - 1; x >= 0; --x) {
				indexBuffer.push_back(x + (z + 1) * resolutionX);
				indexBuffer.push_back(x + z * resolutionX);
				
			}

			// Add degenerate triangles to stitch strips together.
			indexBuffer.push_back((z + 1) * resolutionX);

		} else {
			for (int x = 0; x < resolutionX; ++x) {
				indexBuffer.push_back(x + (z + 1) * resolutionX);
				indexBuffer.push_back(x + z * (resolutionX));
			}

			// Add degenerate triangles to stitch strips together.
			indexBuffer.push_back((resolutionX - 1) + (z + 1) * resolutionX);

		}
	}
	
	
	m_totalIndices = indexBuffer.size();
	//m_totalIndices = (resolutionZ - 1)* (2 * resolutionX + 1);

	glGenBuffers(1, &m_vbo);
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//Texccords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void TerrainNV::LoadTree() {
	modelT.loadModel("res/models/trunk.obj");
	modelL.loadModel("res/models/leaves.obj");

	//float half_width = 0.5f*(float)width;
	//float half_height = 0.5f*(float)height;
	//for (unsigned int i = 0; i < entities.size(); i++) {
	//	m_instances.push_back(Matrix4f::Translate(-half_width, 0, -half_height) * Matrix4f::Translate((*entities[i])[0], (*entities[i])[1], (*entities[i])[2]) );
	//}
	modelT.createInstancesStatic(m_instances);
	modelL.createInstancesStatic(m_instances);

	//modelT.addInstance(Matrix4f::IDENTITY);
	//modelL.addInstance(Matrix4f::IDENTITY);
}


void TerrainNV::DrawTree() {
	modelT.drawRawInstanced();
	modelL.drawRawInstanced();
}
