#include <engine/Vector.h>
#include "Rasterizer.h"


void Rasterizer::supercover_line(int x, int y, int x2, int y2, std::vector<std::array<int, 2>>& points) {
	int dx = x2 - x, dy = y2 - y;
	int nx = abs(dx), ny = abs(dy);
	int sign_x = dx > 0 ? 1 : -1, sign_y = dy > 0 ? 1 : -1;


	std::array<int, 2> p = { x, y };
	points.push_back(p);

	for (int ix = 0, iy = 0; ix < nx || iy < ny;) {
		int decision = (1 + 2 * ix) * ny - (1 + 2 * iy) * nx;
		if (decision == 0) {
			// next step is diagonal
			p[0] += sign_x;
			p[1] += sign_y;
			ix = ix + 1;
			iy = iy + 1;
		}
		else if (decision < 0) {
			// next step is horizontal
			p[0] += sign_x;
			ix = ix + 1;;
		}
		else {
			// next step is vertical
			p[1] += sign_y;
			iy = iy + 1;
		}
		points.push_back(p);
	}
}

void Rasterizer::linev5(int x1, int y1, int x2, int y2, std::vector<std::array<int, 2>>& points) {
	int dx = x2 - x1,
		dy = y2 - y1;

	if (dx || dy) {
		if (abs(dx) >= abs(dy)) {
			float y = y1 + 0.5;
			float dly = float(dy) / float(dx);
			if (dx > 0)
				for (int x = x1; x <= x2; x++) {
					//s.Plot(x, unsigned(floor(y)), colour);
					points.push_back({ int(x), int(floor(y)) });
					y += dly;
				}
			else
				for (int x = x1; x >= int(x2); x--) {
					//s.Plot(x, unsigned(floor(y)), colour);
					points.push_back({ int(x), int(floor(y)) });
					y -= dly;
				}
		}
		else {
			float x = x1 + 0.5;
			float dlx = float(dx) / float(dy);
			if (dy > 0)
				for (int y = y1; y <= y2; y++) {
					//s.Plot(unsigned(floor(x)), y, colour);
					points.push_back({ int(floor(x)), int(y) });
					x += dlx;
				}
			else
				for (int y = y1; y >= int(y2); y--) {
					//s.Plot(unsigned(floor(x)), y, colour);
					points.push_back({ int(floor(x)), int(y) });
					x -= dlx;
				}
		}
	}
}

void Rasterizer::triangle(std::array<int, 2> t0, std::array<int, 2> t1, std::array<int, 2> t2, std::vector<std::array<int, 2>>& points) {
	linev5(t0[0], t0[1], t1[0], t1[1], points);
	linev5(t1[0], t1[1], t2[0], t2[1], points);
	linev5(t2[0], t2[1], t0[0], t0[1], points);
}


void Rasterizer::fillFlatSideTriangleInt(std::array<int, 2>  v1, std::array<int, 2> v2, std::array<int, 2>  v3, std::vector<std::array<int, 2>>& points) {
	std::array<int, 2> vTmp1 = { v1[0], v1[1] };
	std::array<int, 2> vTmp2 = { v1[0], v1[1] };

	bool changed1 = false;
	bool changed2 = false;

	int dx1 = abs(v2[0] - v1[0]);
	int dy1 = abs(v2[1] - v1[1]);

	int dx2 = abs(v3[0] - v1[0]);
	int dy2 = abs(v3[1] - v1[1]);

	if (dx1 == 0.0f) {
		return;
	}

	if (dy1 == 0.0f) {
		return;
	}

	if (dx2 == 0.0f) {
		return;
	}

	if (dy2 == 0.0f) {
		return;
	}

	int signx1 = Math::Sgn(v2[0] - v1[0]);
	int signx2 = Math::Sgn(v3[0] - v1[0]);

	int signy1 = Math::Sgn(v2[1] - v1[1]);
	int signy2 = Math::Sgn(v3[1] - v1[1]);

	if (dy1 > dx1)
	{   // swap values
		int tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}

	if (dy2 > dx2)
	{   // swap values
		int tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}

	int e1 = 2 * dy1 - dx1;
	int e2 = 2 * dy2 - dx2;

	for (int i = 0; i <= dx1; i++)
	{
		supercover_line(vTmp1[0], vTmp1[1], vTmp2[0], vTmp2[1], points);

		while (e1 >= 0)
		{
			if (changed1)
				vTmp1[0] += signx1;
			else
				vTmp1[1] += signy1;
			e1 = e1 - 2 * dx1;
		}

		if (changed1)
			vTmp1[1] += signy1;
		else
			vTmp1[0] += signx1;

		e1 = e1 + 2 * dy1;

		/* here we rendered the next point on line 1 so follow now line 2
		* until we are on the same y-value as line 1.
		*/
		while (vTmp2[1] != vTmp1[1])
		{
			while (e2 >= 0)
			{
				if (changed2)
					vTmp2[0] += signx2;
				else
					vTmp2[1] += signy2;
				e2 = e2 - 2 * dx2;
			}

			if (changed2)
				vTmp2[1] += signy2;
			else
				vTmp2[0] += signx2;

			e2 = e2 + 2 * dy2;
		}
	}
}

void Rasterizer::rasterTriangle(std::vector<std::array<int, 2>>& initial, std::vector<std::array<int, 2>>& points) {

	std::sort(initial.begin(), initial.end(), [](const std::array<int, 2> &a, const std::array<int, 2> &b) {
		return a[1] > b[1];
	});


	if (initial[1][1] == initial[2][2]) {
		fillFlatSideTriangleInt({ initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, { initial[2][0],initial[2][1] }, points);

	}
	else if (initial[0][1] == initial[1][1]) {
		fillFlatSideTriangleInt({ initial[2][0],initial[2][1] }, { initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, points);

	}
	else {
		std::array<int, 2> vTmp = { (int)(initial[0][0] + ((float)(initial[1][1] - initial[0][1]) / (float)(initial[2][1] - initial[0][1])) * (initial[2][0] - initial[0][0])), initial[1][1] };
		fillFlatSideTriangleInt({ initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, vTmp, points);
		fillFlatSideTriangleInt({ initial[2][0],initial[2][1] }, { initial[1][0],initial[1][1] }, vTmp, points);
	}
}
