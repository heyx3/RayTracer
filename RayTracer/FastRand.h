#pragma once

#include "BasicMath.h"
#include <iostream>

//A fast (but not cryptographically-strong) PRNG.
class FastRand
{
public:

	//static int GetMaxValue(void) { return mod - 1; }

	int Seed;

	FastRand(int seed = 12345) : Seed(seed)/*, v(36969)*/ { }

	//int GetGoodRandInt(void) { srand(Seed); Seed = rand(); return Seed; }
	//float GetGoodZeroToOne(void) { const int b = 32000; return ((float)(BasicMath::Abs(GetGoodRandInt()) % b)) / (float)b; }

	inline int GetRandInt(void)
	{
		Seed = (Seed ^ 61) ^ (Seed >> 16);
		Seed += (Seed << 3);
		Seed ^= (Seed >> 4);
		Seed *= 0x27d4eb2d;
		Seed ^= (Seed >> 15);
		return Seed;

		//Seed = 36969*(Seed & 65535) + (Seed >> 16); v = 18000*(v & 65535) + (v >> 16);
		//if (true) return (Seed << 16) + v;
		
		//Seed = (Seed * mult) % mod;
		//return Seed;
	}
	inline float GetZeroToOne(void)
	{
		const int b = 999999;
		return (float)(BasicMath::Abs(GetRandInt()) % b) / (float)b;

		//return ((float)Abs(GetRandInt())) / ((float)GetMaxValue());
	}
	
private:

	//int v;

	//static const int mod, mult;
};