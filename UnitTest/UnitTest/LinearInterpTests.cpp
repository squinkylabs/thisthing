#include "stdafx.h"

#include "LinearInterp.h"
#include "LinearInterp.h"
#include "DiscreteLookup.h"


void testEnds(int y0, int y1)
{
	LinearInterp li(y0, y1);
	if ( li.interp(0) != y0)
	{
		printf("failing interp enpoints at zero. expected %d got %d\n", y0, li.interp(0));
	}
	if ( li.interp(0x3ff) != y1)
	{
		printf("failing interp enpoints at 3ff. expected %d got %d\n", y1, li.interp(0x3ff));
	}
	assert( li.interp(0) == y0);
	assert( li.interp(0x3ff) == y1);
}


// mixed pos, neg 
void li6()
{
	testEnds(1, 40);
	testEnds(0, 400);

	testEnds(40, 1);
	testEnds(400, 0);

	testEnds(-1, -40);
	testEnds(0, -400);

	testEnds(-40, -1);
	testEnds(-400, 0);

    testEnds(-1, 40);
	testEnds(-2, 40);
	testEnds(3, -100);

	testEnds(0, 1);
	testEnds(1, 0);
	testEnds(0, -1);
	testEnds(-1, 0);

}

// ends for 0..n
void li1()
{
	for (int i=1; i<1000; ++i)
	{
		testEnds(0, i);
	}
}


// ends for n...n+10
void li2()
{
	for (int i=1; i<1000; ++i)
	{
		testEnds(i , i +10);
	}
}


// ends for n...n-10
// (backwards range
void li5()
{
	for (int i=11; i<1000; ++i)
	{
		testEnds(i , i -10);
	}
}

// middle
void li3()
{
	LinearInterp li(1,3);
	assert(li.interp(0x200) == 2);
}

// rounding
#if 1
void li4()
{
	LinearInterp li(1,4);
	
	int break1 = 0x3ff / 4;
	int break2=  0x3ff / 2;
	int break3=  (3 * 0x3ff) / 4;

	assert(li.interp(break1-1) == 1);
	assert(li.interp(break1+1) == 2);

	assert(li.interp(break2-1) == 2);
	assert(li.interp(break2+1) == 3);

	assert(li.interp(break3-1) == 3);
	assert(li.interp(break3+1) == 4);
}

void li4b()
{
	printf("li4b\n");
	LinearInterp li(4, 1);
	
	int break1 = 0x3ff / 4;
	int break2=  0x3ff / 2;
	int break3=  (3 * 0x3ff) / 4;

	assert(li.interp(break1-1) == 4);
	assert(li.interp(break1+1) == 3);

	assert(li.interp(break2-1) == 3);
	assert(li.interp(break2+1) == 2);

	assert(li.interp(break3-1) == 2);
	assert(li.interp(break3+1) == 1);
}
#endif


/************* discrete lookup ****************/

void dl1()
{
	DiscreteLookup<3> dl;
	dl.setValue(0, 55);
	dl.setValue(1, -53);
	dl.setValue(2, 223344);

	int x = dl.process(0);
	assert(x == 55);
	 x = dl.process(0x200);
	assert(x == -53);
	 x = dl.process(0x3ff);
	assert(x == 223344);

}


// test that the _inverse function works
void inv0()
{
	LinearInterp li(0, 11);
	for (int i=0; i < 0x3ff; ++i)
	{
		int x = li.interp(i);
		//printf("interp(%d) gave %d\n", i, x);

		int y = li._inverse(x);
		int x0=  li.interp(y);
		//printf("interp(%d) gave %d inv=%d and back %d\n", i, x, y, x0);
		assert(x == x0);
	}
}

void LinearInterpTests()
{
	li6();
	li1();
	li2();
	li3();
	
	li4();
	li4b();
	li5();

	dl1();

	inv0();
}
