

#include "stdafx.h"
#include "TriggerOutput.h"
#include "GateTrigger.h"
#include "SchmidtTrigger.h"
#include "SchmidtTrigger.h"
#include "DACVoltage.h"
#include "Random.h"

void sc0()
{
	SchmidtTrigger sc(-10, 10);
	bool b = sc.go(-20);
	assert(!b);
}


void sc1()
{
	SchmidtTrigger sc(-10, 10);
	bool b = sc.go(20);
	assert(b);
}


void sc2()
{
	SchmidtTrigger sc(-10, 10);
	bool b = sc.go(20);
	assert(b);
	b = sc.go(9);
	assert(b);
	b = sc.go(-9);
	assert(b);
	b = sc.go(-11);
	assert(!b);

	b = sc.go(-999);
	assert(!b);

	b = sc.go(9);
	assert(!b);
	b = sc.go(11);
	assert(b);
}


// check that threshold accessors are sane

void g_1(bool useZ)
{
	GateTrigger g(useZ);
	assert( g.thlo() > 0);
	assert( g.thhi() > g.thlo());

	const int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	assert( g.thhi() < ten);
	const int one = DACVoltage::codeForMV(useZ, 1 * 1000);
	assert( g.thhi() > one);
}

void g0(bool useZ)
{
	GateTrigger g(useZ);

	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	g.go(zero);
	assert(!g.gate());
	g.go(ten);
	assert(g.gate());
}


void g1(bool useZ)
{
	GateTrigger g(useZ);

	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	g.go(zero);
	assert(!g.gate());
	assert(!g.trigger());
	g.go(ten);
	assert(g.gate());
	assert(g.trigger());

	g.go(ten);
	assert(g.gate());
	assert(!g.trigger());
}


// test the schmids work in gate
void g2(bool useZ)
{
	GateTrigger g(useZ);

	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int mid = DACVoltage::codeForMV(useZ, 2500);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	assert(g.thlo() < mid);		// test assumes this
	assert(g.thhi() > mid);		// test assumes this

	g.go(zero);
	assert( !g.gate());
	g.go(mid);
	assert( !g.gate());
	g.go(ten);
	assert( g.gate());
	g.go(mid);
	assert( g.gate());
	g.go(zero);
	assert( !g.gate());
}

class Always : public ConditionalGateTrigger
{
public:
	Always(bool useZ) : ConditionalGateTrigger(useZ) {}
	virtual bool allowGate() { return true; }
};

// test conditional when condition is always true (acts like normal)
void cg1(bool useZ)
{
	Always g(useZ);

	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	for (int i=0; i<10; ++i)
	{
	g.go(zero);			// was 0
	assert(!g.gate());
	g.go(ten);
	assert(g.gate());
	}
}


class Never : public ConditionalGateTrigger
{
public:
	Never(bool useZ) : ConditionalGateTrigger(useZ) {}
	virtual bool allowGate() { return false; }
};


// test conditional when condition is always true (acts like normal)
void cg2(bool useZ)
{
	Never g(useZ);

	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	for (int i=0; i<10; ++i)
	{
	g.go(zero);
	assert(!g.gate());
	g.go(ten);
	assert(!g.gate());
	}
}




class Half : public ConditionalGateTrigger
{
public:
	Half(bool useZ) : ConditionalGateTrigger(useZ) {}
	virtual bool allowGate()
	{ 
		return _r.get() & 1;
	}
private:
	Random _r;
};

// test conditional when condition is always true (acts like normal)
void cg3(bool useZ)
{
	Half g(useZ);

	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	int count=0;
	for (int i=0; i<100; ++i)
	{
	g.go(zero);
	assert(!g.gate());
	g.go(ten);
	//assert(!g.gate());
	if (g.gate()) ++count;
	}
	//printf("count was %d\n", count);
	//getchar();
	assert(count > 30);
	assert(count < 70);

}





// test conditional when condition prob is 50%
void cg4()
{
	//printf("gc4\n");
	RandomGateTrigger g(false);	
	g.setPercent(50);

	int ten = DACVoltage::xcodeForMV( 10 * 1000);
	int zero =  DACVoltage::xcodeForMV(0);

	int count=0;
	int z= 0x200;
	for (int i=0; i<100; ++i)
	{
		g.go(zero);
		assert(!g.gate());
		g.go(ten);
	//assert(!g.gate());
		if (g.gate()) ++count;
	}
	printf("cg4 count was %d\n", count);
	//getchar();
	assert(count > 30);
	assert(count < 70);

}


void cg_sub(int percent)
{
	//printf("\n*** run with %d %d p=%d\n", min, max, pot);
	RandomGateTrigger g(false);
	g.setPercent(percent);

	int ten = DACVoltage::xcodeForMV( 10 * 1000);
	int zero =  DACVoltage::xcodeForMV(0);
	
	const int times = 10000;
	int count=0;
	for (int i=0; i<times; ++i)
	{
		g.go(zero);
		assert(!g.gate());
		g.go(ten);
	//assert(!g.gate());
		if (g.gate()) ++count;
	}
	int expectedCount = (times * percent) / 100;
	

	double ratio = (double) expectedCount / (double) count;

	if (ratio > 1.1 || ratio < .9)
		printf("percent %d count was %d expected = %d ratio = %f\n", percent, count, expectedCount, ratio);

	assert(ratio < 1.1);
	assert(ratio > .9);
}


void cg5()
{
	
	cg_sub(10);			// 0..100 on pot, run at 0 (fudge by passing expected 1%... rounding errors..
	cg_sub(100);			//0.100 on pot, run at max
	cg_sub(50);

	cg_sub( 40);
	cg_sub( 60);
	//printf("getchar cg5\n");
	//getchar();
}


#if 0	// good model for module test
void cg_sub(int min, int max, int pot, int percent)
{
	//printf("\n*** run with %d %d p=%d\n", min, max, pot);
	RandomGateTrigger g(false);

	int ten = DACVoltage::xcodeForMV( 10 * 1000);
	int zero =  DACVoltage::xcodeForMV(0);
	
	const int times = 10000;
	int count=0;
	for (int i=0; i<times; ++i)
	{
		g.go(zero,pot);
		assert(!g.gate());
		g.go(ten,pot);
	//assert(!g.gate());
		if (g.gate()) ++count;
	}
	int expectedCount = (times * percent) / 100;
	

	double ratio = (double) expectedCount / (double) count;
#if (VERBOSE > 1)
	printf("count was %d expected = %d ratio = %f\n", count, expectedCount, ratio);
#endif
	assert(ratio < 1.1);
	assert(ratio > .9);
}


void cg5()
{
	
	cg_sub(0, 100, 0, 1);			// 0..100 on pot, run at 0 (fudge by passing expected 1%... rounding errors..
	cg_sub(0, 100, 0x3ff, 100);			//0.100 on pot, run at max
	cg_sub(0, 100, 0x200, 50);

	cg_sub(40, 60, 0, 40);
	cg_sub(40, 60, 0x3ff, 60);
	//printf("getchar cg5\n");
	//getchar();
}
#endif





void to0()
{
	TriggerOutput t;

	const int ghigh = DACVoltage::xcodeForGateHi();
	t.go(false);
	assert(t.get() == 0);

	t.go(true);
	assert(t.get() ==ghigh);

	int duration=0;
	for (int i=0; i< 100000; ++i)
	{
		t.go(true);
		if (t.get() == 0)
			break;
		++duration;
	}
	printf("duration = %d\n", duration);
	assert(duration > 385);
	assert(duration < 395);	
}


void testAfterReset(GateTrigger& g, bool useZ)
{
	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	g.go(ten);				// right after "reset", start with gate
	assert(!g.gate());
	assert(!g.trigger());

	g.go(zero);
	assert(!g.gate());
	assert(!g.trigger());

	g.go(ten);
	assert(g.gate());
	assert(g.trigger());

	
}

void grst1()
{
	printf("grst\n");
	const bool useZ = false;
	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);

	GateTrigger g(useZ);
	testAfterReset(g, useZ);

	g.go(ten);
	g.reset();
	testAfterReset(g, useZ);


}


void testAfterReset(RandomGateTrigger& g, bool useZ)
{
	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	int zero =  DACVoltage::codeForMV(useZ, 0);

	g.go(ten);				// right after "reset", start with gate
	assert(!g.gate());
	assert(!g.trigger());

	g.go(zero);
	assert(!g.gate());
	assert(!g.trigger());

	g.go(ten);
	assert(g.gate());
	assert(g.trigger());

	
}

void grst2()
{
	printf("grst2\n");
	const bool useZ = false;
	RandomGateTrigger g(useZ);
	g.setPercent(100);			

	testAfterReset(g, useZ);

	printf("grst2-b\n");
	int ten = DACVoltage::codeForMV(useZ, 10 * 1000);
	
	g.reset();
	testAfterReset(g, useZ);
}

/*
void resetModule(DModule * mod)
{
	assert(false);
}

// test probability vs pot
void pm0_sub(DMSampleAndHoldProbabalistic& mod)
{
	assert(false);
}
void pm0();
{
	DMSampleAndHoldProbabalistic mod;
	pm0_sub(mod);
	resetModule(mod);
	pm0_sub(mod);
}
*/



void GateTriggerTests()
{
	sc0();
	sc1();
	sc2();

	for (int i=0; i<=1; ++i)
	{
		const bool b = !!i;
		g_1(b);
		g0(b);
		g1(b);
		g2(b);
		cg1(b);


		cg2(b);
		cg3(b);
	}

	

	cg4();
	cg5();
	to0();
	grst1();
	grst2();


	printf("low probability in random gate trigger doesn't work\n");
	
}
