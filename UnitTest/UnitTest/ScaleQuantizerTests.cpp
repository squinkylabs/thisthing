
#include "stdafx.h"

#include "ScaleQuantizer.h"



static char quantize_semi(int semi, const char * scale)
{
	// first expand the scale


	char expanded[ ScaleQuantizer::expandedSize];
	const int len = ScaleQuantizer::expandScale(expanded, scale);

	// now quantize it
	return  ScaleQuantizer::quantize_semi_expanded(semi, expanded, len);
}

// test some simple case: ScaleQuantizer::quantize_semi
// single note in scale
static void sq0()
{
	printf("sq0\n");
	{
	char scale[] = {0, -1};
	int q = quantize_semi(0, scale);
	assert(q == 0);
	}

	printf("sq0-b\n");
	{
	char scale2[] = {6, -1};
	int q = quantize_semi(6, scale2);
	assert(q == 6);
	}

	printf("sq0-c\n");
	{
	char scale3[] = {6, 7, 8, 11, -1};
	int q = quantize_semi(7, scale3);
	assert(q == 7);
	q = quantize_semi(6, scale3);
	assert(q == 6);
	q = quantize_semi(8, scale3);
	assert(q == 8);
	q = quantize_semi(11, scale3);
	assert(q == 11);
	}
}


// test  ScaleQuantizer::quantize_semi
// single note note in scale, wrap octaves
void sq1()
{
	printf("sq1\n");
	{
		// out note is "past the end"
		char scale[] = {3, -1};
		int q = quantize_semi(11, scale);
		assert(q == 3+12); // wrap next octave
		}
	printf("sq1-b\n");
	{
		// out note is "before the start"
		char scale[] = {11, -1};
		int q = quantize_semi(3, scale);
		printf("q =%d\n", q);
		assert(q == 11-12);		// wraps under
	}
}

// basic tests for "in between" notes
void sq2()
{
	printf("sq2, a\n");
	char scale[] = {3, 7, 11, -1};
	int q = quantize_semi(4, scale);
	assert(q == 3);		// 4 is close to 3

	printf("sq2, b\n");
	q = quantize_semi(6, scale);
	assert(q == 7);		// 6 is close to 7

}

// basic test for octave wrap around
void sq3()
{
	printf("sq3\n");
	char scale[] = { 1, 2, -1};
	int q = quantize_semi(11, scale);
	assert(q == 1+12);		// 1 is "pretty close" to 11 if you wrap octaves
}


void scale0()
{
	assert(ScaleQuantizer::getNumOctaveScales() > 0);
	for (int i = 0; i < ScaleQuantizer::getNumOctaveScales(); ++i)
	{
		const char * scale = ScaleQuantizer::getOctaveScale(i);
		char expanded[ ScaleQuantizer::expandedSize];
		const int len = ScaleQuantizer::expandScale(expanded, scale);
		assert(len >= 2);
	}
}

void ScaleQuantizerTests()
{
	sq0();
	sq1();
	sq2();
	sq3();

	scale0();
}