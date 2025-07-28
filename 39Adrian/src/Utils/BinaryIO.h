#pragma once

// We haven't checked which filesystem to include yet
#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

// Check for feature test macro for <filesystem>
#   if defined(__cpp_lib_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0

// Check for feature test macro for <experimental/filesystem>
#   elif defined(__cpp_lib_experimental_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// We can't check if headers exist...
// Let's assume experimental to be safe
#   elif !defined(__has_include)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Check if the header "<filesystem>" exists
#   elif __has_include(<filesystem>)

// If we're compiling on Visual Studio and are not compiling with C++17, we need to use experimental
#       ifdef _MSC_VER

// Check and include header that defines "_HAS_CXX17"
#           if __has_include(<yvals_core.h>)
#               include <yvals_core.h>

// Check for enabled C++17 support
#               if defined(_HAS_CXX17) && _HAS_CXX17
// We're using C++17, so let's use the normal version
#                   define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#               endif
#           endif

// If the marco isn't defined yet, that means any of the other VS specific checks failed, so we need to use experimental
#           ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
#               define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#           endif

// Not on Visual Studio. Let's use the normal version
#       else // #ifdef _MSC_VER
#           define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#       endif

// Check if the header "<filesystem>" exists
#   elif __has_include(<experimental/filesystem>)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Fail if neither header is available with a nice error message
#   else
#       error Could not find system header "<filesystem>" or "<experimental/filesystem>"
#   endif

// We priously determined that we need the exprimental version
#   if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
// Include it
#       include <experimental/filesystem>

// We need the alias from std::experimental::filesystem to std::filesystem
namespace std {
	namespace filesystem = experimental::filesystem;
}

// We have a decent compiler and can use the normal version
#   else
// Include it
#       include <filesystem>
#   endif

#endif // #ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

#include <fstream>
#include <iomanip>
#include <array>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>

#include <engine/MeshBone.h>
#include <engine/MeshSequence.h>
#include <engine/BoundingBox.h>
#include <Navigation/NavigationMesh.h>

#define	MD2_IDENT_VAL	(('I'<<0) | ('D' << 8) | ('P' << 16) | ('2' << 24))
#define	MD2_VERSION_VAL	8

namespace Utils {

	static const float BONE_SIZE_THRESHOLD = 0.05f;
	static const int MAX_JOINTS = 50;

	enum anim_attack_type {
		neutral,
		normalattack,
		reversed,
		reversal,
		attack_type_count
	};

	enum anim_height_type {
		lowheight,
		middleheight,
		highheight,
		height_type_count
	};

	enum bodypart {
		head,
		neck,
		leftshoulder,
		leftelbow,
		leftwrist,
		lefthand,
		rightshoulder,
		rightelbow,
		rightwrist,
		righthand,
		abdomen,
		lefthip,
		righthip,
		groin,
		leftknee,
		leftankle,
		leftfoot,
		rightknee,
		rightankle,
		rightfoot
	};


	struct Joint{
		Vector3f position;		
		float length;
		float mass;
		bool hasparent;
		bool locked;
		int modelnum;
		bool visible;
		bool sametwist;
		int label;
		int hasgun;
		bool lower;
		int parentIndex;
		void print();

		Vector3f velocity;
	};

	struct Muscle {
		float length;
        float targetlength;
        float minlength;
        float maxlength;
        float strength;
		int type;
		int numvertices;
		std::vector<int> vertexIndices;
		bool visible;
		int parentIndex1;
		int parentIndex2;
		Vector3f initialPosition;
		void print();

		float rotate1, rotate2, rotate3;
		Matrix4f m_modelMatrixInitial;

		float oldrotate1, oldrotate2, oldrotate3;
		float newrotate1, newrotate2, newrotate3;
	};

	struct Skeleton {
		std::vector<Joint> m_joints;
		std::vector<Muscle> m_muscles;
		int forwardJoint1;
		int forwardJoint2;
		int forwardJoint3;
		int lowForwardJoint1;
		int lowForwardJoint2;
		int lowForwardJoint3;
		void print();

		void findForwards();
		void findRotationMuscle(int which, int animation);
		Vector3f specialforward[5];
		Vector3f forward;
		Vector3f lowForward;

		int jointlabels[MAX_JOINTS];
		int free = 0;
		bool spinny = false;
		Joint& joint(int bodypart);
	};

	struct AnimationFrameJointInfo {
		Vector3f position;
		float twist;
		float twist2;
		bool onground;
	};

	struct AnimationFrame {
		std::vector<AnimationFrameJointInfo> joints;
		Vector3f forward;
		int label;
		Vector3f weapontarget;
		float speed;
	};

	struct Animation {
		int numFrames;
		int numJoints;
		std::vector<AnimationFrame> frames;

		Utils::anim_height_type height;
		Utils::anim_attack_type attack;
		Vector3f offset;
		void print();
	};

	enum VertexElementSemantic {
		SEM_POSITION = 0,
		SEM_NORMAL,
		SEM_BINORMAL,
		SEM_TANGENT,
		SEM_TEXCOORD,
		SEM_COLOR,
		SEM_BLENDWEIGHTS,
		SEM_BLENDINDICES,
		SEM_OBJECTINDEX,
		MAX_VERTEX_ELEMENT_SEMANTICS
	};

	struct GeometryDesc {
		float lodDistance;
		unsigned vbRef;
		unsigned ibRef;
		unsigned drawStart;
		unsigned drawCount;
	};

	const unsigned ELEMENT_TYPESIZES[] = {
		sizeof(int),
		sizeof(float),
		2 * sizeof(float),
		3 * sizeof(float),
		4 * sizeof(float),
		sizeof(unsigned),
		sizeof(unsigned)
	};

	union UFloat {
		float flt;
		unsigned char c[4];
	};

	union UShort {
		short shrt;
		unsigned char c[2];
	};

	union UBool {
		bool bl;
		unsigned char c[1];
	};

	union UInt {
		int nt;
		unsigned char c[4];
	};

	union UUint {
		unsigned int unt;
		unsigned char c[4];
	};

	float bytesToFloatLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	float bytesToFloatBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	int bytesToIntLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	int bytesToIntBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	unsigned int bytesToUIntLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	unsigned int bytesToUIntBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	short bytesToShortLE(unsigned char b0, unsigned char b1);
	short bytesToShortBE(unsigned char b0, unsigned char b1);
	bool bytesToBool(unsigned char b0);

	static std::array<float, 3> RotatePoint(std::array<float, 3> point, float xang, float yang, float zang);
	static std::array<float, 3> ScalePoint(std::array<float, 3> point, float scaleX, float scaleY, float scaleZ);

	struct SolidIO{
		
		struct Vertex {
			std::array<float, 5> data;
			unsigned int index;
		};

		struct ComparerUv {
			bool operator()(const std::array<float, 2>& lhs, const std::array<float, 2>& rhs) const {
				return memcmp(lhs.data(), rhs.data(), 2 * sizeof(float)) < 0;
			}
		};

		struct Comparer {
			bool operator()(const Vertex& lhs, const Vertex& rhs)const {
				return memcmp(lhs.data.data(), rhs.data.data(), 5 * sizeof(float)) < 0;
			}
		};


		void solidToObj(const char* filename, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical = true);
		void solidToBuffer(const char* filename, bool flipTextureVertical, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<Vertex>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);
		void loadSkeleton(const char* filename, const std::vector<Vertex>& vertexBufferMap, std::vector<float>& vertexBufferOut, Skeleton& skeleton, std::vector<std::array<float, 4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut);
		void loadAnimation(const char* filename, Utils::anim_height_type height, Utils::anim_attack_type attack, Animation& animation);

	private:
			
		bool getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, ComparerUv>& uvToOutIndex, short& result);	
		bool getSimilarVertexIndex(Vertex& packed, std::map<Vertex, short, Comparer>& vertexToOutIndex, short & result);		
	};

	struct MdlIO {
		
		void mdlToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath);
		void mdlToBuffer(const char* path, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);
		void mdlToBuffer(const char* path, std::array<float, 3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);
		void mdlToBuffer(const char* path, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float,4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<MeshBone>& bones, BoundingBox& boundingBox);
		void mdlToBuffer(const char* path, std::array<float,3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float,4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<MeshBone>& bones, BoundingBox& boundingBox);
	};

	struct VbmIO{

		struct VBM_HEADER{
			unsigned int magic;
			unsigned int size;
			char name[64];
			unsigned int num_attribs;
			unsigned int num_frames;
			unsigned int num_vertices;
			unsigned int num_indices;
			unsigned int index_type;
		};

		struct VBM_ATTRIB_HEADER{
			char name[64];
			unsigned int type;
			unsigned int components;
			unsigned int flags;
		};

		struct VBM_FRAME_HEADER{
			unsigned int first;
			unsigned int count;
			unsigned int flags;
		};

		void vbmToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath);
		void vbmToBuffer(const char* path, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);
	};

//https://formats.kaitai.io/quake2_md2/
	struct MD2IO {
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

		const std::array<float,3> aNormals[162] =
		{
			{ -0.525731f,  0.000000f,  0.850651f }, { -0.442863f,  0.238856f,  0.864188f },
			{ -0.295242f,  0.000000f,  0.955423f }, { -0.309017f,  0.500000f,  0.809017f },
			{ -0.162460f,  0.262866f,  0.951056f }, {  0.000000f,  0.000000f,  1.000000f },
			{  0.000000f,  0.850651f,  0.525731f }, { -0.147621f,  0.716567f,  0.681718f },
			{  0.147621f,  0.716567f,  0.681718f }, {  0.000000f,  0.525731f,  0.850651f },
			{  0.309017f,  0.500000f,  0.809017f }, {  0.525731f,  0.000000f,  0.850651f },
			{  0.295242f,  0.000000f,  0.955423f }, {  0.442863f,  0.238856f,  0.864188f },
			{  0.162460f,  0.262866f,  0.951056f }, { -0.681718f,  0.147621f,  0.716567f },
			{ -0.809017f,  0.309017f,  0.500000f }, { -0.587785f,  0.425325f,  0.688191f },
			{ -0.850651f,  0.525731f,  0.000000f }, { -0.864188f,  0.442863f,  0.238856f },
			{ -0.716567f,  0.681718f,  0.147621f }, { -0.688191f,  0.587785f,  0.425325f },
			{ -0.500000f,  0.809017f,  0.309017f }, { -0.238856f,  0.864188f,  0.442863f },
			{ -0.425325f,  0.688191f,  0.587785f }, { -0.716567f,  0.681718f, -0.147621f },
			{ -0.500000f,  0.809017f, -0.309017f }, { -0.525731f,  0.850651f,  0.000000f },
			{  0.000000f,  0.850651f, -0.525731f }, { -0.238856f,  0.864188f, -0.442863f },
			{  0.000000f,  0.955423f, -0.295242f }, { -0.262866f,  0.951056f, -0.162460f },
			{  0.000000f,  1.000000f,  0.000000f }, {  0.000000f,  0.955423f,  0.295242f },
			{ -0.262866f,  0.951056f,  0.162460f }, {  0.238856f,  0.864188f,  0.442863f },
			{  0.262866f,  0.951056f,  0.162460f }, {  0.500000f,  0.809017f,  0.309017f },
			{  0.238856f,  0.864188f, -0.442863f }, {  0.262866f,  0.951056f, -0.162460f },
			{  0.500000f,  0.809017f, -0.309017f }, {  0.850651f,  0.525731f,  0.000000f },
			{  0.716567f,  0.681718f,  0.147621f }, {  0.716567f,  0.681718f, -0.147621f },
			{  0.525731f,  0.850651f,  0.000000f }, {  0.425325f,  0.688191f,  0.587785f },
			{  0.864188f,  0.442863f,  0.238856f }, {  0.688191f,  0.587785f,  0.425325f },
			{  0.809017f,  0.309017f,  0.500000f }, {  0.681718f,  0.147621f,  0.716567f },
			{  0.587785f,  0.425325f,  0.688191f }, {  0.955423f,  0.295242f,  0.000000f },
			{  1.000000f,  0.000000f,  0.000000f }, {  0.951056f,  0.162460f,  0.262866f },
			{  0.850651f, -0.525731f,  0.000000f }, {  0.955423f, -0.295242f,  0.000000f },
			{  0.864188f, -0.442863f,  0.238856f }, {  0.951056f, -0.162460f,  0.262866f },
			{  0.809017f, -0.309017f,  0.500000f }, {  0.681718f, -0.147621f,  0.716567f },
			{  0.850651f,  0.000000f,  0.525731f }, {  0.864188f,  0.442863f, -0.238856f },
			{  0.809017f,  0.309017f, -0.500000f }, {  0.951056f,  0.162460f, -0.262866f },
			{  0.525731f,  0.000000f, -0.850651f }, {  0.681718f,  0.147621f, -0.716567f },
			{  0.681718f, -0.147621f, -0.716567f }, {  0.850651f,  0.000000f, -0.525731f },
			{  0.809017f, -0.309017f, -0.500000f }, {  0.864188f, -0.442863f, -0.238856f },
			{  0.951056f, -0.162460f, -0.262866f }, {  0.147621f,  0.716567f, -0.681718f },
			{  0.309017f,  0.500000f, -0.809017f }, {  0.425325f,  0.688191f, -0.587785f },
			{  0.442863f,  0.238856f, -0.864188f }, {  0.587785f,  0.425325f, -0.688191f },
			{  0.688191f,  0.587785f, -0.425325f }, { -0.147621f,  0.716567f, -0.681718f },
			{ -0.309017f,  0.500000f, -0.809017f }, {  0.000000f,  0.525731f, -0.850651f },
			{ -0.525731f,  0.000000f, -0.850651f }, { -0.442863f,  0.238856f, -0.864188f },
			{ -0.295242f,  0.000000f, -0.955423f }, { -0.162460f,  0.262866f, -0.951056f },
			{  0.000000f,  0.000000f, -1.000000f }, {  0.295242f,  0.000000f, -0.955423f },
			{  0.162460f,  0.262866f, -0.951056f }, { -0.442863f, -0.238856f, -0.864188f },
			{ -0.309017f, -0.500000f, -0.809017f }, { -0.162460f, -0.262866f, -0.951056f },
			{  0.000000f, -0.850651f, -0.525731f }, { -0.147621f, -0.716567f, -0.681718f },
			{  0.147621f, -0.716567f, -0.681718f }, {  0.000000f, -0.525731f, -0.850651f },
			{  0.309017f, -0.500000f, -0.809017f }, {  0.442863f, -0.238856f, -0.864188f },
			{  0.162460f, -0.262866f, -0.951056f }, {  0.238856f, -0.864188f, -0.442863f },
			{  0.500000f, -0.809017f, -0.309017f }, {  0.425325f, -0.688191f, -0.587785f },
			{  0.716567f, -0.681718f, -0.147621f }, {  0.688191f, -0.587785f, -0.425325f },
			{  0.587785f, -0.425325f, -0.688191f }, {  0.000000f, -0.955423f, -0.295242f },
			{  0.000000f, -1.000000f,  0.000000f }, {  0.262866f, -0.951056f, -0.162460f },
			{  0.000000f, -0.850651f,  0.525731f }, {  0.000000f, -0.955423f,  0.295242f },
			{  0.238856f, -0.864188f,  0.442863f }, {  0.262866f, -0.951056f,  0.162460f },
			{  0.500000f, -0.809017f,  0.309017f }, {  0.716567f, -0.681718f,  0.147621f },
			{  0.525731f, -0.850651f,  0.000000f }, { -0.238856f, -0.864188f, -0.442863f },
			{ -0.500000f, -0.809017f, -0.309017f }, { -0.262866f, -0.951056f, -0.162460f },
			{ -0.850651f, -0.525731f,  0.000000f }, { -0.716567f, -0.681718f, -0.147621f },
			{ -0.716567f, -0.681718f,  0.147621f }, { -0.525731f, -0.850651f,  0.000000f },
			{ -0.500000f, -0.809017f,  0.309017f }, { -0.238856f, -0.864188f,  0.442863f },
			{ -0.262866f, -0.951056f,  0.162460f }, { -0.864188f, -0.442863f,  0.238856f },
			{ -0.809017f, -0.309017f,  0.500000f }, { -0.688191f, -0.587785f,  0.425325f },
			{ -0.681718f, -0.147621f,  0.716567f }, { -0.442863f, -0.238856f,  0.864188f },
			{ -0.587785f, -0.425325f,  0.688191f }, { -0.309017f, -0.500000f,  0.809017f },
			{ -0.147621f, -0.716567f,  0.681718f }, { -0.425325f, -0.688191f,  0.587785f },
			{ -0.162460f, -0.262866f,  0.951056f }, {  0.442863f, -0.238856f,  0.864188f },
			{  0.162460f, -0.262866f,  0.951056f }, {  0.309017f, -0.500000f,  0.809017f },
			{  0.147621f, -0.716567f,  0.681718f }, {  0.000000f, -0.525731f,  0.850651f },
			{  0.425325f, -0.688191f,  0.587785f }, {  0.587785f, -0.425325f,  0.688191f },
			{  0.688191f, -0.587785f,  0.425325f }, { -0.955423f,  0.295242f,  0.000000f },
			{ -0.951056f,  0.162460f,  0.262866f }, { -1.000000f,  0.000000f,  0.000000f },
			{ -0.850651f,  0.000000f,  0.525731f }, { -0.955423f, -0.295242f,  0.000000f },
			{ -0.951056f, -0.162460f,  0.262866f }, { -0.864188f,  0.442863f, -0.238856f },
			{ -0.951056f,  0.162460f, -0.262866f }, { -0.809017f,  0.309017f, -0.500000f },
			{ -0.864188f, -0.442863f, -0.238856f }, { -0.951056f, -0.162460f, -0.262866f },
			{ -0.809017f, -0.309017f, -0.500000f }, { -0.681718f,  0.147621f, -0.716567f },
			{ -0.681718f, -0.147621f, -0.716567f }, { -0.850651f,  0.000000f, -0.525731f },
			{ -0.688191f,  0.587785f, -0.425325f }, { -0.587785f,  0.425325f, -0.688191f },
			{ -0.425325f,  0.688191f, -0.587785f }, { -0.425325f, -0.688191f, -0.587785f },
			{ -0.587785f, -0.425325f, -0.688191f }, { -0.688191f, -0.587785f, -0.425325f }
		};

		struct Vertex {
			float x, y, z;
			float n1, n2, n3;

			static Vertex Lerp(const Vertex &a, const Vertex &b, float begin, float end) {
				return { a.x * end + begin * b.x, a.y * end + begin * b.y, a.z * end + begin * b.z,
					     a.n1 * end + begin * b.n1, a.n2 * end + begin * b.n2, a.n3 * end + begin * b.n3 };
			}
		};

		struct Frame {
			BoundingBox boundingBox;
			std::vector<Vertex> vertices;
		};

		struct Animation {
		public:
			char name[16];
			std::vector<Frame> frames;
			float fps;
		};

		void md2ToObj(const char *path, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical = true, int frame = 0);
		void md2ToBuffer(const char* path, bool flipVertical , int frame, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);
		void md2ToSequence(const char* path, bool flipVertical, std::array<float, 3> eulerAngle, std::array<float, 3> scale, MeshSequence& sequenceOut);
		int loadMd2(const char* path, bool flipVertical, std::array<float, 3> positionOffset, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<Vertex>& vertexBufferOut, std::vector<float>& texelsOut, std::vector<unsigned int>& indexBufferOut, std::vector<Animation>& animations);

	private:

		std::map<int, std::vector<int>> m_vertexCache;
		int addVertex(int hash, const float *pVertex, int stride, std::vector<float>& vertexBufferOut);
		void convert(const std::vector<float>& in, std::vector<Vertex>& out);
		void extractTexels(const std::vector<float>& in, std::vector<float>& out);
		void calcAABB(const std::vector<float>& vertices, BoundingBox& boundingBox);
	};

	struct NavIO {
		void writeNavigationMap(const std::string path, int numX, int numZ, const BoundingBox& boundingBox, const std::unordered_map<int, Buffer>& data);
		void readNavigationMap(const std::string path, int& numX, int& numZ, BoundingBox& boundingBox, std::unordered_map<int, Buffer>& data);
	};
}