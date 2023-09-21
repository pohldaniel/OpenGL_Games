#pragma once


#define PIXEL(surface, x, y) *((uint32_t*)(\
(uint8_t*)surface->pixels + ((y) * surface->pitch) + ((x) * sizeof(uint32_t)))\
)

#define QUOTE(s) #s

#define UCH_SET_PIXEL(pix_ar, ofs, c_r, c_g, c_b, c_a) \
				pix_ar[ofs + 0] = c_b;\
				pix_ar[ofs + 1] = c_g;\
				pix_ar[ofs + 2] = c_r;\
				pix_ar[ofs + 3] = c_a;

#define REGISTER(material) MATERIALS.insert(MATERIALS.begin() + material.id, &material);