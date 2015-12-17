
#include "stdafx.h"
#include "FixedPoint.h"
#include "FixedPoint.h"

// test constants work
void fp1()
{
	FixedPoint<16> x = FixedPoint<16>::fromFloat(0.0f);
	float f = x.toFloat();
	assert(f==0);

	FixedPoint<16> y= FixedPoint<16>::fromFloat(1.0f);
	f = y.toFloat();
	assert(f==1);

	FixedPoint<16> z= FixedPoint<16>::fromInt(-1);
	f = z.toFloat();
	assert(f==-1);

	FixedPoint<16> q= FixedPoint<16>::fromFloat(-111.5f);
	f = q.toFloat();
	assert(f== -111.5);
}




// simple multiply, integer
template <int precission>
void _fp3()
{
	FixedPoint<precission> zero = FixedPoint<precission>::fromInt(0);
	FixedPoint<precission> one =  FixedPoint<precission>::fromInt(1);
	FixedPoint<precission> two =  FixedPoint<precission>::fromInt(2);
	FixedPoint<precission> negtwo =  FixedPoint<precission>::fromInt(-2);

	FixedPoint<precission> z = zero.mul(one);
	assert(z.toInt() == 0);
	z = one.mul(zero);
	assert(z.toInt() == 0);


	z = one.mul(one);
	assert(z.toInt() == 1);

	z = two.mul(two);
	assert(z.toInt() == 4);

	z = two.mul(negtwo);
	assert(z.toInt() == -4);

	z = negtwo.mul(two);
	assert(z.toInt() == -4);

	z = negtwo.mul(negtwo);
	assert(z.toInt() == 4);

}

void fp3()
{
	_fp3<16>();
	_fp3<15>();
	_fp3<4>();
	_fp3<17>();
	_fp3<27>();
}

// multiply
void fp4()
{
	FixedPoint<24> two =  FixedPoint<24>::fromInt(2);
	assert(two.toInt() == 2);

	FixedPoint<24> one =  FixedPoint<24>::fromInt(1);
	assert( one.mul(one).toInt() == 1);

	FixedPoint<24> z = two.mul(two);
	assert(z.toInt() == 4);
}


// add, subtract
void fp5()
{
	FixedPoint<24> two =  FixedPoint<24>::fromInt(2);
	FixedPoint<24> three =  FixedPoint<24>::fromInt(3);

	FixedPoint<24> z = two.add(three);
	assert(z.toInt() == 5);

	z = two.sub(three);
	assert(z.toInt() == -1);
}

// fractional mult
void fp6()
{
	FixedPoint<24> half =  FixedPoint<24>::fromFloat(.5);
	FixedPoint<24> three =  FixedPoint<24>::fromFloat(3.);

	FixedPoint<24> z = half.mul(three);
	assert(z.toFloat() == 1.5);
}

// mixed precission
void fp7()
{
	FixedPoint<24> three =  FixedPoint<24>::fromInt(3);
	FixedPoint<7> five =  FixedPoint<7>::fromInt(5);
	FixedPoint<30> ptfive =  FixedPoint<30>::fromFloat(.5f);


	FixedPoint<24> z = three.mulMixed(five);
	assert(z.toInt() == 15);

	z = three.mulMixed(ptfive);
	assert(z.toFloat() == 1.5f);

}

// regressions
void fp8()
{
	FixedPoint<2> neg_half =  FixedPoint<2>::fromFloat(-.5);
	FixedPoint<2> neg_q = neg_half.mul(neg_half);
	//printf("nq = %f\n", neg_q.toFloat());
	assert(neg_q.toFloat() == .25);
}


void FixedPointTest()
{
	fp1();

	fp3();
	fp4();
	fp5();
	fp6();
	fp7();
	fp8();

//	printf("try something crazy: 16:16 with 40000\n");
	FixedPoint<16> s = FixedPoint<16>::fromInt(32000); 

}
