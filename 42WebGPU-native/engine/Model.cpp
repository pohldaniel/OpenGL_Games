#include <vector>
#include <array>

#include "Mesh.h"
#include "Model.h"

void Model::GenerateColors(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int& stride, unsigned int startIndex, unsigned int endIndex, ModelColor modelColor) {
	if (stride < 8)
		return;

	std::vector<float> tmpVertex;
	for (int i = 0; i < vertexBuffer.size(); i++) {
		tmpVertex.push_back(vertexBuffer[i]);

		if ((i + 1) % stride == 0) {
			if (modelColor == ModelColor::MC_POSITION) {
				tmpVertex.push_back(vertexBuffer[i - stride + 1]);
				tmpVertex.push_back(vertexBuffer[i - stride + 2]);
				tmpVertex.push_back(vertexBuffer[i - stride + 3]);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_WHITE) {
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_RED) {
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_GREEN) {
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_BLUE) {
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_BLACK) {
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
			}
		}
	}

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(vertexBuffer));
	tmpVertex.clear();
	stride += 4;
}

void Model::PackBuffer(std::vector<float>& vertexBuffer, unsigned int stride) {
	std::vector<float> vertexBufferNew;
	unsigned int strideNew = 0u;

	if (stride == 3) {
		vertexBufferNew.resize(vertexBuffer.size() / 3 * 4);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 4, k = k + 3) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;
		}
		strideNew = 4;
	}

	if (stride == 5) {
		vertexBufferNew.resize(vertexBuffer.size() / 5 * 8);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 8, k = k + 5) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;
		}
		strideNew = 8;
	}

	if (stride == 8) {
		vertexBufferNew.resize(vertexBuffer.size() / 8 * 12);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 12, k = k + 8) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;

			vertexBufferNew[i + 8] = vertexBuffer[k + 5];
			vertexBufferNew[i + 9] = vertexBuffer[k + 6];
			vertexBufferNew[i + 10] = vertexBuffer[k + 7];
			vertexBufferNew[i + 11] = 0.0f;
		}
		strideNew = 12;
	}

	if (stride == 14) {
		vertexBufferNew.resize(vertexBuffer.size() / 14 * 20);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 16, k = k + 20) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;

			vertexBufferNew[i + 8] = vertexBuffer[k + 5];
			vertexBufferNew[i + 9] = vertexBuffer[k + 6];
			vertexBufferNew[i + 10] = vertexBuffer[k + 7];
			vertexBufferNew[i + 11] = 0.0f;

			vertexBufferNew[i + 12] = vertexBuffer[k + 8];
			vertexBufferNew[i + 13] = vertexBuffer[k + 9];
			vertexBufferNew[i + 14] = vertexBuffer[k + 10];
			vertexBufferNew[i + 15] = 0.0f;

			vertexBufferNew[i + 16] = vertexBuffer[k + 11];
			vertexBufferNew[i + 17] = vertexBuffer[k + 12];
			vertexBufferNew[i + 18] = vertexBuffer[k + 13];
			vertexBufferNew[i + 19] = 0.0f;
		}
		strideNew = 20;
	}
	stride = strideNew;
	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	vertexBuffer.insert(vertexBuffer.end(), vertexBufferNew.begin(), vertexBufferNew.end());
}

void Model::Rewind(const std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride) {
	if (stride < 6)
		return;

	unsigned int offset = stride == 6 ? 3 : 5;

	for (unsigned int i = 0; i < indexBuffer.size(); i = i + 3) {
		std::array<float, 3> a = { vertexBuffer[indexBuffer[i] * stride], vertexBuffer[indexBuffer[i] * stride + 1], vertexBuffer[indexBuffer[i] * stride + 2] };
		std::array<float, 3> b = { vertexBuffer[indexBuffer[i + 1] * stride], vertexBuffer[indexBuffer[i + 1] * stride + 1], vertexBuffer[indexBuffer[i + 1] * stride + 2] };
		std::array<float, 3> c = { vertexBuffer[indexBuffer[i + 2] * stride], vertexBuffer[indexBuffer[i + 2] * stride + 1], vertexBuffer[indexBuffer[i + 2] * stride + 2] };
		std::array<float, 3> ab = { b[0] - a[0] , b[1] - a[1], b[2] - a[2] };
		std::array<float, 3> ac = { c[0] - a[0] , c[1] - a[1], c[2] - a[2] };
		std::array<float, 3> faceNormal = Normalize(Cross(ab, ac));
		std::array<float, 3> n1 = { vertexBuffer[indexBuffer[i] * stride + offset], vertexBuffer[indexBuffer[i] * stride + offset + 1u], vertexBuffer[indexBuffer[i] * stride + offset + 2u] };
		std::array<float, 3> n2 = { vertexBuffer[indexBuffer[i + 1] * stride + offset], vertexBuffer[indexBuffer[i + 1] * stride + offset + 1u], vertexBuffer[indexBuffer[i + 1] * stride + offset + 2u] };
		std::array<float, 3> n3 = { vertexBuffer[indexBuffer[i + 2] * stride + offset], vertexBuffer[indexBuffer[i + 2] * stride + offset + 1u], vertexBuffer[indexBuffer[i + 2] * stride + offset + 2u] };
		std::array<float, 3> vertexNormal = Normalize({ n1[0] + n2[0] + n3[0],  n1[1] + n2[1] + n3[1] , n1[2] + n2[2] + n3[2] });

		float dot = Dot(faceNormal, vertexNormal);

		if (dot < 0.0f) {
			unsigned int index2 = indexBuffer[i + 1];
			indexBuffer[i + 1] = indexBuffer[i + 2];
			indexBuffer[i + 2] = index2;
		}
	}
}

void Model::GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, Model& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex) {
	if (hasNormals) { return; }

	std::vector<float> tmpVertex;
	const unsigned int* pTriangle = 0;
	float* pVertex0 = 0;
	float* pVertex1 = 0;
	float* pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float length = 0.0f;
	int vertexLength = stride == 5 ? 8 : 6;
	int vertexOffset = stride == 5 ? 2 : 0;

	for (int i = 0; i < vertexBuffer.size(); i++) {
		tmpVertex.push_back(vertexBuffer[i]);
		if ((i + 1) % stride == 0) {

			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
		}
	}

	for (int j = startIndex; j < endIndex; j++) {
		for (int i = 0; i < model.m_meshes[j]->m_numberOfTriangles; i++) {

			pTriangle = &indexBuffer[i * 3 + model.m_meshes[j]->m_baseIndex];

			pVertex0 = &vertexBuffer[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex1 = &vertexBuffer[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex2 = &vertexBuffer[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * stride];

			// Calculate triangle face normal.
			edge1[0] = pVertex1[0] - pVertex0[0];
			edge1[1] = pVertex1[1] - pVertex0[1];
			edge1[2] = pVertex1[2] - pVertex0[2];

			edge2[0] = pVertex2[0] - pVertex0[0];
			edge2[1] = pVertex2[1] - pVertex0[1];
			edge2[2] = pVertex2[2] - pVertex0[2];

			normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
			normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
			normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

			// Accumulate the normals.
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] + normal[2];

			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] + normal[2];

			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] + normal[2];
		}
	}

	for (int i = 0; i < tmpVertex.size(); i = i + vertexLength) {

		length = 1.0f / sqrtf(tmpVertex[i + 3 + vertexOffset] * tmpVertex[i + 3 + vertexOffset] +
			tmpVertex[i + 4 + vertexOffset] * tmpVertex[i + 4 + vertexOffset] +
			tmpVertex[i + 5 + vertexOffset] * tmpVertex[i + 5 + vertexOffset]);

		tmpVertex[i + 3 + vertexOffset] *= length;
		tmpVertex[i + 4 + vertexOffset] *= length;
		tmpVertex[i + 5 + vertexOffset] *= length;
	}

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(vertexBuffer));
	tmpVertex.clear();

	stride = stride + 3;
	hasNormals = true;
}

void Model::GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords) {
	normalCoords.resize(vertexCoords.size());
	float pVertex0[3] = { 0.0f, 0.0f, 0.0f };
	float pVertex1[3] = { 0.0f, 0.0f, 0.0f };
	float pVertex2[3] = { 0.0f, 0.0f, 0.0f };
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float length;

	for (unsigned int i = 0; i < face.size(); i++) {

		pVertex0[0] = vertexCoords[((face[i])[0] - 1) * 3]; pVertex0[1] = vertexCoords[((face[i])[0] - 1) * 3 + 1]; pVertex0[2] = vertexCoords[((face[i])[0] - 1) * 3 + 2];
		pVertex1[0] = vertexCoords[((face[i])[1] - 1) * 3]; pVertex1[1] = vertexCoords[((face[i])[1] - 1) * 3 + 1]; pVertex1[2] = vertexCoords[((face[i])[1] - 1) * 3 + 2];
		pVertex2[0] = vertexCoords[((face[i])[2] - 1) * 3]; pVertex2[1] = vertexCoords[((face[i])[2] - 1) * 3 + 1]; pVertex2[2] = vertexCoords[((face[i])[2] - 1) * 3 + 2];

		// Calculate triangle face normal.
		edge1[0] = pVertex1[0] - pVertex0[0];
		edge1[1] = pVertex1[1] - pVertex0[1];
		edge1[2] = pVertex1[2] - pVertex0[2];

		edge2[0] = pVertex2[0] - pVertex0[0];
		edge2[1] = pVertex2[1] - pVertex0[1];
		edge2[2] = pVertex2[2] - pVertex0[2];

		normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
		normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
		normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

		normalCoords[((face[i])[0] - 1) * 3] = normalCoords[((face[i])[0] - 1) * 3] + normal[0];
		normalCoords[((face[i])[0] - 1) * 3 + 1] = normalCoords[((face[i])[0] - 1) * 3 + 1] + normal[1];
		normalCoords[((face[i])[0] - 1) * 3 + 2] = normalCoords[((face[i])[0] - 1) * 3 + 2] + normal[2];

		normalCoords[((face[i])[1] - 1) * 3] = normalCoords[((face[i])[1] - 1) * 3] + normal[0];
		normalCoords[((face[i])[1] - 1) * 3 + 1] = normalCoords[((face[i])[1] - 1) * 3 + 1] + normal[1];
		normalCoords[((face[i])[1] - 1) * 3 + 2] = normalCoords[((face[i])[1] - 1) * 3 + 2] + normal[2];

		normalCoords[((face[i])[2] - 1) * 3] = normalCoords[((face[i])[2] - 1) * 3] + normal[0];
		normalCoords[((face[i])[2] - 1) * 3 + 1] = normalCoords[((face[i])[2] - 1) * 3 + 1] + normal[1];
		normalCoords[((face[i])[2] - 1) * 3 + 2] = normalCoords[((face[i])[2] - 1) * 3 + 2] + normal[2];

		(face[i])[6] = (face[i])[0]; (face[i])[7] = (face[i])[1]; (face[i])[8] = (face[i])[2];
	}

	for (int i = 0; i < normalCoords.size(); i = i + 3) {

		length = 1.0f / sqrtf(normalCoords[i] * normalCoords[i] + normalCoords[i + 1] * normalCoords[i + 1] + normalCoords[i + 2] * normalCoords[i + 2]);

		normalCoords[i] *= length;
		normalCoords[i + 1] *= length;
		normalCoords[i + 2] *= length;
	}
}

void Model::GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, Model& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex) {
	if (hasTangents) { return; }
	if (stride == 3 || stride == 6) { return; }
	if (!hasNormals) {
		Model::GenerateNormals(vertexBuffer, indexBuffer, model, hasNormals, stride, startIndex, endIndex);
	}

	std::vector<float> tmpVertex;
	const unsigned int* pTriangle = 0;
	float* pVertex0 = 0;
	float* pVertex1 = 0;
	float* pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float texEdge1[2] = { 0.0f, 0.0f };
	float texEdge2[2] = { 0.0f, 0.0f };
	float tangent[3] = { 0.0f, 0.0f, 0.0f };
	float bitangent[3] = { 0.0f, 0.0f, 0.0f };
	float det = 0.0f;
	float nDotT = 0.0f;
	float bDotB = 0.0f;
	float length = 0.0f;

	for (int i = 0; i < vertexBuffer.size(); i++) {

		tmpVertex.push_back(vertexBuffer[i]);

		if ((i + 1) % stride == 0) {

			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
		}
	}

	for (int j = startIndex; j < endIndex; j++) {
		for (int i = 0; i < model.m_meshes[j]->m_numberOfTriangles; i++) {

			pTriangle = &indexBuffer[i * 3 + model.m_meshes[j]->m_baseIndex];

			pVertex0 = &vertexBuffer[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex1 = &vertexBuffer[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex2 = &vertexBuffer[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * stride];

			// Calculate triangle face normal.
			edge1[0] = pVertex1[0] - pVertex0[0];
			edge1[1] = pVertex1[1] - pVertex0[1];
			edge1[2] = pVertex1[2] - pVertex0[2];

			edge2[0] = pVertex2[0] - pVertex0[0];
			edge2[1] = pVertex2[1] - pVertex0[1];
			edge2[2] = pVertex2[2] - pVertex0[2];

			texEdge1[0] = pVertex1[3] - pVertex0[3];
			texEdge1[1] = pVertex1[4] - pVertex0[4];

			texEdge2[0] = pVertex2[3] - pVertex0[3];
			texEdge2[1] = pVertex2[4] - pVertex0[4];

			det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

			if (fabs(det) < 1e-6f) {

				tangent[0] = 1.0f;
				tangent[1] = 0.0f;
				tangent[2] = 0.0f;

				bitangent[0] = 0.0f;
				bitangent[1] = 1.0f;
				bitangent[2] = 0.0f;

			}else {

				det = 1.0f / det;

				tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
				tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
				tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

				bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
				bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
				bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
			}

			// Accumulate the tangents and bitangents.
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 8] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 8] + tangent[0];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 9] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 9] + tangent[1];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 10] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 10] + tangent[2];

			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 11] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 11] + bitangent[0];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 12] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 12] + bitangent[1];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 13] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 13] + bitangent[2];

			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 8] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 8] + tangent[0];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 9] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 9] + tangent[1];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 10] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 10] + tangent[2];

			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 11] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 11] + bitangent[0];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 12] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 12] + bitangent[1];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 13] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 13] + bitangent[2];

			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 8] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 8] + tangent[0];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 9] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 9] + tangent[1];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 10] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 10] + tangent[2];

			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 11] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 11] + bitangent[0];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 12] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 12] + bitangent[1];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 13] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 13] + bitangent[2];
		}
	}

	// Orthogonalize and normalize the vertex tangents.
	for (int i = 0; i < tmpVertex.size(); i = i + 14) {

		pVertex0 = &tmpVertex[i];

		// Gram-Schmidt orthogonalize tangent with normal.

		nDotT = pVertex0[5] * pVertex0[8] +
			pVertex0[6] * pVertex0[9] +
			pVertex0[7] * pVertex0[10];

		pVertex0[8] -= pVertex0[5] * nDotT;
		pVertex0[9] -= pVertex0[6] * nDotT;
		pVertex0[10] -= pVertex0[7] * nDotT;

		// Normalize the tangent.

		length = 1.0f / sqrtf(pVertex0[8] * pVertex0[8] +
			pVertex0[9] * pVertex0[9] +
			pVertex0[10] * pVertex0[10]);

		pVertex0[8] *= length;
		pVertex0[9] *= length;
		pVertex0[10] *= length;

		bitangent[0] = (pVertex0[6] * pVertex0[10]) -
			(pVertex0[7] * pVertex0[9]);
		bitangent[1] = (pVertex0[7] * pVertex0[8]) -
			(pVertex0[5] * pVertex0[10]);
		bitangent[2] = (pVertex0[5] * pVertex0[9]) -
			(pVertex0[6] * pVertex0[8]);

		bDotB = bitangent[0] * pVertex0[11] +
			bitangent[1] * pVertex0[12] +
			bitangent[2] * pVertex0[13];

		// Calculate handedness
		if (bDotB < 0.0f) {
			pVertex0[11] = -bitangent[0];
			pVertex0[12] = -bitangent[1];
			pVertex0[13] = -bitangent[2];

		}else {

			pVertex0[11] = bitangent[0];
			pVertex0[12] = bitangent[1];
			pVertex0[13] = bitangent[2];
		}
	}

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(vertexBuffer));
	tmpVertex.clear();

	hasTangents = true;
	stride = 14;
}

void Model::GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords) {

	if (textureCoords.empty()) return;

	tangentCoords.resize(vertexCoords.size());
	bitangentCoords.resize(vertexCoords.size());
	std::vector<float> tmpNormalCoords;
	tmpNormalCoords.resize(vertexCoords.size());

	float pVertex0[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float pVertex1[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float pVertex2[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float texEdge1[2] = { 0.0f, 0.0f };
	float texEdge2[2] = { 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float tangent[3] = { 0.0f, 0.0f, 0.0f };
	float bitangent[3] = { 0.0f, 0.0f, 0.0f };
	float det = 0.0f;

	for (unsigned int i = 0; i < face.size(); i++) {
		pVertex0[0] = vertexCoords[((face[i])[0] - 1) * 3]; pVertex0[1] = vertexCoords[((face[i])[0] - 1) * 3 + 1]; pVertex0[2] = vertexCoords[((face[i])[0] - 1) * 3 + 2];
		pVertex0[3] = textureCoords[((face[i])[3] - 1) * 2]; pVertex0[4] = textureCoords[((face[i])[3] - 1) * 2 + 1];
		tmpNormalCoords[((face[i])[0] - 1) * 3] = normalCoords[((face[i])[6] - 1) * 3];
		tmpNormalCoords[((face[i])[0] - 1) * 3 + 1] = normalCoords[((face[i])[6] - 1) * 3 + 1];
		tmpNormalCoords[((face[i])[0] - 1) * 3 + 2] = normalCoords[((face[i])[6] - 1) * 3 + 2];

		pVertex1[0] = vertexCoords[((face[i])[1] - 1) * 3]; pVertex1[1] = vertexCoords[((face[i])[1] - 1) * 3 + 1]; pVertex1[2] = vertexCoords[((face[i])[1] - 1) * 3 + 2];
		pVertex1[3] = textureCoords[((face[i])[4] - 1) * 2]; pVertex1[4] = textureCoords[((face[i])[4] - 1) * 2 + 1];
		tmpNormalCoords[((face[i])[1] - 1) * 3] = normalCoords[((face[i])[7] - 1) * 3];
		tmpNormalCoords[((face[i])[1] - 1) * 3 + 1] = normalCoords[((face[i])[7] - 1) * 3 + 1];
		tmpNormalCoords[((face[i])[1] - 1) * 3 + 2] = normalCoords[((face[i])[7] - 1) * 3 + 2];

		pVertex2[0] = vertexCoords[((face[i])[2] - 1) * 3]; pVertex2[1] = vertexCoords[((face[i])[2] - 1) * 3 + 1]; pVertex2[2] = vertexCoords[((face[i])[2] - 1) * 3 + 2];
		pVertex2[3] = textureCoords[((face[i])[5] - 1) * 2]; pVertex2[4] = textureCoords[((face[i])[5] - 1) * 2 + 1];
		tmpNormalCoords[((face[i])[2] - 1) * 3] = normalCoords[((face[i])[8] - 1) * 3];
		tmpNormalCoords[((face[i])[2] - 1) * 3 + 1] = normalCoords[((face[i])[8] - 1) * 3 + 1];
		tmpNormalCoords[((face[i])[2] - 1) * 3 + 2] = normalCoords[((face[i])[8] - 1) * 3 + 2];

		edge1[0] = pVertex1[0] - pVertex0[0];
		edge1[1] = pVertex1[1] - pVertex0[1];
		edge1[2] = pVertex1[2] - pVertex0[2];

		edge2[0] = pVertex2[0] - pVertex0[0];
		edge2[1] = pVertex2[1] - pVertex0[1];
		edge2[2] = pVertex2[2] - pVertex0[2];

		texEdge1[0] = pVertex1[3] - pVertex0[3];
		texEdge1[1] = pVertex1[4] - pVertex0[4];

		texEdge2[0] = pVertex2[3] - pVertex0[3];
		texEdge2[1] = pVertex2[4] - pVertex0[4];

		det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

		if (fabs(det) < 1e-6f) {

			tangent[0] = 1.0f;
			tangent[1] = 0.0f;
			tangent[2] = 0.0f;

			bitangent[0] = 0.0f;
			bitangent[1] = 1.0f;
			bitangent[2] = 0.0f;

		}else {
			det = 1.0f / det;

			tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
			tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
			tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

			bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
			bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
			bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
		}

		tangentCoords[((face[i])[0] - 1) * 3] = tangentCoords[((face[i])[0] - 1) * 3] + tangent[0];
		tangentCoords[((face[i])[0] - 1) * 3 + 1] = tangentCoords[((face[i])[0] - 1) * 3 + 1] + tangent[1];
		tangentCoords[((face[i])[0] - 1) * 3 + 2] = tangentCoords[((face[i])[0] - 1) * 3 + 2] + tangent[2];

		tangentCoords[((face[i])[1] - 1) * 3] = tangentCoords[((face[i])[1] - 1) * 3] + tangent[0];
		tangentCoords[((face[i])[1] - 1) * 3 + 1] = tangentCoords[((face[i])[1] - 1) * 3 + 1] + tangent[1];
		tangentCoords[((face[i])[1] - 1) * 3 + 2] = tangentCoords[((face[i])[1] - 1) * 3 + 2] + tangent[2];

		tangentCoords[((face[i])[2] - 1) * 3] = tangentCoords[((face[i])[2] - 1) * 3] + tangent[0];
		tangentCoords[((face[i])[2] - 1) * 3 + 1] = tangentCoords[((face[i])[2] - 1) * 3 + 1] + tangent[1];
		tangentCoords[((face[i])[2] - 1) * 3 + 2] = tangentCoords[((face[i])[2] - 1) * 3 + 2] + tangent[2];

		bitangentCoords[((face[i])[0] - 1) * 3] = bitangentCoords[((face[i])[0] - 1) * 3] + bitangent[0];
		bitangentCoords[((face[i])[0] - 1) * 3 + 1] = bitangentCoords[((face[i])[0] - 1) * 3 + 1] + bitangent[1];
		bitangentCoords[((face[i])[0] - 1) * 3 + 2] = bitangentCoords[((face[i])[0] - 1) * 3 + 2] + bitangent[2];

		bitangentCoords[((face[i])[1] - 1) * 3] = bitangentCoords[((face[i])[1] - 1) * 3] + bitangent[0];
		bitangentCoords[((face[i])[1] - 1) * 3 + 1] = bitangentCoords[((face[i])[1] - 1) * 3 + 1] + bitangent[1];
		bitangentCoords[((face[i])[1] - 1) * 3 + 2] = bitangentCoords[((face[i])[1] - 1) * 3 + 2] + bitangent[2];

		bitangentCoords[((face[i])[2] - 1) * 3] = bitangentCoords[((face[i])[2] - 1) * 3] + bitangent[0];
		bitangentCoords[((face[i])[2] - 1) * 3 + 1] = bitangentCoords[((face[i])[2] - 1) * 3 + 1] + bitangent[1];
		bitangentCoords[((face[i])[2] - 1) * 3 + 2] = bitangentCoords[((face[i])[2] - 1) * 3 + 2] + bitangent[2];
	}

	float nDotT = 0.0f;
	float bDotB = 0.0f;
	float length = 0.0f;

	for (unsigned int i = 0; i < tangentCoords.size(); i = i + 3) {

		normal[0] = tmpNormalCoords[i]; normal[1] = tmpNormalCoords[i + 1]; normal[2] = tmpNormalCoords[i + 2];
		tangent[0] = tangentCoords[i]; tangent[1] = tangentCoords[i + 1]; tangent[2] = tangentCoords[i + 2];

		// Gram-Schmidt orthogonalize tangent with normal.
		nDotT = normal[0] * tangent[0] +
			normal[1] * tangent[1] +
			normal[2] * tangent[2];

		tangent[0] -= normal[0] * nDotT;
		tangent[1] -= normal[1] * nDotT;
		tangent[2] -= normal[2] * nDotT;

		// Normalize the tangent.
		length = 1.0f / sqrtf(tangent[0] * tangent[0] +
			tangent[1] * tangent[1] +
			tangent[2] * tangent[2]);

		tangentCoords[i] *= length;
		tangentCoords[i + 1] *= length;
		tangentCoords[i + 2] *= length;

		bitangent[0] = (normal[1] * tangentCoords[i + 2]) -
			(normal[2] * tangentCoords[i + 1]);
		bitangent[1] = (normal[2] * tangentCoords[i]) -
			(normal[0] * tangentCoords[i + 2]);
		bitangent[2] = (normal[0] * tangentCoords[i + 1]) -
			(normal[1] * tangentCoords[i]);

		bDotB = bitangent[0] * bitangentCoords[i] +
			bitangent[1] * bitangentCoords[i + 1] +
			bitangent[2] * bitangentCoords[i + 2];

		// Calculate handedness
		if (bDotB < 0.0f) {
			bitangentCoords[i] = -bitangent[0];
			bitangentCoords[i + 1] = -bitangent[1];
			bitangentCoords[i + 2] = -bitangent[2];

		}else {
			bitangentCoords[i] = bitangent[0];
			bitangentCoords[i + 1] = bitangent[1];
			bitangentCoords[i + 2] = bitangent[2];
		}

	}
}

std::array<float, 3> Model::Normalize(const std::array<float, 3>& v) {
	float length = sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	float invMag = length != 0.0f ? 1.0f / length : 1.0f;
	return { v[0] * invMag, v[1] * invMag, v[2] * invMag };
}

std::array<float, 3> Model::Cross(const std::array<float, 3>& p, const std::array<float, 3>& q) {
	return { (p[1] * q[2]) - (p[2] * q[1]),
			 (p[2] * q[0]) - (p[0] * q[2]),
			 (p[0] * q[1]) - (p[1] * q[0]) };
}

float Model::Dot(const std::array<float, 3>& p, const std::array<float, 3>& q) {
	return (p[0] * q[0]) + (p[1] * q[1]) + (p[2] * q[2]);
}