#include "stdafx.h"

#include "DMSwitch.h"

void st0()
{
	DMSwitch sw;
	DModule& dm = sw;

	int a, b;
	sw.go(false, 0, 0, ZState(0), a, b);
	assert(a == 0);
	assert(b == 0);
}

void resetModule(DModule& mod)
{
	int a;
	int x = DACVoltage::xcodeForMV(10000);
	
	mod.go(true, x, x, ZState(0x3ff, true), a, a);
}

void st1_sub(DModule& sw)
{
	
	ZState zlo(0);
	ZState zhi(0x3ff);

	int a, b;
	sw.go(false, 0, 0, zlo, a, b);
	sw.go(false, 1, 2, zhi, a, b);		// first trigger, x -> a
	assert(a == 1);

	sw.go(false, 1, 2, zlo, a, b);
	assert(a == 1);

	sw.go(false, 1, 2, zhi, a, b);		// first trigger, x -> a
	assert(a == 2);

	sw.go(false, 1, 2, zlo, a, b);
	assert(a == 2);

	sw.go(false, 1, 2, zhi, a, b);		// first trigger, x -> a
	assert(a == 1);
	
}

void st1()
{
	DMSwitch sw;
	DModule& dm = sw;

	st1_sub(sw);
	resetModule(sw);
	st1_sub(sw);



}

void SwitchTests()
{
	st0();
	st1();
}
