 // For conditions of distribution and use, see copyright notice in License.txt

#include "RandomTu.h"

static unsigned randomSeed = 1;

void SetRandomSeed(unsigned seed)
{
    randomSeed = seed;
}

unsigned RandomSeed()
{
    return randomSeed;
}

int Rand()
{
    randomSeed = randomSeed * 214013 + 2531011;
    return (randomSeed >> 16) & 32767;
}

float RandStandardNormal()
{
    float val = 0.0f;
    for (int i = 0; i < 12; i++)
        val += Rand() / 32768.0f;
    val -= 6.0f;
    
    // Now val is approximately standard normal distributed
    return val;
}

