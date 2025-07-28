#include <engine/Material.h>
#include <engine/Spritesheet.h>
#include "SceneManager.h"

Scene::Scene() {
	loadScene();
	m_xconvfactor = 2000.0f / ((100.0f / 640.0f) * 1024.0f);
	m_yconvfactor = 2000.0f / ((100.0f / 480.0f) * 768.0f);
}

Scene::~Scene() {
	Material::CleanupTextures();
}

void Scene::loadScene() {
	Material::AddTexture("res/textures/los.tga");
	Material::AddTexture();
	Material::AddTexture("res/textures/crate.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/barrel.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/aircraftgun.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/generalsbuilding.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/wall.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::GetTextures().back().setWrapMode(GL_REPEAT);
	Material::AddTexture("res/textures/200x200building.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/bunker.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/metal.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/ground.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();

	Material::AddTexture("data/models/dynamic/hero/hero.tga");
	Material::AddTexture("data/models/dynamic/hueteotl/hueteotl.tga");
	Material::AddTexture("data/models/dynamic/mutantman/mutantman.tga");
	Material::AddTexture("data/models/dynamic/corpse/corpse.tga");
	Material::AddTexture("data/models/dynamic/mutantlizard/mutantlizard.tga");
	Material::AddTexture("data/models/dynamic/mutantcheetah/mutantcheetah.tga");
	Material::AddTexture("data/models/dynamic/ripper/ripper.tga");
	Material::AddTexture("data/textures/misc/tree.tga");

	m_heroModel.load("data/models/dynamic/hero/hero.md2");
	m_hueteotl.load("data/models/dynamic/hueteotl/hueteotl.md2");
	m_mutantman.load("data/models/dynamic/mutantman/mutantman.md2");
	m_corpse.load("data/models/dynamic/corpse/corpse.md2");
	m_mutantlizard.load("data/models/dynamic/mutantlizard/mutantlizard.md2");
	m_mutantcheetah.load("data/models/dynamic/mutantcheetah/mutantcheetah.md2");
	m_ripper.load("data/models/dynamic/ripper/ripper.md2");

	loadBuilding("res/building_0.bld");
	loadBuilding("res/building_1.bld");
	loadBuilding("res/building_2.bld");
	loadBuilding("res/building_3.bld");
	loadBuilding("res/building_5.bld");
	loadBuilding("res/building_6.bld");
	loadBuilding("res/building_8.bld");
	loadBuilding("res/building_9.bld");
	loadBuilding("res/building_7.bld");
	loadBuilding("res/building_4.bld", true);

	loadBuilding("res/building_10.bld", true);
	loadBuilding("res/building_11.bld", true);
	loadBuilding("res/building_12.bld");
	loadBuilding("res/building_13.bld", true);
	loadBuilding("res/building_14.bld");
	loadBuilding("res/building_15.bld", true);

	loadBuilding("res/building_16.bld", true);
	loadBuilding("res/building_17.bld");
	loadBuilding("res/building_18.bld");
	loadBuilding("res/building_19.bld");
	loadBuilding("res/building_20.bld");

	loadBuilding("res/building_21.bld", true);
	loadBuilding("res/building_22.bld");
	loadBuilding("res/building_23.bld", true);
	loadBuilding("res/building_24.bld", true);
	loadBuilding("res/building_25.bld");

	std::for_each(m_buildings.begin(), m_buildings.end(), std::mem_fn(&Shape::markForDelete));

	m_sphere.fromObj("res/models/sphere.obj");
	m_sphere.markForDelete();

	m_segment.buildSegmentXZ(100.0f, 60.0f, 120.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, false, false);
	m_segment.createBoundingBox();
	m_segment.markForDelete();

	m_disk.buildDiskXZ(20.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, false, false);
	m_disk.createBoundingBox();
	m_disk.markForDelete();

	loadMiniMap("data/maps/default/main.map");
	loadFont();
}

void Scene::loadBuilding(const char* fn, bool changeWinding) {
	FILE *f;
	char buf[1024];
	char filepath[256];
	strncpy(filepath, fn, 256);

	if ((f = fopen(filepath, "r")) == NULL) {
		fprintf(stderr, "Cannot open building file: %s\n", filepath);
		exit(-1);
	}

	std::vector<Vector3f> positions;
	std::vector<Vector2f> texels;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<Vector4f> shapeColor;

	Vector4f currentColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	std::string currentTexture;
	unsigned int currentPolygon;

	while (fgets(buf, 1024, f) != NULL) {
		char *str = buf + 5;
		float fl[5];
		int ret;

		if (!strncmp(buf, "#", 1))
			continue;

		for (int i = 0; str[i]; i++)
			if (str[i] == '\n' || str[i] == '\r')
				str[i] = '\0';

		ret = sscanf(str, "%f,%f,%f,%f,%f", &fl[0], &fl[1], &fl[2], &fl[3], &fl[4]);
		if (!strncmp(buf, "txco:", 5)) {
			texels.push_back({ fl[0], fl[1] });
		}else if (!strncmp(buf, "colr:", 5)) {
			currentColor.set(fl[0], fl[1], fl[2], fl[3]);
		}else if (!strncmp(buf, "vrtx:", 5)) {
			positions.push_back({ fl[0], fl[1], fl[2] });
		}else if (!strncmp(buf, "begn:", 5)) {
			sscanf(str, "%d", &currentPolygon);
		}else if (!strncmp(buf, "txtr:", 5)) {
			if (currentTexture != str && !currentTexture.empty()) {
				m_buildings.push_back(Shape(vertices, indices, 8u));
				m_buildings.back().createBoundingBox();
				m_buildings.back().setVec4Attribute(shapeColor, 0u, 3u);

				vertices.shrink_to_fit();
				vertices.clear();
				indices.shrink_to_fit();
				indices.clear();
				shapeColor.shrink_to_fit();
				shapeColor.clear();
			}
			currentTexture = str;
		}else if (!strncmp(buf, "ends", 4)) {

			if (currentPolygon == 6u) {
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}
					else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}

				for (unsigned int i = 1u; i < positions.size() - 1; i++) {
					indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? i + 1u + baseIndex : i + baseIndex); indices.push_back(changeWinding ? i + baseIndex : i + 1u + baseIndex);
				}

				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 7u) {
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}
					else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}

				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 1u + baseIndex); indices.push_back(changeWinding ? 1u + baseIndex : 2u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 2u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 3u + baseIndex);


				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 8u) {

				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}
					else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}

				for (unsigned int i = 0; i < positions.size() - 2; i = i + 2) {
					indices.push_back(0u + i + baseIndex); indices.push_back(changeWinding ? 1u + i + baseIndex : 2u + i + baseIndex); indices.push_back(changeWinding ? 2u + i + baseIndex : 1u + i + baseIndex);
					indices.push_back(1u + i + baseIndex); indices.push_back(changeWinding ? 3u + i + baseIndex : 2u + i + baseIndex); indices.push_back(changeWinding ? 2u + i + baseIndex : 3u + i + baseIndex);
				}

				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 9u) {

				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < 8; i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}
					else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}


				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 1u + baseIndex); indices.push_back(changeWinding ? 1u + baseIndex : 2u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 2u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 3u + baseIndex);

				indices.push_back(0u + baseIndex);  indices.push_back(changeWinding ? 7u + baseIndex : 6u + baseIndex); indices.push_back(changeWinding ? 6u + baseIndex : 7u + baseIndex);
				indices.push_back(0u + baseIndex);  indices.push_back(changeWinding ? 6u + baseIndex : 5u + baseIndex); indices.push_back(changeWinding ? 5u + baseIndex : 6u + baseIndex);

				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 5u + baseIndex : 4u + baseIndex); indices.push_back(changeWinding ? 4u + baseIndex : 5u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 4u + baseIndex : 3u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 4u + baseIndex);


				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}

		}
	}
	m_buildings.push_back(Shape(vertices, indices, 8u));
	m_buildings.back().createBoundingBox();
	m_buildings.back().setVec4Attribute(shapeColor, 0u, 3u);
}

void block_convert(float &newx, float &newy, float x, float y) {
	int tempx, tempy;

	tempx = (int)((2000.0f / 2.0 + x) / 40.0f);
	tempy = (int)((2000.0f / 2.0 + y) / 40.0f);
	newx = -2000.0f / 2.0 + tempx * 40.0f + 40.0f / 2.0;
	newy = -2000.0f / 2.0 + tempy * 40.0f + 40.0f / 2.0;
}

void Scene::loadMiniMap(const char* filename) {
	FILE *f;
	char buf[256];
	char filepath[256];
	strncpy(filepath, filename, 256);

	if ((f = fopen(filepath, "r")) == NULL) {
		fprintf(stderr, "Cannot open building file: %s\n", filepath);
		exit(-1);
	}
	float width, height;
	fscanf(f, "%f %f", &width, &height);

	int numTextures;
	unsigned int texId;
	fscanf(f, "%d", &numTextures);
	for (int i = 0; i < numTextures; i++) {
		fscanf(f, "%s %d", buf, &texId);
	}

	int noOfBuildings, btype;
	float bx1, by1, bx2, by2;
	char fn[256];
	fscanf(f, "%d", &noOfBuildings);
	for (int i = 0; i < noOfBuildings; i++) {
		fscanf(f, "%f %f %f %f %d %d %s", &bx1, &by1, &bx2, &by2, &btype, &texId, fn);
		float _bx1, _by1, _bx2, _by2;
		block_convert(_bx1, _by1, bx1, by1);
		block_convert(_bx2, _by2, bx2, by2);
		m_buildingsMiniMap.push_back({ _bx1 + (_bx2 - _bx1) * 0.5f, -(_by1 + (_by2 - _by1) * 0.5f),  (_bx2 - _bx1) * 0.5f, (_by2 - _by1) * 0.5f });
	}
}

void Scene::loadFont() {
	m_characterSet.frame = 0u;
	m_characterSet.characters[46] = { {0, 0}, {15u, 15u}, {0.625f, 0.625f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[63] = { {0, 0}, {15u, 15u}, {0.742f, 0.875f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[35] = { {0, 0}, {15u, 15u}, {0.25f, 0.5f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[32] = { {0, 0}, {15u, 15u}, {0.875f, 0.875f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[48] = { {0, 0}, {15u, 15u}, {0.875f, 0.625f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };

	m_characterSet.characters[49] = { {0, 0}, {15u, 15u}, {0.0f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[50] = { {0, 0}, {15u, 15u}, {0.117f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[51] = { {0, 0}, {15u, 15u}, {0.25f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[52] = { {0, 0}, {15u, 15u}, {0.367f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[53] = { {0, 0}, {15u, 15u}, {0.5f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[54] = { {0, 0}, {15u, 15u}, {0.625f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[55] = { {0, 0}, {15u, 15u}, {0.742f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[56] = { {0, 0}, {15u, 15u}, {0.875f, 0.75f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };

	m_characterSet.characters[57] = { {0, 0}, {15u, 15u}, {0.0f, 0.875f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[58] = { {0, 0}, {15u, 15u}, {0.125f, 0.875f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[65] = { {0, 0}, {15u, 15u}, {0.0f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[66] = { {0, 0}, {15u, 15u}, {0.117f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[67] = { {0, 0}, {15u, 15u}, {0.25f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[68] = { {0, 0}, {15u, 15u}, {0.367f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[69] = { {0, 0}, {15u, 15u}, {0.5f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[70] = { {0, 0}, {15u, 15u}, {0.625f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[71] = { {0, 0}, {15u, 15u}, {0.742f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[72] = { {0, 0}, {15u, 15u}, {0.875f, 0.0f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };

	m_characterSet.characters[73] = { {0, 0}, {15u, 15u}, {0.0f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[74] = { {0, 0}, {15u, 15u}, {0.117f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[75] = { {0, 0}, {15u, 15u}, {0.25f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[76] = { {0, 0}, {15u, 15u}, {0.367f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[77] = { {0, 0}, {15u, 15u}, {0.5f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[78] = { {0, 0}, {15u, 15u}, {0.625f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[79] = { {0, 0}, {15u, 15u}, {0.742f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[80] = { {0, 0}, {15u, 15u}, {0.875f, 0.125f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };

	m_characterSet.characters[81] = { {0, 0}, {15u, 15u}, {0.0f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[82] = { {0, 0}, {15u, 15u}, {0.117f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[83] = { {0, 0}, {15u, 15u}, {0.25f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[84] = { {0, 0}, {15u, 15u}, {0.367f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[85] = { {0, 0}, {15u, 15u}, {0.5f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[86] = { {0, 0}, {15u, 15u}, {0.625f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[87] = { {0, 0}, {15u, 15u}, {0.742f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[88] = { {0, 0}, {15u, 15u}, {0.875f, 0.25f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };

	m_characterSet.characters[89] = { {0, 0}, {15u, 15u}, {0.0f, 0.375f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };
	m_characterSet.characters[90] = { {0, 0}, {15u, 15u}, {0.117f, 0.375f}, {(float)10 / (float)128, (float)15 / (float)128}, 15u };

	Spritesheet::CreateSpritesheet(Texture::LoadFromFile("res/textures/font.tga", false), 128u, 128u, 1u, m_characterSet.spriteSheet);
}