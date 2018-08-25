#pragma once
#include <stdlib.h>  

class Random
{
public:
	static void SetSeed(int seed) { srand(seed); };

	static float Range(float min, float max)
	{
		float r = (float)rand() / (float)RAND_MAX;
		return min + r * (max - min);
	}

	static float Range(int min, int max)
	{
		return  min + (rand() % (int)(max - min + 1));
	}

// return random value in [0, 1]
	static float Value()
	{
		return (double)rand() / (RAND_MAX);
	}
};