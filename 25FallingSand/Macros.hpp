#pragma once


#define PIXEL(surface, x, y) *((uint32_t*)(\
(uint8_t*)surface->pixels + ((y) * surface->pitch) + ((x) * sizeof(uint32_t)))\
)

#define QUOTE(s) #s
