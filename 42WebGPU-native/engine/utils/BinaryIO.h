#pragma once

#include <vector>
#include <array>
#include <map>
#include <filesystem>
#include <fstream>

#include <engine/animation/BoneDescription.h>
#include <engine/animation/Animation.h>

namespace Utils {
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

	struct MdlcIO {
		void animatedModelToMdlc(const char* out, const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, uint32_t stride, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& joints, const std::vector<BoneDescription>& boneDescriptions);
		void mdlcModelToBuffer(const char* in, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, uint32_t& stride, std::vector<std::array<float, 4>>& weights, std::vector<std::array<unsigned int, 4>>& joints, std::vector<BoneDescription>& boneDescriptions);

		void animationToAnic(const char* out, const std::string& animationName, const float length, const std::vector<AnimationTrack>& animationTracks);
		void anicToBuffer(const char* in, std::string& animationName, float& length, std::map<std::string, AnimationTrack>& animationTracks);
	};
}