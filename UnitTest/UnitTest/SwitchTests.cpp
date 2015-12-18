#include "stdafx.h"
#include <algorithm>

#include "DMSwitch.h"
extern int led_test, led_mode;

void st0()
{
	DMSwitch sw;
	DModule& dm = sw;
	assert(led_test == -1);

	int a, b;
	sw.go(false, 0, 0, ZState(0), a, b);
	assert(a == 0);
	assert(b == 0);

	assert(led_test == -1);
}

void resetModule(DModule& mod)
{
	int a;
	int x = DACVoltage::xcodeForMV(10000);
	
	mod.go(true, x, x, ZState(0x3ff, true), a, a);
}

bool within8(int a, int b)
{
	return std::abs(a -b ) <=8;
}

void st1_sub(DModule& sw, bool test_led)
{

	if (!test_led) printf("TODO: reset led state\n");
	ZState zlo(0);
	ZState zhi(0x3ff);

	const int c1 = ChromaticQuantizer::middleCV;
	const int c2 = ChromaticQuantizer::middleCV + ChromaticQuantizer::octaveV;

	if (test_led) assert(led_test == -1);
	int a, b;
	sw.go(false, 0, 0, zlo, a, b);
	sw.go(false, c1, c2, zhi, a, b);		// first trigger, x -> a
	assert(a == c1);
	assert(b == c1);
	
	assert(led_test == 1);
	if (test_led) assert(led_mode == 1);

	sw.go(false, c1, c2, zlo, a, b);
	assert(a == c1);
	assert(b == c1);

	sw.go(false, c1, c2, zhi, a, b);		// secon trigger, y -> b
	//printf("here a = %d b=%d c1=%d, c2 = %d b-c2=%d\n", a, b, c1, c2, b-c2);
	assert(a == c2);
	assert(within8(b, c2));
	if (test_led) assert(led_test == 2);

	sw.go(false, c1, c2, zlo, a, b);
	assert(a == c2);
	assert(within8(b, c2));

	sw.go(false, c1, c2, zhi, a, b);		// first trigger, x -> a
	assert(a == c1);
	assert(b == c1);
	if (test_led) assert(led_test == 1);


	sw.go(false, c1, c2, zlo, a, b);		//hold still, x - a
	assert(a == c1);
	assert(b == c1);

	sw.go(false, c1+1, c2+1, zlo, a, b);		//hold still, x - a
	assert(a == c1+1);
	assert(b == c1);
	
}

void st1()
{
	printf("st1-a\n");
	DMSwitch sw;
	DModule& dm = sw;

	st1_sub(sw, true);
	resetModule(sw);
	printf("st1-b\n");
	st1_sub(sw, false);



}

void SwitchTests()
{
	led_test = -1;
	st0();
	st1();
}
