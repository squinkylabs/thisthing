
#include "stdafx.h"

#include "DMSampleAndHold.h"
#include "SampleAndHold.h"
#include "SampleAndHold.h"
#include "DMSlopeDetector.h"

const int zerox =  DACVoltage::xcodeForMV(0);
const int twox = DACVoltage::xcodeForMV(2 * 1000);
const int threex = DACVoltage::xcodeForMV(3 * 1000);
const int fourx = DACVoltage::xcodeForMV(4 * 1000);
const ZState tenz(DACVoltage::zcodeForMV(10 * 1000), true);
const ZState zeroz(DACVoltage::zcodeForMV(0 * 1000), true);

void sh0()
{
	SampleAndHold sh;
	assert(0==sh.sampleAndHold());
	assert(0==sh.trackAndHold());
	sh.go(false, false, 0);
	assert(0==sh.sampleAndHold());
	assert(0==sh.trackAndHold());
}

void sh1()
{
	SampleAndHold sh;
	sh.go(false, true, 100);
	assert(100 == sh.sampleAndHold());
	assert(0 == sh.trackAndHold());
}


void sh2()
{
	SampleAndHold sh;
	sh.go(true, false, 100);
	assert(0 == sh.sampleAndHold());
	assert(100 == sh.trackAndHold());
}

void sh3()
{
	SampleAndHold sh;
	sh.go(true, false, 100);
	assert(0 == sh.sampleAndHold());
	assert(100 == sh.trackAndHold());
}

void sh4()
{
	SampleAndHold sh;
	sh.go(true, true, 100);
	assert(100 == sh.sampleAndHold());
	assert(100 == sh.trackAndHold());

	sh.go(true, false, -100);
	assert(100 == sh.sampleAndHold());
	assert(-100 == sh.trackAndHold());

	sh.go(true, false, -100);
	assert(100 == sh.sampleAndHold());
	assert(-100 == sh.trackAndHold());

	sh.go(false, false, 55);
	assert(100 == sh.sampleAndHold());
	assert(-100 == sh.trackAndHold());
}

void sh5()
{
	DMSampleAndHold dm;		// just see if it builds
	int a=1, b=2;
	ZState z(0, true);
	dm.go(false, 0, 0, z, a, b);
	assert( a==0 && b==0);
}


void sh6_sub2(DMSampleAndHold& dm, bool zchange)
{
	int a=1, b=2;
	
	ZState _zero(zeroz.value, zchange);
	ZState _ten(tenz.value, zchange);

	dm.go(false, twox, 0, _zero, a, b);	//prime with a low gate after reset
	assert(a==0 && b==0);

	dm.go(false, twox, 0, _ten, a, b);	// should generate trig
	assert(a==twox && b==twox);

	//dm.go(three, ten, 0, a, b);
	dm.go(false, threex, 0, tenz, a, b);
	assert(a==twox && b==threex);
}

void sh6_sub(bool zchange)
{
	DMSampleAndHold dm;		// just see if it builds
	sh6_sub2(dm, zchange);
	int a, b;

	// now reset it
	dm.go(true, fourx, 0, tenz, a, b);
	assert(a==0 && b==0);
	sh6_sub2(dm, zchange);
}

void sh6()
{
	sh6_sub(true);
	sh6_sub(false);
}



//------------------------------------------------
// module tests

enum  CVIN { CVIN_X, CVIN_Y } ;
enum  CVOUT { CVOUT_A, CVOUT_B } ;
enum CKIN {CKIN_X, CKIN_Y, CKIN_Z};


const int FLAG_BISMINUSA = 1;
const int FLAG_IGNOREOTHER = 2;
const int FLAG_CLOCKINVERTED = 4;

bool testMod(DModule& mod, CVIN cvin, CVOUT cvout, bool isTrackAndHold, CKIN ckin,
	bool isProbabalistic = false,
	int flag = 0)
{
#if VERBOSE > 1
	printf("enter testMod\n");
#endif
	int x=0, y=0;
	ZState zReal(0, true);
	int a=-1, b=-1;
	bool didFire = false;
	const bool bIsMinusA = !!(flag & FLAG_BISMINUSA);
	const bool bIgnoreOtherOutput = !!(flag & FLAG_IGNOREOTHER);
	const bool bCLockInverted = !!(flag & FLAG_CLOCKINVERTED);

	const int testv = DACVoltage::xcodeForMV(6666);
	const int testv2 = DACVoltage::xcodeForMV(7777);
	// set up inputs
	switch(cvin)
	{
	case CVIN_X:
		x = testv;
		break;
	case CVIN_Y:
		y = testv;
		break;
	default:
		assert(false);
	}

	if (isProbabalistic)
		zReal = ZState(0x200, true);

#if VERBOSE > 1
	printf("running first high gate\n");
#endif
	// first turn with a gate active. should not trigger (yet)
	// (new for rest
	ZState zNothing(0, false);
	switch (ckin)
	{
	case CKIN_Z:
		zReal = ZState(bCLockInverted ? 0 : DACVoltage::zcodeForMV(10000), true);
		mod.go(false, x, y, zReal, a, b);
	
		break;
	case CKIN_X:
		assert(!bCLockInverted);		// can't test this yet
		x = DACVoltage::xcodeForMV(10000);
		mod.go(false, x, y, zNothing, a, b);
	
		break;
	default:
		assert(false);
	}

	if (cvout == CVOUT_A)
	{
		if (a !=0 ) printf("gate at reset not ignored, seeing %d\n", a);
		assert(a == 0);
	}
	
	if (cvout == CVOUT_B)
	{
		if (b !=0 ) printf("gate at reset not ignored, seeing %d\n", b);
		assert(b == 0);
	}


	// first, set up a gate low then high
	switch (ckin)
	{
	case CKIN_Z:
		zReal = ZState(bCLockInverted ? DACVoltage::zcodeForMV(10000) : 0, true);
		mod.go(false, x, y, zReal, a, b);
		zReal = ZState(bCLockInverted ? 0 : DACVoltage::zcodeForMV(10000), true);
		mod.go(false, x, y, zReal, a, b);
	
		break;
	case CKIN_X:
		assert(!bCLockInverted);		// can't test this yet
		x = 0;
		mod.go(false, x, y, zNothing, a, b);
		x = DACVoltage::xcodeForMV(10000);
		mod.go(false, x, y, zNothing, a, b);
	
		break;
	default:
		assert(false);
	}

	const int _inversionPoint = DACVoltage::xcodeForuV(Constants::INVERSION_VOLTAGE * 1000 * 1000);
	//printf("x=%d y=%d z=%d a=%d b=%d testv=%d\n", x,y,z,a,b,testv);
	assert(a >=0);

	if (bIsMinusA)
		assert (b == _inversionPoint -a);
	else
		assert(b >= 0);
	

	// now look at outputs 
	switch(cvout)
	{
	case CVOUT_A:
		if (a == testv) didFire = true;
		if (!isProbabalistic)
		{
			if (a != testv) printf("deterministic failed to fire a=%d testv=%d\n", a, testv);
			assert(a == testv);
		}

		if (!bIsMinusA && !bIgnoreOtherOutput)
			assert(b == 0);
		break;
	case CVOUT_B:
		assert(b == testv);
		if (!bIgnoreOtherOutput) assert(a == 0);
		break;
	default:
		assert(false);
	}

	// now change input, with gate still high
	// set up inputs
	switch(cvin)
	{
	case CVIN_X:
		x = testv2;
		break;
	case CVIN_Y:
		y = testv2;
		break;
	default:
		assert(false);
	}

	mod.go(false, x, y,
		(ckin == CKIN_Z) ? zReal : zNothing,
		a, b);

	// now look at outputs again
	switch(cvout)
	{
	case CVOUT_A:
		//TODO: test probabalistic track and hold
		if (isProbabalistic) assert(!isTrackAndHold);
		if (!isProbabalistic)
			assert(a == (isTrackAndHold ? testv2 : testv));

		if (bIsMinusA)
			assert(b == _inversionPoint-a);
		else if (!bIgnoreOtherOutput)
			assert(b == 0);
		break;
	case CVOUT_B:
		assert(b == (isTrackAndHold ? testv2 : testv));
		if (!bIgnoreOtherOutput) assert(a == 0);
		break;
	default:
		assert(false);
	}
	return didFire;
}

static void resetModule(DModule& mod)
{
	int a;
	int x = DACVoltage::xcodeForMV(10000);
	
	mod.go(true, x, x, ZState(0x3ff, true), a, a);
}

void shm1()
{
	printf("shm1\n");
	{
		DMSampleAndHoldDual mod;
		testMod(mod, CVIN_X, CVOUT_A, false, CKIN_Z);
		resetModule(mod);
		testMod(mod, CVIN_X, CVOUT_A, false, CKIN_Z);
	}
	{
		DMSampleAndHoldDual mod;
		testMod(mod, CVIN_Y, CVOUT_B, false, CKIN_Z);
		resetModule(mod);
		testMod(mod, CVIN_Y, CVOUT_B, false, CKIN_Z);
	}
}

void shm2()
{
	// channel 0
	{
		DMSampleAndHoldDualClock mod;
		testMod(mod, CVIN_Y, CVOUT_A, false, CKIN_Z);
		resetModule(mod);
		testMod(mod, CVIN_Y, CVOUT_A, false, CKIN_Z);
	}
	{
		DMSampleAndHoldDualClock mod;
		testMod(mod, CVIN_Y, CVOUT_B, false, CKIN_X);
		resetModule(mod);
		testMod(mod, CVIN_Y, CVOUT_B, false, CKIN_X);
	}
}



void shm4()
{
	// channel 0
	{
		DMTrackAndHoldDualClock mod;
		testMod(mod, CVIN_Y, CVOUT_A, true, CKIN_Z);
		resetModule(mod);
		testMod(mod, CVIN_Y, CVOUT_A, true, CKIN_Z);
	}
	{
		DMTrackAndHoldDualClock mod;
		testMod(mod, CVIN_Y, CVOUT_B, true, CKIN_X);
		resetModule(mod);
		testMod(mod, CVIN_Y, CVOUT_B, true, CKIN_X);
	}
}


void shm3()
{
	// channel 0
	{
		printf("shm 2, channel 0\n");
		DMSampleAndHoldTrackAndHoldZ mod;
		testMod(mod, CVIN_X, CVOUT_A, false, CKIN_Z);
		resetModule(mod);
		testMod(mod, CVIN_X, CVOUT_A, false, CKIN_Z);
	}
	{
		printf("shm 2, channel 1\n");
		DMSampleAndHoldTrackAndHoldZ mod;
		testMod(mod, CVIN_Y, CVOUT_B, true, CKIN_Z);
		resetModule(mod);
		testMod(mod, CVIN_Y, CVOUT_B, true, CKIN_Z);
	}
}




void shm5()
{
	printf("*** shm5\n");

	int count =0;
	DMSampleAndHoldProbabalistic shp;
	DModule * m = &shp;
	bool fired =testMod(*m, CVIN_Y, CVOUT_A, false, CKIN_X, false, FLAG_BISMINUSA);

	printf("*** shm5-2\n");
	resetModule(shp);
	printf("back from reset module\n");
	fired =testMod(*m, CVIN_Y, CVOUT_A, false, CKIN_X, false, FLAG_BISMINUSA);
	assert(fired);
}


void shm5b()
{
	printf("*** shm5b\n");

	int count =0;
	DMTrackAndHoldProbabalistic shp;
	DModule * m = &shp;

	bool fired =testMod(*m, CVIN_Y, CVOUT_A, true, CKIN_X, false, FLAG_BISMINUSA);
	assert(fired);

	resetModule(shp);
	fired =testMod(*m, CVIN_Y, CVOUT_A, true, CKIN_X, false, FLAG_BISMINUSA);
	assert(fired);
}



//x = clock
//y = CV
//z = probability (linear taper, 10% to 100%)
//
//a = s&h
//b = 1 s&h (inverted output)
//static double getProb(DMSampleAndHoldProbabalistic& shp, ZState& z)
static double getProb(DModule& shp, ZState& z)
{
	assert(z.value <= 0x3ff);
	int fired=0, didntFire=0;
	const int _10v = DACVoltage::xcodeForMV(10000);
	//ZState z(_z, true);
	for (int i=0; i < 100000; ++i)
	{
		int a=0, b=0;
		int y = i + 50;
		shp.go(false, 0, y, z, a, b);
		shp.go(false, _10v, y, z, a, b);
		if (a == y) ++fired;
		else didntFire++;
	}

	return (double) fired / (double)(fired + didntFire);
}




// test stochastic probability range
void shm6_sub(DModule& shp)
{
	//DMSampleAndHoldProbabalistic shp;

	// first initial probability
	ZState zNone(0x200, false);
	double p = getProb(shp, zNone);
	//printf("p rest = %f\n", p);
	assert(p == 1);


	ZState z(0x200);
	p = getProb(shp, z);
	//printf("p 200 = %f\n", p);
	assert(p > .3 && p < .7);


	z.value = 0x3ff;
	p = getProb(shp, z);
	assert(p == 1.0);				// strict

	z.value = 0;
	p = getProb(shp, z);
	//printf("p 0 = %f\n", p);
	assert(p < .13 && p > .08);		// loose tolerence on the 10%
	printf("smh6, tighten up low percent)\n");

	//
	resetModule(shp);
	p = getProb(shp, zNone);
	//printf("p rest = %f\n", p);
	assert(p == 1);
}

void shm6()
{
	printf("** shm6a\n");
	DMSampleAndHoldProbabalistic shp;
	shm6_sub(shp);
}


void shm6b()
{
	printf("** shm6b\n");
	DMTrackAndHoldProbabalistic shp;
	shm6_sub(shp);
}




/*

const int FLAG_BISMINUSA = 1;
const int FLAG_IGNOREOTHER = 2;
const int FLAG_CLOCKINVERTED = 4;

bool testMod(DModule& mod, CVIN cvin, CVOUT cvout, bool isTrackAndHold, CKIN ckin,
	bool isProbabalistic = false,
	int flag = 0)
	*/
void shm8()
{
	printf("shm8\n");
	DMDualFunctionQuantizerHelper mod;
	testMod(mod, CVIN_X, CVOUT_A, false, CKIN_Z, false, FLAG_IGNOREOTHER );
	resetModule(mod);
	testMod(mod, CVIN_X, CVOUT_A, false, CKIN_Z, false, FLAG_IGNOREOTHER );
}


void SampleAndHoldTest()
{

	sh0();
	sh1();
	sh2();
	sh3();
	sh4();
	sh5();
	sh6();

	shm1();
	shm2();
	shm3();
	shm4();

	shm5();
	shm5b();

	printf("skipping most shm\n");

	shm6();

	shm6b();
	//shm7();
	shm8();


	
}