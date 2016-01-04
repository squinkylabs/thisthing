
#include "stdafx.h"
#include "TestWrappers.h"
#include "TestWrappers.h"
#include "DMSlopeDetector.h"


bool doesTrigger(I_Test& module, int stepSize, int duration)
{
	//const int thresh = DACVoltage::xcodeForMV(1000);		// huge step
	int input = 0;
	for (int i=0; i<duration; ++i)
	{
	//	printf("about to call do it with %x\n", input);
		if (input > 0x7fffff)
		{
			return false;
		}
		int output = module.doIt(input);
		input += stepSize;
		//printf("output = %d\n", output);
		if (output > 0) 
			return true;
	}
	return false;
}

void sdTest(int step, bool expectDetect, int tcUS, int modMinUS, int modMaxUS, int modZ)
{
	printf("sdtest, step=%d, tc=%d,%d modz=(%d) expect=%d\n", step, modMinUS, modMaxUS, modZ, expectDetect);
	
	// first, non-module case
	{
		SlopeDetector _sd(20, tcUS, Constants::DISTING_SAMPLE_RATE, true);
		SDWrap sd(_sd);
		bool trig = doesTrigger(sd, step, 70000);
		if (trig != expectDetect)
		{
			printf("failing non-mod, step=%d, tc=%d\n", step, tcUS);
		}
		assert(trig == expectDetect);

	}
	// now the module case
	{
		//assert(modZ == 0);
		DMSlopeDetectorDual _sdm(tcUS);
		SDMWrap sdm(_sdm, modZ);
		bool trig = doesTrigger(sdm, step, 70000);
		if (trig != expectDetect)
		{
			printf("failing mod, step=%d, tc=%d,%d modz=(%d) expect=%d\n", step, modMinUS, modMaxUS, modZ, expectDetect);
		}
		assert(trig == expectDetect);
	}
}

// step that triggers at 1 ms
void sd2_1()
{
	printf("sd2_1\n");
	const int step = DACVoltage::xcodeForMV(1) * Constants::SAMPLE_RATE_DIVISOR;	// make step bigger at reduced sr	
	const int tc = 1000;
	sdTest(step, true, tc, tc, 10 * tc, 0);
}



// step that doesn't trigger at 1 ms
void sd2_2()
{
	printf("sd2_2\n");
	//const int step = 1;		// tinny step
	const int step = Constants::SAMPLE_RATE_DIVISOR * DACVoltage::xcodeForMV(1) / 4;	 // todo: tighten tolerence between mod and non-mode
	const int tc = 1000;
	sdTest(step, false, tc, tc, 10 * tc, 0);
}


// step that triggers at 10 ms
void sd2_3()
{
	printf("sd2_3\n");
	const int step =  Constants::SAMPLE_RATE_DIVISOR *DACVoltage::xcodeForMV(1) / 10;	
	const int tc = 1000 * 10;
	sdTest(step, true, tc, tc, 10 * tc, 0);
}

// step that triggers at 10 ms, other pot orientation
void sd2_3b()
{
	printf("sd3_b\n");
	const int step =   Constants::SAMPLE_RATE_DIVISOR * DACVoltage::xcodeForMV(1) / 10;	
	const int tc = 1000 * 10;
	sdTest(step, true, tc, tc / 10, tc, 0x3ff);
}


// step that doesn't triggers at 10 ms
void sd2_4()
{
	printf("sd2_4\n");
	const int step = Constants::SAMPLE_RATE_DIVISOR *DACVoltage::xcodeForMV(1) / 40;	
	const int tc = 1000 * 10;
	sdTest(step, false, tc, tc, 10 * tc, 0);
}


// step that doesn't triggers at 10 ms
void sd2_4b()
{
	printf("sd2_4b\n");
	const int step = Constants::SAMPLE_RATE_DIVISOR *DACVoltage::xcodeForMV(1) / 40;	
	const int tc = 1000 * 10;
	sdTest(step, false, tc, tc/4, tc, 0x3ff);
}


void sd2_5()
{
	printf("sd2_5\n");
	DMRiseAndFallDetector d;

	int x=0;
	int y=0;
	
	int a, b;
	ZState z(0, true);

	d.go(false, x, y, z, a, b);
	assert(a==0);
	assert(b==0);

	// go high
	const int ghigh = DACVoltage::xcodeForGateHi();
	x = DACVoltage::xcodeForMV(1000);


	const int filter_delay = Constants::SLOPE_TEST_DELAY;


	for (int i=0; i<filter_delay; ++i)
		d.go(false, x, y, z, a, b);

	assert(a==ghigh);
	assert(b==0);

	// stay high
	x = DACVoltage::xcodeForMV(1000);
	for (int i=0; i<filter_delay; ++i)
	d.go(false, x, y, z, a, b);
	assert(a==ghigh);
	assert(b==0);

	// go low
	x = -DACVoltage::xcodeForMV(1000);
	for (int i=0; i<filter_delay; ++i)
		d.go(false, x, y, z, a, b);
	assert(a==ghigh);
	assert(b==ghigh);

	// stay here for 500 samples
	for (int i=0; i<500; ++i)
		d.go(false, x, y, z, a, b);

	assert(a==0);
	assert(b==0);

	// go high again
	x = DACVoltage::xcodeForMV(1000);
	for (int i=0; i<filter_delay; ++i)
		d.go(false, x, y, z, a, b);
	assert(a==ghigh);
	assert(b==0);

		// stay here for 500 samples
	for (int i=0; i<500; ++i)
		d.go(false, x, y, z, a, b);

	assert(a==0);
	assert(b==0);

	// go low again
	x = -DACVoltage::xcodeForMV(1000);
	for (int i=0; i<filter_delay; ++i)
		d.go(false, x, y, z, a, b);
	assert(a==0);
	assert(b==ghigh);
}


void sd2_6()
{
	printf("sd2_6\n");

	const int filter_delay = Constants::SLOPE_TEST_DELAY;

	//y->b
	DMDualFunctionQuantizerHelper d;

	int x=0;;
	int y=0;
	//int z=0;
	ZState z(0, true);
	int a, b;

	d.go(false, x, y, z, a, b);
	assert(a==0);
	assert(b==0);

	// go high
	const int ghigh = DACVoltage::xcodeForGateHi();

	
	y = DACVoltage::xcodeForMV(1000);
	for (int i=0; i<filter_delay; ++i)
		d.go(false, x, y, z, a, b);
	assert(a==0);
	assert(b==ghigh);

	// stay high
	y = DACVoltage::xcodeForMV(1000);
	d.go(false, x, y, z, a, b);
	assert(a==0);
	assert(b==ghigh);



	// stay here for 500 samples
	for (int i=0; i<500; ++i)
		d.go(false, x, y, z, a, b);

	assert(a==0);
	assert(b==0);

	// let it settle
	for (int i=0; i<5000; ++i)
		d.go(false, x, y, z, a, b);

		// go low
	y = -DACVoltage::xcodeForMV(1000);
	for (int i=0; i<filter_delay; ++i)
		d.go(false, x, y, z, a, b);
	assert(a==0);
	assert(b==ghigh);
}

void SlopeDetectorTests2()
{
	sd2_1();
	sd2_2();
	sd2_3();
	sd2_3b();
	sd2_4();
	sd2_4b();

	sd2_5();

	//printf("sd2_6 test skipped becuase slope detector tests don't know about LPF\n");
	sd2_6();
	

}
