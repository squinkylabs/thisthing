
#include "stdafx.h"

#include "ModuleTester.h"
#include "DMScaleQuantizer.h"
#include "ScaleQuantizer.h"
#include "OctaveScaleManager.h"



static void q_test(const char * scale, char semi, char pitch, char expected_semi, char expected_pitch)
{
	char expanded[ ScaleQuantizer::expandedSize];
	const int len = ScaleQuantizer::expandScale(expanded, scale);

	int q = ScaleQuantizer::quantize_semi_expanded(semi, expanded, len);
	assert(q == expected_semi);

	q = ScaleQuantizer::quantize_expanded(pitch, expanded, len);
	if (q != expected_pitch) printf("will fail, q=%d, expected %d\n", q, expected_pitch);
	assert(q == expected_pitch);
}

static void q_test_shift(int shift, const char * scale, char semi, char pitch, char expected_semi, char expected_pitch)
{
	char expanded[ ScaleQuantizer::expandedSize];
	const int len = ScaleQuantizer::expandScaleShift(expanded, scale, shift);

	int q = ScaleQuantizer::quantize_semi_expanded(semi, expanded, len);
	assert(q == expected_semi);

	q = ScaleQuantizer::quantize_expanded(pitch, expanded, len);
	if (q != expected_pitch) printf("will fail, q=%d, expected %d\n", q, expected_pitch);
	assert(q == expected_pitch);
}


// test some simple case: ScaleQuantizer::quantize_semi
// single note in scale
static void sq0()
{
	printf("sq0\n");

	char scale[] = {0, -1};
	q_test(scale, 0, 0 + 2*12, 0, 0 + 2 * 12);


	char scale2[] = {6, -1};
	q_test(scale2, 6, 6+3*12, 6, 6 + 3*12);


	char scale3[] = {6, 7, 8, 11, -1};
	q_test(scale3, 7, 7, 7, 7);
	q_test(scale3, 6, 6, 6, 6);
	q_test(scale3, 8, 8, 8, 8);
	q_test(scale3, 11, 11 + 5*12, 11, 11 + 5*12);
}


// test  ScaleQuantizer::quantize_semi
// single note note in scale, wrap octaves
void sq1()
{
	printf("sq1\n");

		// out note is "past the end"
	char scale[] = {3, -1};
	q_test(scale, 11, 11 + 12, 3+12, 3+12*2);

	// out note is "before the start"
	char scale2[] = {11, -1};
	q_test(scale2, 3, 3, 11-12, 11-12);

}

// basic tests for "in between" notes
void sq2()
{
	printf("sq2, a\n");
	char scale[] = {3, 7, 11, -1};
	q_test(scale, 4, 4 + 12, 3, 3 + 12); // 4 is close to 3
	

	printf("sq2, b\n");
	q_test(scale, 6, 6, 7, 7);

}

// basic test for octave wrap around
void sq3()
{
	printf("sq3\n");
	char scale[] = { 1, 2, -1};
	q_test(scale, 11, 11, 1+12, 1+12); // 1 is "pretty close" to 11 if you wrap octaves
	
}


void scale0()
{
	OctaveScaleManager mgr;
	assert(OctaveScaleManager::getNumOctaveScales() > 0);
	for (int i = 0; i < OctaveScaleManager::getNumOctaveScales(); ++i)
	{
		const char * scale = OctaveScaleManager::getOctaveScale(i);
		char expanded[ ScaleQuantizer::expandedSize];
		const int len = ScaleQuantizer::expandScale(expanded, scale);
		assert(len >= 2);

		mgr.select(i);
		char size;
		char x = mgr.get(size)[0];
		char y = mgr.get(size)[size -1];
		assert(x >= -12);
		assert(x <= 0);
		assert(y >= 11);
		assert(y <= 24);
		assert(size >0);
		assert(size < 12);
	}
}


// simple shifted scale test
static void shs0()
{
	printf("shs0\n");

	char scale[] = {0, -1};
	q_test_shift(0, scale, 0, 0 + 2*12, 0, 0 + 2 * 12);

	// shift scale to c#, expect c# from C
	q_test_shift(1, scale, 0, 0 + 2*12, 1, 1 + 2 * 12);

}

void ScaleQuantizerTests()
{
	sq0();
	sq1();
	sq2();
	sq3();

	scale0();

	shs0();


}