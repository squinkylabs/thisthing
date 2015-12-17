
#include "stdafx.h"
#include "DMSlopeDetector.h"
#include "SlopeDetector.h"
#include "SlopeDetector.h"
#include "DACVoltage.h"
#include "DMSlopeDetector.h"
#include "TestWrappers.h"

//#define _HACK

/** Test harness
 */


void sd0()
{
#if (_VERBOSE > 0)
	printf("sd0\n");
#endif
	SlopeDetector sd(20, 1000, 70000, true);
	assert( !sd.getPositiveSlope());
	assert( !sd.getNegativeSlope());
	assert( !sd.getBothSlope());

}


// POSITIVE
void sd1()
{
#if (_VERBOSE > 0)
	printf("sd1\n");
#endif
	SlopeDetector sd(20, 1000, 70000, true);					// 20 mv thresh
	int input = DACVoltage::xcodeForMV(1000);		// huge step
	sd.go(input);
	assert( sd.getPositiveSlope() );
	assert( !sd.getNegativeSlope());
	assert( sd.getBothSlope());
}

// below thresh
void sd2()
{
#if (_VERBOSE > 0)
	printf("sd2\n");
#endif
	SlopeDetector sd(20, 1000, 70000, true);					//210 mv thresh
	int input = DACVoltage::xcodeForMV(10);		// small step
	sd.go(input);
	assert( !sd.getPositiveSlope() );
	assert( !sd.getNegativeSlope());
	assert( !sd.getBothSlope());
}


// neg
void sd3()
{
#if (_VERBOSE > 0)
	printf("sd3\n");
#endif
	SlopeDetector sd(20, 1000, 70000, true);					// 20 mv thresh
	int input = DACVoltage::xcodeForMV(1000);		// huge step
	sd.go(-input);
	assert( !sd.getPositiveSlope() );
	assert( sd.getNegativeSlope());
	assert( sd.getBothSlope());
}

class Note
{
public:
	int midiPitch;
	int durationMS;
};

Note chromatic[] = {
	{100, 1000 },
	{101, 1000 },
	{102, 1000 },
	{103, 1000 },
	{104, 1000 },
	{105, 1000 },
	{0,0 }
};

Note trill[] = {
	{1, 1000 },
#ifndef _HACK
	{2, 1000 },
	{1, 1000 },
	{2, 1000 },
	{1, 1000 },
	{2, 1000 },
#endif
	{0,0 }
};


Note leap[] = {
	{20, 1000 },
#ifndef _HACK
	{80, 1000 },
	{81, 1000 },
	{20, 1000 },
	{21, 1000 },
	{20, 1000 },
#endif
	{0,0 }
};


Note leapfast[] = {
	{20, 100 },
#ifndef _HACK
	{80, 100 },
	{81, 100 },
	{20, 100 },
	{21, 100 },
	{20, 100 },
#endif
	{0,0 }
};

void testSeq(I_Test& sd, Note * seq, int sampleRate, const char * msg)
{
	// validate test inputs
	Note * p = seq;
	for (bool done=false; !done; ++p)
	{
		if (p->durationMS == 0)
			done=true;
		else
		{
			assert(p->midiPitch >= 0);
			assert(p->midiPitch <= 127);
			assert(p->durationMS > 0);
			assert(p->durationMS < 10000);
		}
	}

	//for( ; seq->durationMS!=0; ++seq)
	for (int step=0; seq[step].durationMS != 0; ++step)
	{

		int mv = (1000 * seq[step].midiPitch) / 12;
		int input = DACVoltage::xcodeForMV(mv);	

#if (_VERBOSE > 1)
		printf("top of loop in testSeq step = %d input = %d pitch = %d mv=%d\n", step, input, seq[step].midiPitch, mv);
#endif
		
		
		// numer of samples in which we must detect 
		int samples = (seq[step].durationMS * sampleRate) / 1000;


		// becuase of gibbs filter, we may need a few to detect high
		int output=false;
		for (int i=0; i< Constants::SLOPE_TEST_DELAY; ++i)		// was 20
		{
			 output = sd.doIt(input);
			 if (output)
			 {

#if (_VERBOSE > 1)
				printf("detected change on sample %d\n", i);
#endif
				samples -= i;		// we used up some, so take tehm into account
				break;
			 }
		}

		// make sure we detected the change
		if (!output)
		{
			printf("failing due to missed input[%d] %s\n", step, msg);
		}
		assert(output);

		// now make sure the detector clears
		for (int i=0; i<samples; ++i)
		{
#ifdef _HACK
			if (0 == (i % 1000))
			{
				printf("count = %d\n", i);
				sd.print_go(msg, input);
			}
			else
#endif
			//sd.go(input);		// keep clocking in the same input
			output = sd.doIt(input);
		}
		if (output)
		{
			printf("failing becuase detector still firing. step=%d\n", step);
		}
		assert(!output);	// should be recoverd

	}
}

void sd4()
{
#if (_VERBOSE > 0)
	printf("sd4\n");
#endif
	int sampleRate = 70000;
	SlopeDetector _sd(20, 1000, sampleRate, true);					// 20 mv thresh
	SDWrap sd(_sd);
	testSeq(sd, chromatic, sampleRate, "chrom, 1");
}


void sdm4()
{
#if (_VERBOSE > 0)
	printf("sd4m\n");
#endif
	int sampleRate = 70000;
	DMSlopeDetectorDual _sd;					// 20 mv thresh
	SDMWrap sd(_sd);
	//printf("smd4 using wrong stim\n");
	//testSeq(sd, leap, sampleRate, "leap, 1");
	testSeq(sd, chromatic, sampleRate, "chrom, 1");
}

void sd5()
{
#if (_VERBOSE > 0)
	printf("sd5\n");
#endif
	int sampleRate = 70000;
	SlopeDetector _sd(20, 1000, sampleRate, true);					// 20 mv thresh
	SDWrap sd(_sd);
	testSeq(sd, trill, sampleRate, "trill 1");
}


void sd6()
{
#if (_VERBOSE > 0)
	printf("sd6\n");
#endif
	int sampleRate = 70000;
	SlopeDetector _sd(20, 1000, sampleRate, true);					// 20 mv thresh
	SDWrap sd(_sd);
	testSeq(sd, leap, sampleRate, "leap 1");
}


// try when the detector is slower
// trill fails at 650, works 600
void sd7()
{
#if (_VERBOSE > 0)
	printf("sd7\n");
#endif
	int sampleRate = 70000;
	SlopeDetector _sd(20, 600 * 1000, sampleRate, true);					// 20 mv thresh
	SDWrap sd(_sd);
	testSeq(sd, trill, sampleRate, "trill 159");
}


// try when the detector is slower
// leap fails at  200, works 180
void sd8()
{
#if (_VERBOSE > 0)
	printf("sd8\n");
#endif
	int sampleRate = 70000;
	SlopeDetector _sd(20, 180 * 1000, sampleRate, true);					// 20 mv thresh
	SDWrap sd(_sd);
	testSeq(sd, leap, sampleRate, "leap 159");
}


// try when the detector is slower
// leap fails at  20 works at 18

// now with fixed point fails at 18
void sd9()
{
#if (_VERBOSE > 0)
	printf("sd9\n");
#endif
	int sampleRate = 70000;
	SlopeDetector _sd(20, 15 * 1000, sampleRate, true);					// 20 mv thresh
	SDWrap sd(_sd);
	testSeq(sd, leapfast, sampleRate, "leap 159");
	//printf("getchar finished sd9\n");
	//getchar();
}

void sdm9()
{
#if (_VERBOSE > 0)
	printf("sdm9\n");
#endif
	int sampleRate = 70000;
//	SlopeDetector _sd(20, 18 * 1000, sampleRate, true);					// 20 mv thresh
	DMSlopeDetectorDual _sd;
	SDMWrap sd(_sd);
	testSeq(sd, leapfast, sampleRate, "leap 159 m");
	//printf("getchar finished sd9\n");
	//getchar();
}


void sdm0()
{
#if (_VERBOSE > 0)
	printf("sdm0\n");
#endif
	DMSlopeDetectorDual sd;
	DModule * dm = &sd;

	ZState z(0, true);
	int a=-1, b=-1;
	dm->go(false, 0, 0, z, a, b);
	assert(a == 0 && b == 0);
}


void sdm1()
{
#if (_VERBOSE > 0)
	printf("sdm1\n");
#endif
	DMSlopeDetectorDual sd;

	int a=-1, b=-1;
	
	int v = DACVoltage::xcodeForMV(100);		// emough to trigger
	int v5 = DACVoltage::xcodeForMV(5 * 1000);
	ZState z(0, true);
	sd.go(false, 0, 0, z, a, b);
	assert(a == 0 && b == 0);

	for (int i=0; i< Constants::SLOPE_TEST_DELAY; ++i)		// delay for gibbs filter
	{
		sd.go(false, v, 0, z, a, b);
		if (a > v5 && b == 0)
			break;
	}
	//printf("went high, a=%d\n", a);
	assert(a > v5 && b == 0);

	for (int i=0; i<Constants::SLOPE_TEST_DELAY; ++i)		// delay for gibbs filter
	{
		sd.go(false, v, v, z, a, b);
		if (a > v5 && b > v5)
			break;
	}
	assert(a > v5 && b > v5);

}

extern void hack();
void SlopeDetectorTests()
{
	printf("slope detect test\n");

	sd0();
	sd1();
	sd2();
	sd3();
	sd4();
	sdm4();
	sd5();
	sd6();

#ifdef _SLOWTEST
	sd7();
	sd8();
	sd9();

	sdm9();
#endif

	sdm0();
	sdm1();

}

void hack()
{
	int sampleRate = 70000;
	SlopeDetector sd1(20, 159*1000, sampleRate, true);					// 20 mv thresh
	SlopeDetector sd2(20, 159*1000, sampleRate, true);					// 20 mv thresh

	int mv1 = (1000 * 1) / 12;		// midi pitch 1
	int input1 = DACVoltage::xcodeForMV(mv1);	

	int mv2 = (1000 * 100) / 12;		// midi pitch 100
	int input2 = DACVoltage::xcodeForMV(mv2);	

	bool done=false;
	for(  int count=0; !done; ++count )
	{
		if (0 == count % 1000)
		{
			printf("count = %d\n", count);
			sd1.print_go("sd1", input1);
			sd2.print_go("sd2", input2);
		}
		else
		{
			sd1.go(input1);
			sd2.go(input2);
		}

		if (!sd1.getBothSlope() && !sd2.getBothSlope())
		{
			done = true;
			printf("Hack finished at count = %d\n",  count);
		}
	}
}