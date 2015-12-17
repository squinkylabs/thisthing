#include "stdafx.h"


#include "DMCounter.h"
#include "Counter.h"
#include "Counter.h"
#include "DMCounter.h"
#include "DACVoltage.h"

void ct0()
{
	Counter ct;
	ct.setPeriod(1);
	assert(ct.get());
	ct.clock();
	assert(ct.get());
	ct.clock();
	assert(ct.get());

}


void ct1()
{
	Counter ct;
	ct.setPeriod(2);
	assert(ct.get());

	ct.clock();
	assert(!ct.get());
	ct.clock();
	assert(ct.get());
	ct.clock();
	assert(!ct.get());
	ct.clock();
	assert(ct.get());
}


void ct2()
{
	Counter ct;
	ct.setPeriod(10);
	assert(ct.get());

	for (int i=0; i<9; ++i)
	{
		ct.clock();
		assert(!ct.get());
	}
	ct.clock();
	assert(ct.get());

}


void ctm0()
{
	DMCounter ct(2, 4);

	const int _8v = DACVoltage::xcodeForMV(8000);

	int a, b;
	ZState z(0);
	ct.go(false, 0, 0, z, a, b);	// no clock, should be triggering
	assert( a ==_8v);

	// now clock, period 2, should go low
	printf("ctm0 a\n");
	ct.go(false, _8v, 0, z, a, b);
	assert( a ==0);

	// now clock. low (no change) and hi again
	printf("ctm0 b\n");
	ct.go(false, 0, 0, z, a, b);
	assert( a ==0);
	ct.go(false, _8v, 0, z, a, b);
	assert( a ==_8v);

	// now clock. low (no change) and hi again. this will trigger period
	printf("ctm0 c\n");
	ct.go(false, 0, 0, z, a, b);
	assert( a ==_8v);
	ct.go(false, _8v, 0, z, a, b);
	assert( a ==0);
}



void ctm1()
{
	DMCounter ct(2, 4);

	const int _8v = DACVoltage::xcodeForMV(8000);
	ZState z(0x3ff);

	// initial state, 1/4 = high
	int a, b;
	ct.go(false, 0, 0, z, a, b);	// no clock, should be triggering
	assert( a ==_8v);

	// now clock, period 2/4, should go low
	printf("ctm1 a\n");
	ct.go(false, _8v, 0, z, a, b);
	assert( a ==0);

	// now clock. period 3/4
	printf("ctm0 b\n");
	ct.go(false, 0, 0, z, a, b);
	assert( a ==0);
	ct.go(false, _8v, 0, z, a, b);
	assert( a ==0);

	// now clock.period 4/4
	printf("ctm0 c\n");
	ct.go(false, 0, 0, z, a, b);
	assert( a ==0);
	ct.go(false, _8v, 0, z, a, b);
	assert( a ==0);
}

void CounterTests()
{
#if 0
	ct0();
	ct1();
	ct2();


	ctm0();
#endif
	ctm1();
}