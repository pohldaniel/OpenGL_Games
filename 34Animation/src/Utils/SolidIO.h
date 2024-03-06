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
#include <iostream>
#include <algorithm>

#include <engine/MeshBone.h>

namespace Utils {

	static const float BONE_SIZE_THRESHOLD = 0.05f;

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
	};

	enum VertexElementSemantic{
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

	struct SolidIO{
		
		struct ComparerUv {
			bool operator()(const std::array<float, 2>& lhs, const std::array<float, 2>& rhs) const {
				return memcmp(lhs.data(), rhs.data(), 2 * sizeof(float)) < 0;
			}
		};

		struct Comparer {
			bool operator()(const std::array<float, 5>& lhs, const std::array<float, 5>& rhs) const {
				return memcmp(lhs.data(), rhs.data(), 5 * sizeof(float)) < 0;
			}
		};

		void solidToObj(const char* filename, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical = true);
		void solidToBuffer(const char* filename, bool flipTextureVertical, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);
		void loadSkeleton(const char* filename, std::vector<float>& modelVertexBuffer);

		static std::array<float, 3> RotatePoint(std::array<float, 3> point, float xang, float yang, float zang);
		//static Vector3f RotatePoint(Vector3f point, float xang, float yang, float zang);
		static std::array<float, 3> ScalePoint(std::array<float, 3> point, float scaleX, float scaleY, float scaleZ);

		private:
			
		bool getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, ComparerUv>& uvToOutIndex, short & result);
		bool getSimilarVertexIndex(std::array<float, 5>& packed, std::map<std::array<float, 5>, short, Comparer>& uvToOutIndex, short & result);
	};

	struct MdlIO {
		
		void mdlToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath);
		void mdlToBuffer(const char* path, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float,4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<std::vector<GeometryDesc>>& geomDescs, std::vector<MeshBone>& bones, BoundingBox& boundingBox);
		void mdlToBuffer(const char* path, std::array<float,3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float,4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<std::vector<GeometryDesc>>& geomDescs, std::vector<MeshBone>& bones, BoundingBox& boundingBox);
	};

}