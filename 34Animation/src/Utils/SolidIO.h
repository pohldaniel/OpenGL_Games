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
	static const int MAX_JOINTS = 50;

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

	struct AnimationFrameJointInfo{
		Vector3f position;
		float twist;
		float twist2;
		bool onground;
	};

	struct AnimationFrame{
		std::vector<AnimationFrameJointInfo> joints;
		Vector3f forward;
		int label;
		Vector3f weapontarget;
		float speed;
	};

	enum anim_attack_type {
		neutral,
		normalattack,
		reversed,
		reversal
	};

	enum anim_height_type {
		lowheight,
		middleheight,
		highheight
	};

	struct Animation {
		int numFrames;
		int numJoints;
		std::vector<AnimationFrame> frames;

		anim_height_type height;
		anim_attack_type attack;
		Vector3f offset;
		void print();
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

	enum animation_type{
		runanim,
		bounceidleanim,
		stopanim,
		jumpupanim,
		jumpdownanim,
		landanim,
		landhardanim,
		climbanim,
		hanganim,
		spinkickanim,
		getupfromfrontanim,
		getupfrombackanim,
		crouchanim,
		sneakanim,
		rollanim,
		flipanim,
		frontflipanim,
		spinkickreversedanim,
		spinkickreversalanim,
		lowkickanim,
		sweepanim,
		sweepreversedanim,
		sweepreversalanim,
		rabbitkickanim,
		rabbitkickreversedanim,
		rabbitkickreversalanim,
		upunchanim,
		staggerbackhighanim,
		upunchreversedanim,
		upunchreversalanim,
		hurtidleanim,
		backhandspringanim,
		fightidleanim,
		walkanim,
		fightsidestep,
		killanim,
		sneakattackanim,
		sneakattackedanim,
		drawrightanim,
		knifeslashstartanim,
		crouchdrawrightanim,
		crouchstabanim,
		knifefollowanim,
		knifefollowedanim,
		knifethrowanim,
		removeknifeanim,
		crouchremoveknifeanim,
		jumpreversedanim,
		jumpreversalanim,
		staggerbackhardanim,
		dropkickanim,
		winduppunchanim,
		winduppunchblockedanim,
		blockhighleftanim,
		blockhighleftstrikeanim,
		backflipanim,
		walljumpbackanim,
		rightflipanim,
		walljumprightanim,
		walljumpfrontanim,
		leftflipanim,
		walljumpleftanim,
		walljumprightkickanim,
		walljumpleftkickanim,
		knifefightidleanim,
		knifesneakattackanim,
		knifesneakattackedanim,
		swordfightidleanim,
		drawleftanim,
		swordslashanim,
		swordgroundstabanim,
		dodgebackanim,
		swordsneakattackanim,
		swordsneakattackedanim,
		swordslashreversedanim,
		swordslashreversalanim,
		knifeslashreversedanim,
		knifeslashreversalanim,
		swordfightidlebothanim,
		swordslashparryanim,
		swordslashparriedanim,
		wolfidle,
		wolfcrouchanim,
		wolflandanim,
		wolflandhardanim,
		wolfrunanim,
		wolfrunninganim,
		rabbitrunninganim,
		wolfstopanim,
		rabbittackleanim,
		rabbittacklinganim,
		rabbittackledbackanim,
		rabbittackledfrontanim,
		wolfslapanim,
		staffhitanim,
		staffgroundsmashanim,
		staffspinhitanim,
		staffhitreversedanim,
		staffhitreversalanim,
		staffspinhitreversedanim,
		staffspinhitreversalanim,
		sitanim,
		sleepanim,
		talkidleanim,
		sitwallanim,
		dead1anim,
		dead2anim,
		dead3anim,
		dead4anim,
		tempanim,
		animation_count
	};

	enum animation_bit_offsets {
		o_ab_idle,
		o_ab_sit,
		o_ab_sleep,
		o_ab_crouch,
		o_ab_run,
		o_ab_stop,
		o_ab_land,
		o_ab_landhard,
		o_ab_flip,
		o_ab_walljump
	};

	enum animation_bits_def{
		ab_idle = 1 << o_ab_idle,
		ab_sit = 1 << o_ab_sit,
		ab_sleep = 1 << o_ab_sleep,
		ab_crouch = 1 << o_ab_crouch,
		ab_run = 1 << o_ab_run,
		ab_stop = 1 << o_ab_stop,
		ab_land = 1 << o_ab_land,
		ab_landhard = 1 << o_ab_landhard,
		ab_flip = 1 << o_ab_flip,
		ab_walljump = 1 << o_ab_walljump
	};

	static int animation_bits[animation_count] = {
	   ab_run,
	   ab_idle,
	   ab_stop ,
	   0,
	   0,
	   ab_land,
	   ab_landhard,
	   0,
	   0,
	   0,
	   0,
	   0,
	   ab_crouch,
	   0,
	   0,
	   ab_flip,
	   ab_flip,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   ab_idle,
	   0,
	   ab_idle,
	   0,
	   ab_idle,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   ab_flip,
	   ab_walljump,
	   ab_walljump,
	   ab_flip,
	   ab_walljump,
	   ab_flip,
	   ab_walljump,
	   ab_flip,
	   ab_flip,
	   ab_idle,
	   0,
	   0,
	   ab_idle,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   ab_idle,
	   0,
	   0,
	   ab_idle,
	   ab_crouch,
	   ab_land,
	   ab_landhard,
	   ab_run,
	   ab_run,
	   ab_run,
	   ab_stop,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   ab_idle | ab_sit,
	   ab_idle | ab_sleep,
	   ab_idle,
	   ab_sit,
	   ab_sleep,
	   ab_sleep,
	   ab_sleep,
	   ab_sleep,
	   0
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
		void fromBufferMap(const std::vector<Vertex>& bufferMap, std::vector<float>& vertexBufferOut);
		void loadAnimation(const char* filename, anim_height_type aheight, anim_attack_type aattack, Animation& animation);

		static std::array<float, 3> RotatePoint(std::array<float, 3> point, float xang, float yang, float zang);		
		static std::array<float, 3> ScalePoint(std::array<float, 3> point, float scaleX, float scaleY, float scaleZ);

		private:
			
		bool getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, ComparerUv>& uvToOutIndex, short& result);	
		bool getSimilarVertexIndex(Vertex& packed, std::map<Vertex, short, Comparer>& uvToOutIndex, short & result);		
	};

	struct MdlIO {
		
		void mdlToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath);
		void mdlToBuffer(const char* path, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float,4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<std::vector<GeometryDesc>>& geomDescs, std::vector<MeshBone>& bones, BoundingBox& boundingBox);
		void mdlToBuffer(const char* path, std::array<float,3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float,4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<std::vector<GeometryDesc>>& geomDescs, std::vector<MeshBone>& bones, BoundingBox& boundingBox);
	};

}