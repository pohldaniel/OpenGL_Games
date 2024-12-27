#pragma once

#include <fstream>
#include <map>



struct MD2Header {

	int ident;		// Magic number. Must be equal to "IDP2"
	int version;	// MD2 version. Must be equal to 8
	int skinwidth;	// Width of the texture
	int skinheight; // Height of the texture
	int framesize;	// Size of one frame in bytes
	int num_skins;	// Number of textures
	int num_xyz;	// Number of vertices
	int num_st;		// Number of texture coordinates
	int num_tris;	// Number of triangles
	int num_glcmds; // Number of OpenGL commands
	int num_frames; // Total number of frames
	int ofs_skins;	// Offset to skin names (each skin name is an unsigned char[64] and are null terminated)
	int ofs_st;		// Offset to s-t texture coordinates
	int ofs_tris;	// Offset to triangles
	int ofs_frames; // Offset to frame data
	int ofs_glcmds; // Offset to OpenGL commands
	int ofs_end;	// Offset to end of file
};

struct MD2TexCoords {
	short 	s;
	short 	t;
};

struct MD2Tris {
	short vi[3];
	short ti[3];
};

struct MD2VertCoords {
	unsigned char v[3];
	unsigned char normi;
};
struct MD2Frame {
	float scale[3];
	float translate[3];
	char name[16];
	MD2VertCoords vc[0];
};


class MD2 {
public:

	MD2();
	~MD2();

	int load(const char *path, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical = true);
};