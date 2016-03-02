
#include "stdafx.h"

#include "ScaleQuantizer.h"

// test some simple case: ScaleQuantizer::quantize_semi
// single note in scale
static void sq0()
{
	{
	char scale[] = {0, -1};
	int q = ScaleQuantizer::quantize_semi(0, scale);
	assert(q == 0);
	}

	{
	char scale2[] = {6, -1};
	int q = ScaleQuantizer::quantize_semi(6, scale2);
	assert(q == 6);
	}

	{
	char scale3[] = {6, 7, 8, 11, -1};
	int q = ScaleQuantizer::quantize_semi(7, scale3);
	assert(q == 7);
	q = ScaleQuantizer::quantize_semi(6, scale3);
	assert(q == 6);
	q = ScaleQuantizer::quantize_semi(8, scale3);
	assert(q == 8);
	q = ScaleQuantizer::quantize_semi(11, scale3);
	assert(q == 11);
	}
}


// test  ScaleQuantizer::quantize_semi
// single note note in scale
void sq1()
{
	{
		char scale[] = {3, -1};
		int q = ScaleQuantizer::quantize_semi(11, scale);
		assert(q == 3);
	}
}

void ScaleQuantizerTests()
{
	sq0();
	sq1();
}