#pragma once

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <array>
#include <map>
#include <iostream>
#include <algorithm>

namespace Utils {

	union UFloat {
		float flt;
		unsigned char c[4];
	};

	union UShort {
		short shrt;
		unsigned char c[2];
	};

	float bytesToFloatLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	float bytesToFloatBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
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
		void solidToBuffer(const char* filename, bool flipVertical, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut);


		

		private:

			bool getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, ComparerUv>& uvToOutIndex, short & result);
			bool getSimilarVertexIndex(std::array<float, 5>& packed, std::map<std::array<float, 5>, short, Comparer>& uvToOutIndex, short & result);

	};

	struct MdlIO {
		

		void mdlToObj(const char* filename, const char* outFileObj, const char* outFileMtl, const char* texturePath);
	};

}