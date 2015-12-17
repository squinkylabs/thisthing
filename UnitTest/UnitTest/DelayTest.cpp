

#include "stdafx.h"
#include "DelayLine.h"
#include "DMShiftRegister.h"
#include "DACVoltage.h"

// can we construct one
static void dt1()
{
	DelayLine<int, 4> dl(1);
}


// is the intial output correct
static void dt2()
{
	DelayLine<int, 4> dl(1);
	assert (dl.get() == 0);
}


// is the intial output correct (non default)
static void dt3()
{
	DelayLine<int, 4> dl(1, 123);
	assert (dl.get() == 123);
}


//clock it and check results
static void dt4()
{

	printf("starting dt4\n");
	DelayLine<int, 4> dl(1);
	assert (dl.get() == 0);

	
	dl.clock(100);
	assert (dl.get() == 0);

	dl.clock(200);
	assert (dl.get() == 100);
}


//clock all the way around 2 delay
static void dt5()
{

	printf("starting dt4\n");
	DelayLine<int, 4> dl(2);
	assert (dl.get() == 0);

	dl.clock(100); 
	assert (dl.get() == 0);

	dl.clock(200);
	assert (dl.get() == 0);

	dl.clock(300);
	assert (dl.get() == 100);

	
	dl.clock(400);
	assert (dl.get() == 200);

	
	dl.clock(500);
	assert (dl.get() == 300);

	
	dl.clock(600);
	assert (dl.get() == 400);

}



static void dt6()
{
	DelayLine<int, 4> dl(1, 123);
	
	dl.setDelay(2);
	dl.clock(1);
	dl.clock(2);
	dl.clock(3);
	assert(dl.get() == 1);

	dl.setDelay(1);
	assert(dl.get() == 2);

	dl.clock(4);
	assert(dl.get() == 3);
}

void sr1_sub(DMDigitalShiftRegister& dr)
{
	
	DModule& dm = dr;

	const int v8 = DACVoltage::xcodeForMV(8000);
	ZState z(0, true);	// this should make clock div 1
	int xclock = 0;
	int ycv = 57;

	int a=-1, b=-2;
	dm.go(false, xclock, ycv, z, a, b);

	// no clock, init output
	assert(a==0);
	assert(b==0);
	assert(dr.getDivider() == 1);		// z=0 should map to one

	// clock once
	xclock = v8;
	dm.go(false, xclock, ycv, z, a, b);
	assert(a==57);
	assert(b==0);

	xclock = 0;
	dm.go(false, xclock, ycv, z, a, b);
	assert(a==57);
	assert(b==0);

	// clock twice
	xclock = v8;
	ycv = -3;
	dm.go(false, xclock, ycv, z, a, b);
	assert(a==-3);
	assert(b==57);
}

void sr1()
{
	DMDigitalShiftRegister dr;
	sr1_sub(dr);

			// reset
	ZState z(234, false);
	int a, b;
	dr.go(true, 100, 100, z, a, b);
	sr1_sub(dr);
}


void sr2()
{
	printf("*** sr2\n");
	DMDigitalShiftRegister dr;
	DModule& dm = dr;

	const int v8 = DACVoltage::xcodeForMV(8000);
	ZState z(0x3ff);	// this should make clock div 1
	int xclock = 0;
	int ycv = 57;

	int a=-1, b=-2;


	dm.go(false, xclock, ycv, z, a, b);

	// no clock, init output
	assert(a==0);
	assert(b==0);
	assert(dr.getDivider() == 8);		// z=max should map to 8


	// clock 7 times = should do nothing
	for (int i=0; i<7; ++i)
	{
		xclock = v8;
		dm.go(false, xclock, ycv, z, a, b);
		assert(a==0);
		assert(b==0);

		xclock = 0;
		dm.go(false, xclock, ycv, z, a, b);
		assert(a==0);
		assert(b==0);
	}

	//printf("done with loop\n");
	// 8th time should do something

	xclock = v8;
	dm.go(false, xclock, ycv, z, a, b);
	assert(a==57);
	assert(b==0);

	xclock = 0;
	dm.go(false, xclock, ycv, z, a, b);

	ycv = -3;
	// clock 7 times = should do nothing
	for (int i=0; i<7; ++i)
	{
		xclock = v8;
		dm.go(false, xclock, ycv, z, a, b);
		assert(a==57);
		assert(b==0);

		xclock = 0;
		dm.go(false, xclock, ycv, z, a, b);
		assert(a==57);
		assert(b==0);
	}

	//printf("done with loop\n");
	// 8th time should do something

	xclock = v8;
	dm.go(false, xclock, ycv, z, a, b);
	assert(a==-3);
	assert(b==57);

}

void DelayTest()
{
	dt1();
	dt2();
	dt3();
	dt4();
	dt5();
	dt6();

	sr1();
	sr2();
}