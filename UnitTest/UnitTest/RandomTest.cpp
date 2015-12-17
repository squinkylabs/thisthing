
#include "stdafx.h"
#include "Random.h"
#include "Random.h"
#include <set>

void r0()
{
	Random r;
	int x = r.get();
	int z = r.get();
	int y = r.get();
	assert(x != y);
	assert(y != z);
}

// test that generator is not trivially bad. no repeats after 10000
void r1()
{
	{
	std::set<int32_t> set;
	Random r;
	const int num=100000;
	for (int i=0; i<num; ++i)
	{
		int32_t x = r.get();
		assert(set.find(x) == set.end());
		if (set.find(x) != set.end()) printf("FAIL\n");
		set.insert(x);
	}
	printf("leave r1\n");
	}
	printf("really leave\n");
	//getchar();
}

void r2()
{
	Random r;
	const int first = r.get();
	;
	for (int64_t count=0; true; ++count)
	{
		int x = r.get();
		if (x <=0)
		{
			printf("negative\n");
			return;
		}
		if (x == first)
		{

			printf("count was %lld %llx\n", count, count);
			
			return;
		}
	}
}

void r3()
{
	size_t size = 0x80000000;
	char * mem = new char[size];
	for (size_t i=0; i<size; ++i)
	{
		mem[i] = 0;
	}
	printf("size = %llx, mem=%p\n", size, mem);

	Random r;
	bool done = false;
	for (int64_t count=0; !done; ++count)
	{
		int x = r.get();
		if (count > (int64_t)size)
		{
			printf("internal error\n");
			return;
		}
		if (mem[x] != 0)
		{
			printf("non-repeat count = %llx\n", count);
			done = true;
		}
		mem[x] = 1;
	}

	delete mem;
}

void RandomTest()
{
	r0();
#ifdef _SLOWTEST
	r1();
#endif
#if 0		// long running
	r2();
	r3();
	printf("leave random\n");
	getchar();
#endif
}