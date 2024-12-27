#include <iostream>
#include "Md2.h"

#include "Utils/BinaryIO.h"

MD2::MD2() {

}

MD2::~MD2() {

}

int MD2::load(const char *path, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical) {
	
	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<short, 3>> faces;

	std::vector <std::array<float, 2>> uvCoords;
	std::vector<std::array<short, 3>> uvFaves;

	std::ifstream file(path, std::ios::binary);

	MD2Header header;
	file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));
	 
	std::cout << "Ident: " << header.ident << "  " << MD2_IDENT_VAL << std::endl;
	std::cout << "Version: " << header.version  << std::endl;
	std::cout << "Num St: " << header.num_st << std::endl;
	std::cout << "Offset St: " << header.ofs_st << std::endl;

	char* buffer = new char[header.num_st * sizeof(MD2TexCoords)];
	file.seekg(header.ofs_st, std::ios::beg);
	file.read(buffer, header.num_st * sizeof(MD2TexCoords));

	for (int i = 0; i < header.num_st * sizeof(MD2TexCoords); i = i + 4) {
		Utils::UShort s, t;

		s.c[0] = buffer[i + 0]; s.c[1] = buffer[i + 1];
		t.c[0] = buffer[i + 2]; t.c[1] = buffer[i + 3];

		if(flipVertical)
			uvCoords.push_back({ (float)s.shrt / header.skinwidth , 1.0f - (float)t.shrt / header.skinheight });
		else
			uvCoords.push_back({ (float)s.shrt / header.skinwidth , (float)t.shrt / header.skinheight });
	}
	delete buffer;

	buffer = new char[header.num_tris * sizeof(MD2Tris)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read(buffer, header.num_tris * sizeof(MD2Tris));

	for (int i = 0; i < header.num_tris * sizeof(MD2Tris); i = i + sizeof(MD2Tris)) {
		Utils::UShort value[6];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt , value[1].shrt , value[2].shrt });

		value[3].c[0] = buffer[i + 6]; value[3].c[1] = buffer[i + 7];
		value[4].c[0] = buffer[i + 8]; value[4].c[1] = buffer[i + 9];
		value[5].c[0] = buffer[i + 10]; value[5].c[1] = buffer[i + 11];

		uvFaves.push_back({ value[3].shrt , value[4].shrt , value[5].shrt });
	}
	delete buffer;

	int framesize = sizeof(MD2Frame) + header.num_xyz * sizeof(MD2VertCoords);
	buffer = new char[framesize];

	for (int i = 0, j = 0; i < 1; i++) {
		file.seekg(header.ofs_frames + i * framesize, std::ios::beg);
		file.read(buffer, framesize);

		Utils::UFloat scale[3];

		scale[0].c[0] = buffer[i + 0]; scale[0].c[1] = buffer[i + 1]; scale[0].c[2] = buffer[i + 2]; scale[0].c[3] = buffer[i + 3];
		scale[1].c[0] = buffer[i + 4]; scale[1].c[1] = buffer[i + 5]; scale[1].c[2] = buffer[i + 6]; scale[1].c[3] = buffer[i + 7];
		scale[2].c[0] = buffer[i + 8]; scale[2].c[1] = buffer[i + 9]; scale[2].c[2] = buffer[i + 10]; scale[2].c[3] = buffer[i + 11];

		Utils::UFloat translate[3];

		translate[0].c[0] = buffer[i + 12]; translate[0].c[1] = buffer[i + 13]; translate[0].c[2] = buffer[i + 14]; translate[0].c[3] = buffer[i + 15];
		translate[1].c[0] = buffer[i + 16]; translate[1].c[1] = buffer[i + 17]; translate[1].c[2] = buffer[i + 18]; translate[1].c[3] = buffer[i + 19];
		translate[2].c[0] = buffer[i + 20]; translate[2].c[1] = buffer[i + 21]; translate[2].c[2] = buffer[i + 22]; translate[2].c[3] = buffer[i + 23];

		std::string name;
		name.resize(16);

		std::copy(buffer + i + 24, buffer + i + 40, name.begin());
		std::cout << "Name: " << name << std::endl;
		for (int k = 0; k < header.num_xyz; k++) {
			Utils::UShort vertexS[3];
			vertexS[0].c[0] = buffer[40 + k * 4 + 0]; vertexS[0].c[1] = 0;
			vertexS[1].c[0] = buffer[40 + k * 4 + 1]; vertexS[1].c[1] = 0;
			vertexS[2].c[0] = buffer[40 + k * 4 + 2]; vertexS[2].c[1] = 0;
			
			
			Utils::UFloat vertex[3];
					
			vertex[0].flt = buffer[40 + k * 4 + 0];
			vertex[1].flt = buffer[40 + k * 4 + 1];
			vertex[2].flt = buffer[40 + k * 4 + 2];

			vertex[0].flt = vertexS[0].shrt;
			vertex[1].flt = vertexS[1].shrt;
			vertex[2].flt = vertexS[2].shrt;
			
			positions.push_back({ vertex[0].flt * scale[0].flt + translate[0].flt, vertex[2].flt * scale[2].flt + translate[2].flt, vertex[1].flt * scale[1].flt + translate[1].flt });
		}
	}

	file.close();

	std::filesystem::path mltPath(outFileMtl);
	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";
	fileOut << "mtllib " << mltPath.filename().string() << std::endl;

	for (int i = 0; i < positions.size(); i++) {
		fileOut << "v " << positions[i][0] << " " << positions[i][1] << " " << positions[i][2] << std::endl;
	}
	for (int i = 0; i < uvCoords.size(); i++) {
		fileOut << "vt " << uvCoords[i][0] << " " << uvCoords[i][1] << std::endl;
	}
	fileOut << "usemtl Material\n";
	for (int i = 0; i < faces.size(); i++) {		
		fileOut << "f " << faces[i][0] + 1 << "/" << uvFaves[i][0] + 1 << " " << faces[i][1] + 1 << "/" << uvFaves[i][1] + 1 << " " << faces[i][2] + 1 << "/" << uvFaves[i][2] + 1 << std::endl;
	}

	fileOut.close();

	fileOut.open(outFileMtl);
	fileOut << std::setprecision(6) << std::fixed;
	fileOut << "newmtl Material\n";
	fileOut << "Ns 10.000000\n";

	fileOut << "Ka 0.000000 0.000000 0.000000\n";
	fileOut << "Kd 1.000000 1.000000 1.000000\n";
	fileOut << "Ks 0.000000 0.000000 0.000000\n";
	fileOut << "Ni 1.000000\n";
	fileOut << "d 1.000000\n";
	fileOut << "illum 1\n";
	std::string absPath = std::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '/', '\\');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();

	return 0;
}