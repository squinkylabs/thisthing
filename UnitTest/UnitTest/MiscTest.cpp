
#include "stdafx.h"
#include "DMMisc.h"
#include "DMTuner.h"

void mt1()
{
	DMZero dm;
	int a=5, b=10;

	ZState z(222, true);
	dm.go(false, 100, 111, z, a, b);
	assert(a == 0 && b == 0);
}


void mt2()
{
	DMConst dm(1, -1);
	int a=5, b=10;
	ZState z(222, true);
	dm.go(false, 100, 111, z, a, b);
	assert(a == 1 && b == -1);
}

#include <algorithm>

void tuner()
{
	DMTuner tun;
	int min=0;
	int max=0;

	bool wasNeg = false;
	int zeroCross=0;
	for (int i=0; i< Constants::DISTING_SAMPLE_RATE; ++i)
	{
		int x=0;
		int y=0;
		ZState z(0, true);
		int a,b;
		tun.go(false, x, y, z, a, b);

		min = std::min(min, a);
		max = std::max(max, a);

		if (a >= 0)
		{
			if (wasNeg) zeroCross++;
			wasNeg = false;
		}
		else wasNeg = true;
	}

	
	int v2 = DACVoltage::xcodeForMV(2000);
	int vm2 = DACVoltage::xcodeForMV(-2000);

	//printf("max = %d, v2=%d zero=%d\n", max, v2, zeroCross);

	assert(max <= v2);
	assert(min >= vm2);
	assert(max > v2 / 2);
	assert(zeroCross == 440);
	assert(min < vm2 /2);
}

void MiscTests()
{
	mt1();
	mt2();
	tuner();
}