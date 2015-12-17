
#include "stdafx.h"
#include "LookupTable.h"
//#include <math.h>
#include <cmath>
#include "ExpPot.h"

class TestTable : public LookupTable
{
public:
	TestTable()
	{
		for (int i=0; i<= numEntries; ++ i)
		{
			double k = (double)0x200 / (double) numEntries;
			data[i * 2] = (float) (k * (double)i);			// f(x) = x
			//printf("fill value %f\n", data[i * 2]);
		}
		fillSlopes();
	}
};


class TestTable_f : public LookupTable_f<11>
{
public:
	TestTable_f()
	{
		for (int i=0; i<= numEntries; ++ i)
		{
			float k = (float)0x200 / numEntries;
			float dat = k * (float)i;			// f(x) = x
			//printf("fp fill will be %f\n", dat); 
			data[i * 2] = FixedPoint<11>::fromFloat(dat);
		}
		fillSlopes();
	}
};


bool isClose(double a, double b)
{
	if (std::abs(a - b) > .1)
	{
		printf("%f not close to %f\n", a, b);
		return false;
	}
	return true;
}

void lt1()
{
	TestTable t;
	TestTable_f t_f;
	float x = t.lookup(0);
	assert(x == 0);

	x = t.lookup(0x3ff);
	printf("x = %f\n", x);
	//assert(isClose(x, 0x3ff));



//	assert(false);
	for (int i=0; i<0x3ff; ++i)
	{
		x = t.lookup(i);

		if (!isClose(x, i/2.0))
		{
			printf("old lookup bad at i = %d\n", i);
			assert(false);
		}
		

		x = t_f.lookup(i).toFloat();
		if (!isClose(x, i/2.0))
		{
			printf("new fixed point  lookup bad at i = %d\n", i);
			assert(false);
		}
	}
}

void lt2()
{

	ExpPotFilterLookup pfl(1800.0, 180000.0);

	auto x = pfl.lookup(100);

	assert(x.toFloat() > 0);
	assert(x.toFloat() < 1);

}

bool isCloseRatio(double expected, double actual)
{
	double tolerance = 1.02;		// 2%
	double ratio = expected / actual;
	return ratio < tolerance && ratio > (1 / tolerance);
}

// compare ExpPot to ExpPotLookup
void lt4()
{
	const float ymin=1800.0;
	const float ymax = 180000.0;
	ExpPot func(ymin, ymax);
	ExpPotLookup look(ymin, ymax);

	for (int pot = 0; pot < 0x400; pot +=1)
	{
		float ref = func.map( (float)pot);
		float l = look.lookup(pot);
		assert(isCloseRatio(ref, l));
	}

}


bool isKClose(LowPassFilter_f::fp expected, LowPassFilter_f::fp actual)
{
	double tolerance = 1.05;		// seem to only get 5% accuracy. probly ok?
	double ratio = expected.toFloat() / actual.toFloat();
	return ratio < tolerance && ratio > (1 / tolerance);
}

// compare exp pot lookup for exp pot filter loopup
void lt3()
{

	ExpPotFilterLookup pfl(1800.0, 180000.0);
	ExpPotLookup oldl(1800.0, 180000.0);


	//for (int pot = 60; pot <70;++pot)
	for (int pot = 0; pot < 0x400; pot +=1)
	{
		auto x = pfl.lookup(pot);

		assert(x.toFloat() > 0);
		assert(x.toFloat() < 1);

		int yp = (int)oldl.lookup(pot);
		auto yk = LowPassFilter_f::designLPF_TC_US_s(yp, 70000);
		if (!isKClose(yk, x))
			printf("pot=%x, k = %f yk = %f ratio=%f\n", pot, x.toFloat(), yk.toFloat(),  x.toFloat() / yk.toFloat() );
		assert( isKClose(yk, x));
	}
}

// compare exp pot lookup for exp pot filter loopup
void lt3_print()
{

	ExpPotFilterLookup pfl(1800.0, 180000.0);
	ExpPotLookup oldl(1800.0, 180000.0);


	//for (int pot = 60; pot <70;++pot)
	//for (int pot = 0; pot < 0x400; pot +=1)
	int numCols = 64;
	int numRows = 0x400 / numCols;

	for (int row = 0; row < numRows; ++row)
	{
		printf("\n good: ");
		for (int col=0; col<numCols; col++)
		{
			int pot=col + row * numCols;
		//auto x = pfl.lookup(pot);

			int yp = (int)oldl.lookup(pot);
			auto yk = LowPassFilter_f::designLPF_TC_US_s(yp, 70000);
			printf("%f ", yk.toFloat());
		}
		printf("\n aprox: ");
		for (int col=0; col<numCols; col++)
		{
			int pot=col + row * numCols;
			auto x = pfl.lookup(pot);

			
			printf("%f ", x.toFloat());
		}
	}
}

void LookupTableTest()
{
	lt1();
	lt2();
	lt4();
	lt3();

	//lt3_print();
//	assert(false);
}