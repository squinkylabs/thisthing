
#include "stdafx.h"

#include "DMRandomSource1.h"
#include "DMRandomSource1.h"
#include "LFO.h"
#include "LFO.h"
#include "ExpPot.h"
#include "ExpPot.h"


#include <set>


#include <algorithm>

static void t0()
{
	LFO l;
	l.set(1, 10);		// should cycle in 10

	int lastX=0;
	int maxX=0;
	int minX = 0;
	// goes up for firve, then back down
	for (int i=0; i<10; ++i)
	{
		int x=l.getTri();
		//printf("i=%d x = %d last = %d\n", i, x, lastX);
		if (i != 0)
		{
			if (i <= 5) assert(x > lastX);
			else assert(x < lastX);
		}
		
		maxX = std::max(maxX, x);
		minX = std::min(minX, x);	

		l.go();
		lastX = x;
	}
	assert(maxX >=  (9.0 / 10.0) * 0x7fffffff);
	
	int expectedMin = (int)( (9.0 / 10.0) * (int)0x80000000);
#if (VERBOSE > 1)
	printf("min=%d, expect%d\n", minX, expectedMin);
#endif
	assert(minX <=  expectedMin);

}

void t1()
{
	DMRandomSource1 rs(30);
	rs.setFreq(1);
	std::set<int> all;
	ZState z;
	for (int i=0; i<10; ++i)
	{
		int a, b;
		rs.go(false, 0, 0, z, a, b);
		//printf("a = %d, b=%d\n", a, b);
		assert( all.find(a) == all.end());
		assert( all.find(b) == all.end());
		all.insert(a);
		all.insert(b);

	
	}
	//getchar();
}

#if 0
void t2()
{
	DMRandomSource1 rs;
	rs.setFreq(1, 70000);
	std::set<int> all;
	for (int i=0; i<10; ++i)
	{
		int a, b;
		rs.go(0, 0, 0, a, b);
		printf("a = %d, b=%d\n", a, b);
		assert( all.find(a) == all.end());
		assert( all.find(b) == all.end());
		all.insert(a);
		all.insert(b);

	
	}
	getchar();
}
#endif


// LOF at low freq
static void t2()
{
	LFO l;
	l.set(1, 70000);		// should cycle in 10

	int lastX=0;
	int maxX=0;
	int minX = 0;
	// goes up, then back down
	for (int i=0; i<70000; ++i)
	{
		int x=l.getTri();
		//printf("i=%d x = %d last = %d\n", i, x, lastX);
		if (i != 0)
		{
			if (i <= 35000) assert(x > lastX);
			else assert(x < lastX);
		}
		
		maxX = std::max(maxX, x);
		minX = std::min(minX, x);	

		l.go();
		lastX = x;
	}
	assert(maxX >=  (9.9 / 10.0) * 0x7fffffff);
	
	int expectedMin = (int)( (9.9 / 10.0) * (int)0x80000000);
	//printf("min=%d, expect%d\n", minX, expectedMin);
	assert(minX <=  expectedMin);
}

// LOF at low freq
static void t3()
{
	DMRandomSource1 l(70000);
//	l.set(1, 70000);		// should cycle in 10

	int lastX=0;
	int maxX=0;
	int minX = 0;
	ZState z;
	// goes up, then back down
	for (int i=0; i<70000; ++i)
	{
		int a, b;
		l.go(false, 0,0,z, a, b);
		//int x=l.getTri();
	
			int x = a;
		printf("i=%d x = %d last = %d\n", i, x, lastX);

	
		if (i != 0)
		{
			if (i <= 35000) assert(x > lastX);
			else assert(x < lastX);
		}
		
		maxX = std::max(maxX, x);
		minX = std::min(minX, x);	

	//	l.go();
		lastX = x;
	}
	assert(maxX >=  (9.9 / 10.0) * 0x7fffffff);
	
	int expectedMin = (int)( (9.9 / 10.0) * (int)0x80000000);
	//printf("min=%d, expect%d\n", minX, expectedMin);
	assert(minX <=  expectedMin);
}


void t4()
{
	printf("r1:\n");
	DMRandomSource1 l(70000);
	int a, b;
	ZState z;
	l.go(false, 0,0,z, a, b);
	l.go(false, 0,0,ZState(0x200), a, b);
	l.go(false, 0,0,ZState(0x3ff), a, b);

	printf("r2:\n");
	DMRandomSource2 l2(70000);
	//int a, b;
	l2.go(false, 0,0,ZState(0), a, b);
	l2.go(false, 0,0,ZState(0x200), a, b);
	l2.go(false, 0,0,ZState(0x3ff), a, b);
	printf("getchar t4\n");
	getchar();
}



void t5()
{
	{
	ExpPot p(1, 10);
	assert(p.map(0) == 1);
	assert( p.map(0x3ff) == 10);

	assert( p.map(0x200) < 5);
	assert( p.map(0x200) > 1);
	}

	ExpPot p2(1, 1.1f);
	assert (p2.map(0) == 1);
	assert (p2.map(0x3ff) == 1.1f);

}


bool isClose(float a, float b)
{
	float ratio = a/b;
	bool ret =  ratio > .98 && ratio < 1.02;

	//printf("isClose on %f, %f r=%f\n", a, b, ratio);
	if (!ret)
	{
		printf("isClose failing on %f, %f\n", a, b);
	}
	return ret;
}

void t6()
{
	ExpPotLookup l(1, 10);
	assert( isClose(l.map(0), 1));
	assert( isClose(l.map(0x3ff), 10));
}


void t7()
{
	ExpPot p(1, 100);
	ExpPotLookup l(1, 100);
#if 1
	for (int i=0; i<0x400; ++i)
	{

		assert( isClose(l.map(i), p.map((float)i)));
	}
#else
	for (int i=0; i<(0x400 / 14); ++i)
	{
		printf("i = %d\n", i);
		 isClose(l.map(i), p.map((float)i));
		//assert( isClose(l.map(i), p.map((float)i)));
	}
	getchar();
#endif

	//printf("getchar t7\n");
	//getchar();
}
void TestLFO()
{
	t0();
	t1();
	t2();
 //   t3();
 //	t4();
	t5();
	t6();
	t7();


}