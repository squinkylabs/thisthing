
#include "stdafx.h"

#include "ModuleTester.h"
#include "DMScaleQuantizer.h"
#include "ScaleQuantizer.h"


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
	assert(ScaleQuantizer::getNumOctaveScales() > 0);
	for (int i = 0; i < ScaleQuantizer::getNumOctaveScales(); ++i)
	{
		const char * scale = ScaleQuantizer::getOctaveScale(i);
		char expanded[ ScaleQuantizer::expandedSize];
		const int len = ScaleQuantizer::expandScale(expanded, scale);
		assert(len >= 2);
	}
}


// todo: move module tests?
void sqm0()
{
	DModule& m = DMScaleQuantizer1();

	int x = 100;
	int y = 200;
	ZState z(0);

	int a=2;
	int b=3;

	m.go(false, x, y, z, a, b);
	assert(a == 0 && b==0);
}

void ScaleQuantizerTests()
{
	sq0();
	sq1();
	sq2();
	sq3();

	scale0();

	sqm0();
}