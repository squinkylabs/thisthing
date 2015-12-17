
#include "stdafx.h"
#include "DMTrackAndHoldAla152.h" 
#include <vector>


static Random _rand;
static const int ckhiz = DACVoltage::zcodeForMV(10000);
static const int ckloz = DACVoltage::zcodeForMV(0);

static const int ckhix = DACVoltage::xcodeForMV(10000);
static const int cklox = DACVoltage::xcodeForMV(0);

enum ExpectTrack { TRACK_A, TRACK_B, TRACK_NONE };

//static int last = 0;

static int ahold = 0;
static int bhold = 0;

static void verify(DModule& mod,  ExpectTrack expect, int x, int a, int b)
{
	switch(expect)
	{
	case TRACK_A:
		if (b != bhold) printf("expect hold b, but b=%d, last = %d\n", b, bhold);
		assert(b == bhold);

		if (a != x) printf("expect a track, but a=%d, x=%d\n", a, x);
		assert(a == x);
		break;
	case TRACK_B:
		if (b != x) printf("expecting b track, but b=%d, x = %d\n", b, x);
		assert(b == x);
		assert(a == ahold);
		break;
	case TRACK_NONE:
		if (a != ahold) printf("fail track none, a =%d ahold = %d\n", a, ahold);
		if (b != bhold) printf("fail track none, b =%d bhold = %d\n", b, bhold);
		assert(a == ahold);
		assert(b == bhold);
		break;
	default:
		assert(false);
	//assert(false);
	}

}

// this version for testing clock on z, cv on x
static void testz(DModule& mod, bool gate,   ExpectTrack expect)
{
	int x = _rand.get();
	int y = _rand.get();
	ZState z(gate ? ckhiz : ckloz);

	int a=-6, b=-7;
	mod.go(false, x, y, z, a, b);
	verify(mod, expect, x, a, b);
	

	// if we went low to high, then follow with high to low
	if (gate)
	{
		z = ZState(ckloz);
		x = _rand.get();
		mod.go(false, x, y, z, a, b);
		verify(mod, expect, x, a, b);
	}

	switch(expect)
	{
	case TRACK_A:
		ahold = x;
		break;
	case TRACK_B:
		bhold = x;
		break;
	case TRACK_NONE:
		break;

	default:
		assert(false);
	}
}


// this version for testing clock on x, cv on y
static void testx(DModule& mod, bool gate,   ExpectTrack expect, int _z)
{
	//int x = _rand.get();
	int y = _rand.get();
	int x = gate ? ckhix : cklox;

	//printf("testx a rand=%d, gate=%d\n", y, x);
	int a=-6, b=-7;
	ZState z(_z);
	mod.go(false, x, y, z, a, b);
	verify(mod, expect, y, a, b);
	

	//printf("testx b rand=%d, gate=%d\n", y, x);
	// if we went low to high, then follow with high to low
	if (gate)
	{
		x = ckloz;
		y = _rand.get();
		mod.go(false, x, y, z, a, b);
		verify(mod, expect, y, a, b);
	}

	switch(expect)
	{
	case TRACK_A:
		ahold = y;
		break;
	case TRACK_B:
		bhold = y;
		break;
	case TRACK_NONE:
		break;

	default:
		assert(false);
	}
	//printf("leave testx\n");
}

void ala2_sub(DMTrackAndHoldAla152& mod)
{
	ahold = bhold = 0;		// assume reset conditions
	
//	printf("ala2 sub--- \n");
	
	// new reset behavior
	//gate low (init), we should not track 
	testz(mod, false, TRACK_NONE);

	//printf("ala2-b (first clock after reset\n");
	//gate high  we should track A
	testz(mod, true, TRACK_A);

	//gate high, we should be tracking B
	testz(mod, true, TRACK_B);

	// gate high again, verify we track x->a, b holds 
	testz(mod, true, TRACK_A);
}

void ala2()
{
	printf("ala2\n");
	DMTrackAndHoldAla152 mod;
	ala2_sub(mod);

	// reset
	ZState z(234, false);
	int a, b;
	mod.go(true, 100, 100, z, a, b);

	ala2_sub(mod);
}




void testExtended_sub(DMTrackAndHoldExtendedAla152& mod, ExpectTrack * expected, int zVolt)
{	
	ahold=bhold = 0;

	testx(mod, false, TRACK_NONE, zVolt);

	//printf("ala3 b \n");
	testx(mod, true, expected[0], zVolt);
	
	//printf("ala3 cx \n");
	testx(mod, true, expected[1], zVolt);

	//printf("ala3 d \n");
	testx(mod, true, expected[2], zVolt);

	//printf("ala3 e\n");
	testx(mod, true, expected[3], zVolt);

	//printf("ala3 f\n");
	testx(mod, true, expected[0], zVolt);
	
	//printf("ala3 g\n");
	testx(mod, true, expected[1], zVolt);

	//printf("ala3 h \n");
	testx(mod, true, expected[2], zVolt);

	//printf("ala3 i\n");
	testx(mod, true, expected[3], zVolt);
}

void testExtended(ExpectTrack * expected, int zVolt)
{
	DMTrackAndHoldExtendedAla152 mod;
	testExtended_sub( mod, expected, zVolt);

		// reset
	ZState z(234, false);
	int a, b;
	mod.go(true, 100, 100, z, a, b);

	testExtended_sub( mod, expected, zVolt);
}



void ala3()
{
	int t=0;

	printf("ala3\n");
	//1   a b - -
	//printf("ALA[1]\n");
	ExpectTrack ex1[] = {TRACK_A, TRACK_B, TRACK_NONE, TRACK_NONE};
	testExtended( ex1, 0);
	++t;


	//2   a - b -
	//printf("ALA[2]\n");
	ExpectTrack ex2[] = {TRACK_A, TRACK_NONE, TRACK_B, TRACK_NONE};
	testExtended( ex2,  0x3ff * 2 / 12);
	++t;

	//3   a - - b
	//printf("ALA[3]\n");
	ExpectTrack ex3[] = {TRACK_A, TRACK_NONE, TRACK_NONE, TRACK_B};
	testExtended( ex3,  0x3ff * 3 / 12);
	++t;

	//4   - a b -
	//printf("ALA[4]\n");
	ExpectTrack ex4[] = {TRACK_NONE, TRACK_A, TRACK_B, TRACK_NONE};
	testExtended( ex4,  0x3ff * 4 / 12);
	++t;
	//5   - a - b
	//printf("ALA[5]\n");
	ExpectTrack ex5[] = {TRACK_NONE, TRACK_A,TRACK_NONE, TRACK_B};
	testExtended( ex5,  0x3ff * 5 / 12);
	++t;

	//6   b a - -
	//printf("ALA[6]\n");
	ExpectTrack ex6[] = {TRACK_B, TRACK_A, TRACK_NONE, TRACK_NONE };
	testExtended( ex6,  0x3ff * 6 / 12);
	++t;
	//7   - - a b
	//printf("ALA[7]\n");
	ExpectTrack ex7[] = {TRACK_NONE, TRACK_NONE ,TRACK_A, TRACK_B };
	testExtended( ex7,  0x3ff * 7 / 12);
	++t;
	//8   b - a -
	//printf("ALA[8]\n");
	ExpectTrack ex8[] = {TRACK_B, TRACK_NONE ,TRACK_A, TRACK_NONE };
	testExtended( ex8,  0x3ff * 8 / 12);
	++t;

	//9   - b a -
	//printf("ALA[9]\n");
	ExpectTrack ex9[] = {TRACK_NONE, TRACK_B ,TRACK_A, TRACK_NONE };
	testExtended( ex9,  0x3ff * 9 / 12);
	++t;

	//10 b - - a
	//printf("ALA[10]\n");
	ExpectTrack ex10[] = {TRACK_B, TRACK_NONE ,TRACK_NONE, TRACK_A };
	testExtended( ex10,  0x3ff * 10 / 12);
	++t;

	//11 - b - a
	//printf("ALA[11]\n");
	ExpectTrack ex11[] = {TRACK_NONE, TRACK_B ,TRACK_NONE, TRACK_A };
	testExtended( ex11,  0x3ff * 11 / 12);
	++t;

	// 12 - - b a
	//printf("ALA[12]\n");
	ExpectTrack ex12[] = { TRACK_NONE, TRACK_NONE, TRACK_B, TRACK_A};
	testExtended( ex12, 0x3ff * 12 / 12);
	++t;


	assert( t == 12);


	
}

#if 0
void ala3()
{
	printf("ala3\n");
	DMTrackAndHoldExtendedAla152 mod;
	ahold=bhold = 0;

	test(mod, false, TRACK_NONE);

	printf("ala3 b \n");
	test(mod, true, TRACK_A);
	
	printf("ala3 c \n");
	test(mod, true, TRACK_B);

	printf("ala3 d \n");
	test(mod, true, TRACK_NONE);

	printf("ala3 e\n");
	test(mod, true, TRACK_NONE);

	printf("ala3 f\n");
	test(mod, true, TRACK_A);
	
	printf("ala3 g\n");
	test(mod, true, TRACK_B);

	printf("ala3 h \n");
	test(mod, true, TRACK_NONE);

	printf("ala3 i\n");
	test(mod, true, TRACK_NONE);
}
#endif




void Ala152Test()
{
	ala2();
	ala2();

	ala3();

}

#if 0
void ala1()
{
	int x, y, z, a, b;

	

	x = 44;
	z = cklo;
	y = -1;
	DMTrackAndHoldAla152 mod;

	// no clock, verify a is tracking
	mod.go(x, y, z, a, b);
	assert(a==44);
	assert(b==0);

	// first trigger, verify we switch to track b.
	x = 97;
	z = ckhi;
	mod.go(x, y, z, a, b);
	assert(a == 44);
	assert(b == 97);


	// gate low, verify we are still tracking x->b
	x = 77;
	z = cklo;
	mod.go(x, y, z, a, b);
	assert(a == 44);
	assert(b == 77);

	// gate high again, verify we track x->a, b holds 
	x = 1234;
	z = ckhi;
	mod.go(x, y, z, a, b);
	assert(a == 1234);
	assert(b == 77);

	// gate low, verify we are still tracking x->a
	x = 99;
	z = cklo;
	mod.go(x, y, z, a, b);
	assert(a == 99);
	assert(b == 77);

}
#endif
