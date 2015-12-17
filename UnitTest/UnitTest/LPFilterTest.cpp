
#include "stdafx.h"

#include "LowPassFilter_dbl.h"
#include "LowPassFilter_dbl.h"
#include "DMTest.h"

#include <cmath>
#include <math.h>

template <class T>
void f0()
{
	T f;
	assert(f.get() == 0);
}

template <class T>
void f1()
{
	T f;
	f.set(1, 4);	// fc  = fs / 4
	assert(f.get() == 0);
	f.go(10000);
	assert(f.get() > 0);
}

template <class T>
void f2()
{
	const int input = 10000;
	T f;
	f.set(1, 4);	// fc  = fs / 4
	assert(f.get() == 0);

	for (int i=0; i<100000; ++i)
	{
		f.go(input);
		if (f.get() > input-10 && f.get() < input + 10)
		{
			return;
		}
	}
	assert(false);
}


	


// get Tc of filter, in samples
template <class T>
int findT(T& f)
{
	assert(f.get() == 0);

	// us an input that won't blow up 16.16. filter
	const int input = 30000;
//	const int input = 100000;
	bool ok = false;

	const int target = (int) (input * (1.0 - 1.0/Constants::E));
	//printf("input = %d, target=%d\n", input, target);

	int samples;
	for ( samples=0; samples<100000000; ++samples)
	{
		f.go(input);
		//printf("in loop, samp=%d, out = %d\n",samples, f.get());
		if (f.get() > target)
		{
			ok = true;
			break;
		}
	}
	if (!ok) printf("Ginving up in findT, target = %d, filter = %d\n", target, f.get());
	assert(ok);
	return samples;

}

// test that filter is accurate at the specified freq
template <class T>
bool isAccurate(int fc, int fs)
{
	
	T f;

	f.set(fc, fs);	// fc  = fs / 100

	// measured time const
	const int t = findT(f);

	// what it should be
	const double calc = (double) fs / (2.0 * Constants::PI * fc); 

	const double error = std::abs(t -calc);
	
	printf("in isAccurate, fs = %d, calc=%f, t=%d error = %f\n", fs, calc, t, error);
	fflush(stdout);
	return  (error < (.1 * calc));
}

template <class T>
void f3()
{
	assert( isAccurate<T>(1, 100));
}


template <class T>
void f4()
{
	printf("f4 test\n");
	assert( isAccurate<T>(1, 70000));		// 1hz at 70k

	printf("SKIPPING SUPER LF TEST - NEED NEW TEST FOR FP\n");
	//assert( isAccurate<T>(1, 70000 * 60)); // 1/60 herts
}



void f5()
{
	DMFilter f;
}


template <class T>
void _LPFilter()
{
	//f1();
	//printf("press the any key\n");
	//getchar();

	f0<T>();
	f1<T>();
	f2<T>();
#ifdef _SLOWTEST
	printf("running slow LPF tests\n");
	f3<T>();
	f4<T>();
#endif
	f5();
}

void LPFilter()
{
	printf("testing double filter\n");
	_LPFilter<LowPassFilter_dbl>();

#if 0
	printf("testing 16 filter\n");
	_LPFilter<LowPassFilter_16>();
#endif

	printf("testing f filter\n");
	_LPFilter<LowPassFilter_f>();
	printf("!!!!! did filter !!!!\n");
}

