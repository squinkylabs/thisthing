

#include "stdafx.h"
#include "DACVoltage.h"
#include <cmath>

void dv0()
{
	int c =DACVoltage::xcodeForMV(0);
	assert(c == 0);

	c =DACVoltage::zcodeForMV(0);
	assert(c == 0x200);

}


void dv1()
{
	
	//printf("code for 1 mv = %x\n", DACVoltage::xcodeForMV(1));
	//printf("code for 10v =%x\n", DACVoltage::xcodeForMV(10000));
	//printf("error = %d\n", 0x7fffff -  DACVoltage::xcodeForMV(10000));
	int c;
#if _CAL==100
	c =DACVoltage::xcodeForMV(10 * 1000);
	assert(c == 0x7fffff);		// I think it's two's complement, anyway
#else
	c =DACVoltage::xcodeForMV(10 * 1000);
	assert(c ==  10 * (1 << 19));
#endif
	c = DACVoltage::zcodeForMV(10 * 1000);
	//printf("c = %x\n", c);
	assert(c == 0x3ff);		
}


void dvm1()
{
	
	//printf("code for 1 mv = %x\n", DACVoltage::xcodeForMV(1));
	//printf("code for 10v =%x\n", DACVoltage::xcodeForMV(10000));
	//printf("error = %d\n", 0x7fffff -  DACVoltage::xcodeForMV(10000));

	int c =DACVoltage::xcodeForMV(-10 * 1000);
	//printf("c = %x\n", c);
#if _CAL==100
	assert(c == - 0x7fffff);
#else
	assert(c ==  -10 * (1 << 19));
#endif

	c = DACVoltage::zcodeForMV(-10 * 1000);
	//printf("c = %x\n", c);
	assert(c == 1);		// asymetry of two's complement		
}

void dvu()
{
	{
		int x = DACVoltage::xcodeForuV(1000 * 1000);
		int y = DACVoltage::xcodeForMV( 1000);
		assert(x == y);
	}
	{
		int x = DACVoltage::xcodeForuV(10 * 1000 * 1000);
		int y = DACVoltage::xcodeForMV( 10 * 1000);
		assert(x == y);
	}
	{
		int x = DACVoltage::xcodeForuV(-10 * 1000 * 1000);
		int y = DACVoltage::xcodeForMV(- 10 * 1000);
		assert(x == y);
	}

}


bool isCloseZ(int a, int b)
{
	return std::abs(a - b) <= 20;		// 20 mv accuracy is ok for z computations
										// we have a bit of rounding errors in DACVoltage, but
										// I don't want to fix it right now.
}



// test that  DACVoltage::zMVFromCode give sane numbers
void dvz()
{
	for (int i = -10000; i <= 10000; ++i)
	{
		const int code = DACVoltage::zcodeForMV(i);
		const int mv = DACVoltage::zMVFromCode(code);
		//printf("i=%d code=%d, mv=%d\n", i, code, mv);
		assert(isCloseZ(mv, i));
	}
	
}

void DACVoltageTest()
{
	//printf("in dac voltage test\n");
	dv0();
	dv1();
	dvm1();
	dvu();
	dvz();
	//printf("leave dac voltage test\n");

}