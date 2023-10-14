#pragma once

#include <engine/Vector.h>

namespace Math {

	//cart -> iso
	//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
	//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
	inline void cartesianToIsometric(float & x, float & y, float cellWidth = 32.0f, float cellHeight = 32.0f) {
		float cartX = x * cellWidth;
		float cartY = y * cellHeight;
		x = (cartX - cartY);
		y = (cartX + cartY) * 0.5f;
	}

	//iso -> cart
	//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
	//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
	//cartX = (isoX  + cartY * CELL_HEIGHT) / CELL_WIDTH
	//cartY = (-2.0 * isoY  - cartX * CELL_WIDTH) / CELL_HEIGHT
	//
	//cartX = (isoX - 2.0 * isoY - cartX * CELL_WIDTH) / CELL_WIDTH
	//cartX =  (0.5f * isoX - isoY) /CELL_WIDTH 
	//
	//cartY = (-2.0 * isoY  - isoX  - cartY * CELL_HEIGHT) / CELL_HEIGHT
	//cartY =-(0.5f * isoX + isoY) / CELL_HEIGHT
	inline void isometricToCartesian(float& x, float& y, float cellWidth = 32.0f, float cellHeight = 32.0f) {
		float isoX = x;
		float isoY = y;

		x = (0.5f * isoX - isoY) / cellWidth;
		y = -(0.5f * isoX + isoY) / cellHeight;
	}

	inline void isometricToCartesian(float x, float y, int& row, int& col, float cellWidth = 32.0f, float cellHeight = 32.0f) {
		float isoX = x;
		float isoY = y;

		x = (0.5f * isoX - isoY) / cellWidth;
		y = -(0.5f * isoX + isoY) / cellHeight;

		row = static_cast<int>(std::roundf(x));
		col = static_cast<int>(std::roundf(y)) + 1;
	}

	inline void isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight, int min, int max) {
		float isoX = x;
		float isoY = y;

		x = (0.5f * isoX - isoY) / cellWidth;
		y = -(0.5f * isoX + isoY) / cellHeight;

		row = static_cast<int>(std::roundf(x));
		col = static_cast<int>(std::roundf(y)) + 1;

		row = Math::Clamp(row, min, max);
		col = Math::Clamp(col, min, max);
	}

	inline void isometricToRow(float x, float y, int& row, float cellWidth = 32.0f) {
		float isoX = x;
		float isoY = y;

		x = (0.5f * isoX - isoY) / cellWidth;
		row = static_cast<int>(std::roundf(x));
	}

	inline void isometricToRow(float x, float y, int& row, float cellWidth, int min, int max) {
		float isoX = x;
		float isoY = y;

		x = (0.5f * isoX - isoY) / cellWidth;
		row = static_cast<int>(std::roundf(x));

		row = Math::Clamp(row, min, max);
	}

	inline void isometricToCol(float x, float y, int& col, float cellHeight = 32.0f) {
		float isoX = x;
		float isoY = y;

		y = -(0.5f * isoX + isoY) / cellHeight;
		col = static_cast<int>(std::roundf(y)) + 1;
	}

	inline void isometricToCol(float x, float y, int& col, float cellHeight, int min, int max) {
		float isoX = x;
		float isoY = y;

		y = -(0.5f * isoX + isoY) / cellHeight;
		col = static_cast<int>(std::roundf(y)) + 1;

		col = Math::Clamp(col, min, max);
	}

}