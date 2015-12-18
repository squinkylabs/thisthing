
#ifndef INCLUDE_RANDOM
#define INCLUDE_RANDOM

#include <stdint.h>

#define M 2147483647
#define A 16807
#define Q ( M / A )
#define R ( M % A )

/*
 Simple 32-bit pseudo random number generator.s
 */
class Random
{
public:
	Random() : seed(1) {}
	int32_t get()
	{
		seed = A * (seed % Q) - R * (seed / Q);
		if (seed <= 0)
		{
			seed += M;
		}
		return seed;
	}
private:
	int32_t seed;
};
#endif