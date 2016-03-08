
#include "stdafx.h"

#include "ScaleQuantizeAndRotate.h"
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

// tests quantizing to a a rotated scale. not something we do at the moment
static void q_test_rotated_scale(int rotation, const char * scale, char semi, char pitch, char expected_semi, char expected_pitch)
{
	char expanded[ ScaleQuantizer::expandedSize];
	const int len = ScaleQuantizer::expandScaleAndRotate(expanded, scale, rotation);

	int q = ScaleQuantizer::quantize_semi_expanded(semi, expanded, len);
	if (q != expected_semi) printf("will fail, q=%d, expected_semi=%d\n", q, expected_semi);
	assert(q == expected_semi);

	q = ScaleQuantizer::quantize_expanded(pitch, expanded, len);
	if (q != expected_pitch) printf("will fail, q=%d, expected_pitch %d\n", q, expected_pitch);
	assert(q == expected_pitch);
}

static void q_test_scale_then_rotation(int rotation, const char * scale, char semi, char pitch, char expected_semi, char expected_pitch)
{
	//char scale[] = {0, -1};
	char expanded[ ScaleQuantizer::expandedSize];
	const int len = ScaleQuantizer::expandScale(expanded, scale);

	// no rotation
	
	int out_semi = ScaleQuantizeAndRotate::quantizeAndRotate(len, expanded, rotation, semi);
	assert(out_semi == expected_semi);

	int out_pitch = ScaleQuantizeAndRotate::quantizeAndRotate(len, expanded, rotation, pitch);
	assert(out_pitch == expected_pitch);
	
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


// simple rotated scale test
static void shs0()
{
	printf("shs0\n");

	char scale[] = {0, -1};
	q_test_rotated_scale(0, scale, 0, 0 + 2*12, 0, 0 + 2 * 12);

	// shift scale to c#, expect c# from C
	q_test_rotated_scale(1, scale, 0, 0 + 2*12, 1, 1 + 2 * 12);

	// shift scale to b, expect b from C
	q_test_rotated_scale(11, scale, 0, 0 + 2*12, -1, -1 + 2 * 12);
}

static void shs1()
{
printf("\nshs1, a\n");
	char scale[] = {3, 7, 11, -1};
	q_test_rotated_scale(0, scale, 4, 4 + 12, 3, 3 + 12); // 4 is close to 3
	

printf("\nshs1, b\n");	
	// up 6
	q_test_rotated_scale(6, scale, 9, 9, 9, 9);
}

void sr0()
{
	char scale[] = {0, -1};
	char expanded[ ScaleQuantizer::expandedSize];
	const int len = ScaleQuantizer::expandScale(expanded, scale);

	// no rotation
	int outPitch = ScaleQuantizeAndRotate::quantizeAndRotate(len, expanded, 0, 0);
	assert(outPitch == 0);

	//rotate 1
	outPitch = ScaleQuantizeAndRotate::quantizeAndRotate(len, expanded, 1, 0);
	assert(outPitch == 1);



}

static void sr1()
{
	printf("sr1\n");
	char scale[] = {0, 5, -1};

	int rotation=0;
	q_test_scale_then_rotation(rotation, scale, 0, 0+12, 0, 0+12);
	q_test_scale_then_rotation(rotation, scale, 1, 1, 0, 0);
	q_test_scale_then_rotation(rotation, scale, 4, 4+12, 5, 5+12);
	q_test_scale_then_rotation(rotation, scale, 5, 5+12, 5, 5+12);
	q_test_scale_then_rotation(rotation, scale, 6, 6+12, 5, 5+12);

	q_test_scale_then_rotation(rotation, scale, 11, 11+12, 12, 12+12);

	rotation=1;
	printf("sr1-b\n");
	q_test_scale_then_rotation(rotation, scale, 0, 0+12, 0+rotation, 0+rotation+12);
	q_test_scale_then_rotation(rotation, scale, 1, 1+12, 0+rotation, 0+rotation+12);
	q_test_scale_then_rotation(rotation, scale, 4, 4+12, 5+rotation, 5+rotation+12);
	q_test_scale_then_rotation(rotation, scale, 5, 5+12, 5+rotation, 5+rotation+12);
	q_test_scale_then_rotation(rotation, scale, 6, 6+12, 5+rotation, 5+rotation+12);

	//  B quant to C, then rot 1, so no octave shift post quantize
	// (as per LC, 3/7/2016)
	q_test_scale_then_rotation(rotation, scale, 11, 11+12, 12+rotation, 12+rotation+12);
	
	rotation=8;
	printf("sr1-c\n");

	// C -> C, rotate up 8
	q_test_scale_then_rotation(rotation, scale, 0, 0+12, 0+rotation, 0+rotation+12);
	q_test_scale_then_rotation(rotation, scale, 1, 1+12, 0+rotation, 0+rotation+12);

	// 4 -> 5, rotates to 13 (different octave) so goes back down
	q_test_scale_then_rotation(rotation, scale, 4, 4+12, 5+rotation-12, 5+rotation+12-12);

	q_test_scale_then_rotation(rotation, scale, 5, 5+12, 5+rotation-12, 5+rotation+12-12);
	q_test_scale_then_rotation(rotation, scale, 6, 6+12, 5+rotation-12, 5+rotation+12-12);

	// B3 -> C4 then up 11, no wrap on octave
	q_test_scale_then_rotation(rotation, scale, 11, 11+12, 12+rotation, 12+rotation+12);

	
}


void ScaleQuantizerTests()
{
	sq0();
	sq1();
	sq2();
	sq3();

	scale0();

	shs0();
	shs1();


	sr0();
	sr1();

}