#include "Hex.h"

Hex Hex::hex_add(Hex a, Hex b)
{
	return Hex(a.q() + b.q(), a.r() + b.r(), a.s() + b.s());
}


Hex Hex::hex_subtract(Hex a, Hex b)
{
	return Hex(a.q() - b.q(), a.r() - b.r(), a.s() - b.s());
}


Hex Hex::hex_scale(Hex a, int k)
{
	return Hex(a.q() * k, a.r() * k, a.s() * k);
}


Hex Hex::hex_rotate_left(Hex a)
{
	return Hex(-a.s(), -a.q(), -a.r());
}


Hex Hex::hex_rotate_right(Hex a)
{
	return Hex(-a.r(), -a.s(), -a.q());
}

int Hex::hex_length(Hex hex)
{
	return int((std::abs(hex.q()) + std::abs(hex.r()) + std::abs(hex.s())) / 2);
}


int Hex::hex_distance(Hex a, Hex b)
{
	return hex_length(hex_subtract(a, b));
}

