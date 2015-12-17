#include "stdafx.h"

#include "DMTuningRef.h"
#include "DMAdders.h"
#include "DACVoltage.h"

void at0_sub(DMOctaveAdderSubtractor& as)
{
	int x=0;
	int y=0;
	int a=-1;
	int b=-1;
	const int minus5 = DACVoltage::xcodeForMV(-5000);
	const int plus5 = DACVoltage::xcodeForMV(5000);

	// first test that we are at default;
	ZState zreset(0x300, false);
	as.go(true, x, y, zreset, a, b);
	assert(a == 0);
	assert(b == 0);


	ZState z( 0);

	as.go(false, x, y, z, a, b);
	assert(a == minus5);
	assert(b == minus5);

	z = ZState(0x3ff);
	as.go(false, x, y, z, a, b);
	//printf("a=%d, plus3=%d\n", a, plus3);
	assert(a == plus5);
	assert(b == plus5);

	x = 22;
	y = 123456;
	as.go(false, x, y, z, a, b);
	assert(a == plus5 + x + y);
	assert(b == plus5 + x - y);

	z = ZState(0x200);
	x = 222;
	y = 23456;
	as.go(false, x, y, z, a, b);
	assert(a == 0 + x + y);
	assert(b == 0 + x - y);
}

void at0()
{
	printf("at0\n");
	DMOctaveAdderSubtractor as;
	at0_sub(as);


	printf("at0-2\n");
	int x = 0, y=0;
	int a, b;
	ZState z(0, true);
	as.go(true, x, y, z, a, b);		// reset
	at0_sub(as);
}


void at1_sub(DMSemitoneAdderSubtractor& as)
{
	
	DModule& dm = as;
	

	int a=-1;
	int b=-1;
	int x=0;
	int y=0;

	const int semi1 = DACVoltage::xcodeForuV(1000 * 1000 / 12);
	const int oct = DACVoltage::xcodeForMV(1000);


	// first test that we are at default;
	ZState zreset(0x250, false);
	as.go(true, x, y, zreset, a, b);
	assert(a == 0);
	assert(b == 0);

	ZState z;

	as.go(false, x, y, z, a, b);
	assert(a == 0);
	assert(b == 0);

	z =ZState(0x3ff);
	as.go(false, x, y, z, a, b);
	assert(a == oct);
	assert(b == -oct);

	z =ZState(0x200);
	as.go(false, x, y, z, a, b);
	assert(a == oct/2);
	assert(b == -oct/2);


	z = ZState(0x200);
	x = 222;
	y = 23456;
	as.go(false, x, y, z, a, b);
	assert(a == oct/2 + x + y);
	assert(b == -oct/2 + x + y);
}

void at1()
{
	printf("at1\n");
	DMSemitoneAdderSubtractor as;
	at1_sub(as);


	printf("at1-2\n");
	int x = 0, y=0;
	int a, b;
	ZState z(0, true);
	as.go(true, x, y, z, a, b);		// reset
	at1_sub(as);
}


void at2_sub(DMFPrecisionFMHelper& as)
{

	int a=-1;
	int b=-1;
	int x=0;
	int y= ChromaticQuantizer::middleCV;
	const int oct = DACVoltage::xcodeForMV(1000);

	// first test that we are at default;
	ZState zreset(0x250, false);
	as.go(true, x, y, zreset, a, b);
	assert(a == y);
	assert(b == y);


	ZState z;

	as.go(false, x, y, z, a, b);
	assert(a == y);
	assert(b == y);

	x = 345;
	//y = -111;
	as.go(false, x, y, z, a, b);
	assert(a == 0 + x + y);
	assert(b == 0 + x + y);

	z = ZState(0x3ff);
	x=0; 
	
	as.go(false, x, y, z, a, b);
	//printf("a = %d, oct=%d\n", a, oct);
	assert(a == oct + x + y);
	//printf("b = %d, -oct=%d\n", b, -oct);
	//printf("total = %d\n", (-oct + x + y));
	assert(b == (-oct + x + y));

	int lasta=y-100;
	int lastb= y+100;
	for (z.value=0; z.value<0x400; ++z.value)
	{	
		as.go(false, x, y, z, a, b);
		assert(a > lasta);
		assert(b < lastb);

		lasta = a;
		lastb = b;
	}
}


void at2()
{
	printf("at2\n");
	DMFPrecisionFMHelper as;
	at2_sub(as);


	printf("at2-2\n");
	int x = 0, y=0;
	int a, b;
	ZState z(0, true);
	as.go(true, x, y, z, a, b);		// reset
	at2_sub(as);
}




void at3_sub(DMTuningReference& as)
{
	printf("at3 sub\n");
	
	DModule& dm = as;
	
	int x=0;
	int y=0;
	//const int minus3 = DACVoltage::xcodeForMV(-3000);
	const int plus3 = DACVoltage::xcodeForMV(3000);

	const int p75 =  DACVoltage::xcodeForuV((int)(.75 * 1000000.0));
	const int _175v = DACVoltage::xcodeForuV((int)(1.75 * 1000000.0));
	const int _275v = DACVoltage::xcodeForuV((int)(2.75 * 1000000.0));
	const int _375v = DACVoltage::xcodeForuV((int)(3.75 * 1000000.0));
	const int _475v = DACVoltage::xcodeForuV((int)(4.75 * 1000000.0));

	// reset state
	ZState zInit(200, false);
	int a=-1;
	int b=-1;
	as.go(false, x, y, zInit, a, b);
	//printf("1a=%d, 75=%d\n", a, p75);
	assert(a == p75);
	assert(b == (a + plus3));

	ZState z;		// zero
	as.go(false, x, y, zInit, a, b);
	//printf("1a=%d, 75=%d\n", a, p75);
	assert(a == p75);
	assert(b == (a + plus3));

	//4
	
	z.value = 0x3ff;
	as.go(false, x, y, z, a, b);
	//printf("2a=%d, plus3=%d\n", a, plus3);
	assert(a == _475v);
	assert(b == (a + plus3));

	// 1
	//printf("trying step 1\n");
	z.value =  0x50 + (0x400 / 5);
	x = 22;
	y = 123456;
	as.go(false, x, y, z, a, b);
	assert(a == _175v);
	assert(b == (a + plus3));

	// 2
	//printf("trying step 2\n");
	z.value =  0x50 + (2 * 0x400 / 5);
	x = 22;
	y = 123456;
	as.go(false, x, y, z, a, b);
	assert(a == _275v);
	assert(b == (a + plus3));

		// 3
	//printf("trying step 3\n");
	z.value =  0x50 + (3 * 0x400 / 5);
	x = 22;
	y = 123456;
	as.go(false, x, y, z, a, b);
	assert(a == _375v);
	assert(b == (a + plus3));
}

void at3()
{
	printf("at3\n");
	DMTuningReference as;
	at3_sub( as);

	int a,b;
	ZState z(300, false);
	as.go(true, 0, 0, z, a, b);

	at3_sub( as);

}
	

void at4()
{
	printf("at4\n");
	DMTuningReference as;
	DModule& dm = as;
	
	int x=0;
	int y=0;
	int a, b;
	//const int minus3 = DACVoltage::xcodeForMV(-3000);
	const int v8 = DACVoltage::xcodeForMV(8000);
	const int v81 = DACVoltage::xcodeForMV(8010);

	const int _375v = DACVoltage::xcodeForuV((int)(3.75 * 1000000.0));
	const int plus3 = DACVoltage::xcodeForMV(3000);

	const int vn8 = DACVoltage::xcodeForMV(-8000);
	const int vn81 = DACVoltage::xcodeForMV(-8010);

	const int pt25 =  DACVoltage::xcodeForuV((int)(.25 * 1000000.0));
	const int pt75 =  DACVoltage::xcodeForuV((int)(.75 * 1000000.0));


	printf("trying step 3\n");
	ZState z(  0x50 + (3 * 0x400 / 5));
	x = v8;
	as.go(false, x, y, z, a, b);
	assert(a == _375v);
	assert(b == (a + plus3));		

	x = v81;
	as.go(false, x, y, z, a, b);
	assert(a == _375v);
	assert(b == (a + plus3 + pt25));		// overvolatage indicator

	// overvoltage should stay on for a bit
	const int duration = Constants::DISTING_SAMPLE_RATE / 4;

	x = v8;
	
	for (int i=0; i<(duration-1); ++i)
	{
		as.go(false, x, y, z, a, b);
		assert(a == _375v);
	//	printf("dur=%d\", i=%d\n", duration, i);
		assert(b == (a + plus3 + pt25));		// undervolatage indicator
	}

	// after timeout, overvoltage goes away
	as.go(false, x, y, z, a, b);
	assert(a == _375v);
	assert(b == (a + plus3));		// undervolatage indicator

	

	//under
	x = vn81;
	as.go(false, x, y, z, a, b);
	assert(a == _375v);
	assert(b == (a + plus3 - pt75));		// undervolatage indicator

	x = vn8;
	for (int i=0; i<(duration-1); ++i)


	{
		as.go(false, x, y, z, a, b);
		assert(a == _375v);
		assert(b == (a + plus3 - pt75));		// undervolatage indicator
	}



	as.go(false, x, y, z, a, b);
	assert(a == _375v);
	assert(b == (a + plus3 ));		// overvolatage indicator
}



void AdderTests()
{
	at0();
	at1();
	at2();
	at3();
	at4();
	
}