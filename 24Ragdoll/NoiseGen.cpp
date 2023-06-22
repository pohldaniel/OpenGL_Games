#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include "NoiseGen.h"

const int N = 128;
Vector4f shapeData[N * N * N];

const int M = 32;
Vector4f detailData[M * M * M];

const int W_l = 8;
const int W_m = 12;
const int W_h = 16;

Vector3f Worely_low[W_l * W_l * W_l];
Vector3f Worely_mid[W_m * W_m * W_m];
Vector3f Worely_high[W_h * W_h * W_h];

NoiseGen::NoiseGen()
{
	
}

float GetRandom()
{
	return float(rand() % 10) / 10;
}

Vector3f random3(Vector3f st) {
	st = Vector3f(Vector3f::Dot(st, Vector3f(127.1, 311.7,235.4)),
		Vector3f::Dot(st, Vector3f(269.5, 183.3,421.8)), Vector3f::Dot(st, Vector3f(133.6,462.5,248.1)));
	return 2.0f * Vector3f::Fract(Vector3f(sin(st[0]), sin(st[1]), sin(st[2])) * 43758.5453123f) - Vector3f(1.0f);
}

float Remap(float v, float l0, float h0, float ln, float hn)
{
	return ln + ((v - l0) * (hn - ln)) / (h0 - l0);
}

bool NoiseGen::LoadVolumeFromFile(const char* fileName, unsigned int& tex, int dataSizeX, int dataSizeY, int dataSizeZ) {
	FILE* pFile = fopen(fileName, "rb");
	if (NULL == pFile) {
		return false;
	}
	int dataSize = dataSizeX * dataSizeY * dataSizeZ;
	Vector4f * pVolume = new Vector4f[dataSize];
	fread(pVolume, sizeof(Vector4f), dataSize, pFile);
	fclose(pFile);

	//load data into a 3D texture
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_3D, tex);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, dataSizeX, dataSizeY, dataSizeZ, 0, GL_RGBA, GL_FLOAT, pVolume);

	delete[] pVolume;
	return true;
}

bool NoiseGen::WriteVolumeToFile(const char* fileName, Vector4f *data, int dataSizeX, int dataSizeY, int dataSizeZ)
{
	FILE* pFile = fopen(fileName, "wb");
	if (NULL == pFile) {
		return false;
	}
	int dataSize = dataSizeX * dataSizeY * dataSizeZ;
	fwrite(data, sizeof(Vector4f), dataSize, pFile);
	fclose(pFile);
}

void NoiseGen::genWorleyGrid()
{
	// init
	srand(time(NULL));
	memset(Worely_low, 0, sizeof(Worely_low));
	memset(Worely_mid, 0, sizeof(Worely_mid));
	memset(Worely_high, 0, sizeof(Worely_high));

	// For the Grid
	// between (0,1)
	for (int x = 0; x < W_l; x++)
	{
		for (int y = 0; y < W_l; y++)
		{
			for (int z = 0; z < W_l; z++)
			{
				if (x == 0 || x == W_l - 1 ||
					y == 0 || y == W_l - 1 ||
					z == 0 || z == W_l - 1)
				{
					Worely_low[x * W_l * W_l + y * W_l + z] = Vector3f(-1, -1, -1);
					continue;
				}
				else
				{
					Worely_low[x * W_l * W_l + y * W_l + z] = Vector3f(x + GetRandom(),
																	y + GetRandom(),
																	z + GetRandom()) / (float)W_l;
				}
				
			}
		}
	}

	for (int x = 0; x < W_l; x++)
	{
		for (int y = 0; y < W_l; y++)
		{
			for (int z = 0; z < W_l; z++)
			{
				if (x == 0 || x == W_l - 1 ||
					y == 0 || y == W_l - 1 ||
					z == 0 || z == W_l - 1)
				{
					int fixX, fixY, fixZ;
					float offsetX, offsetY, offsetZ;
					
					if (x == 0) { fixX = W_l - 2; offsetX = -(W_l - 2); }
					else if (x == W_l - 1) { fixX = 1; offsetX = (W_l - 2); }
					else { fixX = x; offsetX = 0; }

					if (y == 0) { fixY = W_l - 2; offsetY = -(W_l - 2); }
					else if (y == W_l - 1) { fixY = 1; offsetY = (W_l - 2); }
					else { fixY = y; offsetY = 0; }

					if (z == 0) { fixZ = W_l - 2; offsetZ = -(W_l - 2); }
					else if (z == W_l - 1) { fixZ = 1; offsetZ = (W_l - 2); }
					else { fixZ = z; offsetZ = 0; }

					Worely_low[x * W_l * W_l + y * W_l + z] = Worely_low[fixX * W_l * W_l + fixY * W_l + fixZ]+ Vector3f(offsetX, offsetY, offsetZ) / (float)W_l;
				}
			}
		}
	}

	for (int x = 0; x < W_m; x++)
	{
		for (int y = 0; y < W_m; y++)
		{
			for (int z = 0; z < W_m; z++)
			{
				if (x == 0 || x == W_m - 1 ||
					y == 0 || y == W_m - 1 ||
					z == 0 || z == W_m - 1)
				{
					Worely_mid[x * W_m * W_m + y * W_m + z] = Vector3f(-1, -1, -1);
					continue;
				}
				else
				{
					Worely_mid[x * W_m * W_m + y * W_m + z] = Vector3f(x + GetRandom(),
						y + GetRandom(),
						z + GetRandom()) / (float)W_m;
				}

			}
		}
	}

	for (int x = 0; x < W_m; x++)
	{
		for (int y = 0; y < W_m; y++)
		{
			for (int z = 0; z < W_m; z++)
			{
				if (x == 0 || x == W_m - 1 ||
					y == 0 || y == W_m - 1 ||
					z == 0 || z == W_m - 1)
				{
					int fixX, fixY, fixZ;
					float offsetX, offsetY, offsetZ;

					if (x == 0) { fixX = W_m - 2; offsetX = -(W_m - 2); }
					else if (x == W_m - 1) { fixX = 1; offsetX = (W_m - 2); }
					else { fixX = x; offsetX = 0; }

					if (y == 0) { fixY = W_m - 2; offsetY = -(W_m - 2); }
					else if (y == W_m - 1) { fixY = 1; offsetY = (W_m - 2); }
					else { fixY = y; offsetY = 0; }

					if (z == 0) { fixZ = W_m - 2; offsetZ = -(W_m - 2); }
					else if (z == W_m - 1) { fixZ = 1; offsetZ = (W_m - 2); }
					else { fixZ = z; offsetZ = 0; }

					Worely_mid[x * W_m * W_m + y * W_m + z] = Worely_mid[fixX * W_m * W_m + fixY * W_m + fixZ] + Vector3f(offsetX, offsetY, offsetZ) / (float)W_m;
				}
			}
		}
	}

	for (int x = 0; x < W_h; x++)
	{
		for (int y = 0; y < W_h; y++)
		{
			for (int z = 0; z < W_h; z++)
			{
				if (x == 0 || x == W_h - 1 ||
					y == 0 || y == W_h - 1 ||
					z == 0 || z == W_h - 1)
				{
					Worely_high[x * W_h * W_h + y * W_h + z] = Vector3f(-1, -1, -1);
					continue;
				}
				else
				{
					Worely_high[x * W_h * W_h + y * W_h + z] = Vector3f(x + GetRandom(),
						y + GetRandom(),
						z + GetRandom()) / (float)W_h;
				}

			}
		}
	}

	for (int x = 0; x < W_h; x++)
	{
		for (int y = 0; y < W_h; y++)
		{
			for (int z = 0; z < W_h; z++)
			{
				if (x == 0 || x == W_h - 1 ||
					y == 0 || y == W_h - 1 ||
					z == 0 || z == W_h - 1)
				{
					int fixX, fixY, fixZ;
					float offsetX, offsetY, offsetZ;

					if (x == 0) { fixX = W_h - 2; offsetX = -(W_h - 2); }
					else if (x == W_h - 1) { fixX = 1; offsetX = (W_h - 2); }
					else { fixX = x; offsetX = 0; }

					if (y == 0) { fixY = W_h - 2; offsetY = -(W_h - 2); }
					else if (y == W_h - 1) { fixY = 1; offsetY = (W_h - 2); }
					else { fixY = y; offsetY = 0; }

					if (z == 0) { fixZ = W_h - 2; offsetZ = -(W_h - 2); }
					else if (z == W_h - 1) { fixZ = 1; offsetZ = (W_h - 2); }
					else { fixZ = z; offsetZ = 0; }

					Worely_high[x * W_h * W_h + y * W_h + z] = Worely_high[fixX * W_h * W_h + fixY * W_h + fixZ] + Vector3f(offsetX, offsetY, offsetZ) / (float)W_h;
				}
			}
		}
	}

}

void NoiseGen::getGloudShape(unsigned int &cloud_shape)
{

	if (LoadVolumeFromFile("res/clouds/CloudShapeData.raw", cloud_shape, N, N, N)){

		return;
	}
	
	genWorleyGrid();

	for (int x = 0; x < N; x++)
	{
		for (int y = 0; y < N; y++)
		{
			for (int z = 0; z < N; z++)
			{
				float R, G, B, A;
				// coord (0,1) fo 3D tex
				Vector3f pos = Vector3f(x, y, z) / float(N);

				//Low freq Perlin-Worley
				//R = (GetPerlinValue(glm::vec3(pos[0], pos[1], pos[2]), W_l));
				R = (getPerlinValue(pos, W_l));
				//Medium freq Worley
				G = getWorleyVaule(pos, W_l);

				//High freq Worley
				B = getWorleyVaule(pos, W_m);

				//Higest freq Worly
				A = getWorleyVaule(pos, W_h);

				shapeData[x * N * N + y * N + z] = Vector4f(R,G,B,A);
			}
		}
	}
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &cloud_shape);
	glBindTexture(GL_TEXTURE_3D, cloud_shape);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, N, N, N, 0, GL_RGBA, GL_FLOAT, shapeData);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_3D, 0);

	WriteVolumeToFile("res/clouds/CloudShapeData.raw", shapeData, N, N, N);

}

void NoiseGen::getGloudDetail(unsigned int& cloud_detail) {
	if (LoadVolumeFromFile("res/clouds/CloudDetailData.raw", cloud_detail, M, M, M)){
		return;
	}

	genWorleyGrid();

	for (int x = 0; x < M; x++)
	{
		for (int y = 0; y < M; y++)
		{
			for (int z = 0; z < M; z++)
			{
				float R, G, B;
				// coord (0,1) fo 3D tex
				Vector3f pos = Vector3f(x, y, z) / float(M);

				//Low freq Perlin-Worley
				R = getWorleyVaule(pos, W_l);
				
				//Medium freq Worley
				G = getWorleyVaule(pos, W_m);
				
				//High freq Worley
				B = getWorleyVaule(pos, W_h);

				detailData[x * M * M + y * M + z] = Vector4f(R, G, B, 0);
			}
		}
	}
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &cloud_detail);
	glBindTexture(GL_TEXTURE_3D, cloud_detail);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, M, M, M, 0, GL_RGBA, GL_FLOAT, detailData);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_3D, 0);

	WriteVolumeToFile("res/clouds/CloudDetailData.raw", detailData, M, M, M);

}

float NoiseGen::getPerlinValue(Vector3f texPos, int freq) {
	
	texPos *= (float)freq;

	Vector3f i = Vector3f(floor(texPos[0]), floor(texPos[1]), floor(texPos[2]));
	Vector3f f = Vector3f(texPos[0] - (long)texPos[0], texPos[1] - (long)texPos[1], texPos[2] - (long)texPos[2]);

	Vector3f u = f * f * (Vector3f(3.0f) - 2.0f * f);

	float result = Math::Mix(Math::Mix(Math::Mix(Vector3f::Dot(random3(i + Vector3f(0.0, 0.0, 0.0)), f - Vector3f(0.0, 0.0, 0.0)),
		Vector3f::Dot(random3(i + Vector3f(1.0, 0.0, 0.0)), f - Vector3f(1.0, 0.0, 0.0)), u[0]),
		Math::Mix(Vector3f::Dot(random3(i + Vector3f(0.0, 1.0, 0.0)), f - Vector3f(0.0, 1.0, 0.0)),
			Vector3f::Dot(random3(i + Vector3f(1.0, 1.0, 0.0)), f - Vector3f(1.0, 1.0, 0.0)), u[0]), u[1]),
		Math::Mix(Math::Mix(Vector3f::Dot(random3(i + Vector3f(0.0, 0.0, 1.0)), f - Vector3f(0.0, 0.0, 1.0)),
			Vector3f::Dot(random3(i + Vector3f(1.0, 0.0, 1.0)), f - Vector3f(1.0, 0.0, 1.0)), u[0]),
			Math::Mix(Vector3f::Dot(random3(i + Vector3f(0.0, 1.0, 1.0)), f - Vector3f(0.0, 1.0, 1.0)),
				Vector3f::Dot(random3(i + Vector3f(1.0, 1.0, 1.0)), f - Vector3f(1.0, 1.0, 1.0)), u[0]), u[1]),u[2]);
	result = Remap(result, -1, 1, 0, 1);
	return result;
}

float NoiseGen::getWorleyVaule(Vector3f texPos, int freq) {
	
	// map to (1, freq-2)
	int x = floor(texPos[0] * (freq - 2)) + 1;
	int y = floor(texPos[1] * (freq - 2)) + 1;
	int z = floor(texPos[2] * (freq - 2)) + 1;
	// texPos: (0,1) map to
	texPos = texPos * ((freq - 2) / (float)freq) + Vector3f(1/ (float)freq);
	float result = 1;

	Vector3f *p = NULL;

	switch (freq) {
		case W_l:
			p = Worely_low;
			break;
		case W_m:
			p = Worely_mid;
			break;
		case W_h:
			p = Worely_high;
			break;
		default:
			p = NULL;
			break;
	}

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			for (int k = -1; k <= 1; k++)
			{
				{
					int offset = (x + i) * freq * freq + (y + j) * freq + (z + k);
					float currentDist = (texPos - p[offset]).length();
					result = std::min(result, currentDist);
					if (result == 0) break;
				}
			}
		}
	}
	p = NULL;

	// remap t0 (0,1)
	float maxDist = (float)2 / (freq);
	result = Remap(result, 0, maxDist, 0, 0.9);
	// Inverse
	return 1 - result;
}