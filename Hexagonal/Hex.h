#pragma once
// Generated code -- CC0 -- No Rights Reserved -- http://www.redblobgames.com/grids/hexagons/
// Based on https://www.redblobgames.com/grids/hexagons/implementation.html

#include <cassert>
#include <cstdlib>

struct Hex { // Axial storage, cube constructor
	const int q_, r_;
	Hex(int q, int r, int s) : q_(q), r_(r) {
		assert(q + r + s == 0);
	}

	virtual ~Hex()
	{
	}

	inline int q() { return q_; }
	inline int r() { return r_; }
	inline int s() { return -q_ - r_; }

	inline int get_q() const { return q_; }
	inline int get_r() const { return r_; }
	inline int get_s() const { return -q_ - r_; }

	static Hex hex_add(Hex a, Hex b);

	static Hex hex_subtract(Hex a, Hex b);


	static Hex hex_scale(Hex a, int k);

	static Hex hex_rotate_left(Hex a);

	static Hex hex_rotate_right(Hex a);

	static int hex_length(Hex hex);


	static int hex_distance(Hex a, Hex b);


public:
	bool operator == (const Hex& h) const { return ((q_ == h.get_q()) && (r_ == h.get_r())); }

};

