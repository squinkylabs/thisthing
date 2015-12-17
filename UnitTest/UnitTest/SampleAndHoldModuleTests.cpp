
#include "stdafx.h"
#include "DModule.h"
#include "DMSampleAndHold.h"
#include <set>

class SHMRunner
{
public:
	enum Expected {
		x_to_a_sh_ytrig,
		x_to_b_th_ytrig,
		x_to_a_sh_ztrig,
		y_to_b_sh_ztrig,
		y_to_a_sh_ztrig,
		y_to_b_sh_xtrig,
		y_to_b_th_ztrig
	
	};

	SHMRunner(DModule& mut, const std::set<Expected>& ex) :
		_mut(mut), _expected(ex)
	{
		bool sh, th;


		if (expect(x_to_a_sh_ytrig))
		{
			test_func(inx, outa, iny, sh, th);
			printf("first run, sh=%d, th=%d\n", sh, th);
			assert(sh);
		}

		if (expect(x_to_b_th_ytrig))
		{
			test_func(inx, outb, iny, sh, th);
			printf("second run, sh=%d, th=%d\n", sh, th);
			assert(th);
		}
		

	//	test_func(inx, outb, iny, sh, th);
		if ( expect(x_to_a_sh_ztrig))
		{
			test_func(inx, outa, inz, sh, th);
			printf("third run   sh=%d th=%d\n",sh, th);
			assert(sh);
		}

		if ( expect(y_to_b_sh_ztrig))
		{
			test_func(iny, outb, inz, sh, th);
			printf("fourth run   sh=%d th=%d\n",sh, th);
			assert(sh);
		}

		if (expect(y_to_a_sh_ztrig))
		{
			test_func(iny, outa, inz, sh, th);
			printf("fifth run   sh=%d th=%d\n",sh, th);
			assert(sh);
		}
		if (expect(y_to_b_sh_xtrig))
		{
			test_func(iny, outb, inx, sh, th);
			printf("sixth run   sh=%d th=%d\n",sh, th);
			assert(sh);
		}

		
		if (expect(y_to_b_th_ztrig))
		{
			test_func(iny, outb, inz, sh, th);
			printf("sixth run   sh=%d th=%d\n",sh, th);
			assert(th);
		}
	}


private:
	DModule& _mut;

	enum Inputs { inx, iny, inz};
	enum Outputs { outa, outb };

	const std::set<Expected>& _expected;

	bool expect(Expected e)
	{
		printf("ee %p\n", &_expected);
		printf("Expected (%d), with %d in set\n", (int) e, _expected.size());
		std::set<Expected>::iterator it = _expected.find(e);
		return it != _expected.end();
	}

	void test_func( Inputs input, Outputs output, Inputs gate, bool& isSH, bool& isTH)
	{
		printf("enter test fun %d %d\n", isSH, isTH);
		isSH = false;
		isTH = false;

		assert(input != gate);
		int a0, b0;
		int a, b;

		int x=0, y=0;
		ZState z(0, true);
		const int v1 = DACVoltage::xcodeForMV(1000);
		const int v2 = DACVoltage::xcodeForMV(1000 * 2);
		const int v10 = DACVoltage::xcodeForMV(1000 * 10);
		const int z10 = DACVoltage::zcodeForMV(1000 * 10);
		
		// change the input under test. keep others at zero
		_mut.go(false, x, y, z, a0, b0);
		switch(input)
		{
		case inx:
			x = v1;
			break;
		case iny:
			y = v1;
			break;
		default:
			assert(false);	// z can't be an input
		}
		_mut.go(false, x, y, z, a, b);
		assert(a==a0 && b==b0);		// since we only changed an input, we could not have a gate.

		// bring the gate up
		switch(gate)
		{
		case inx:
			x = v10;
			break;
		case iny:
			y = v10;
			break;
		case inz:
			z = ZState(z10, true);
			break;
		}
		_mut.go(false, x, y, z, a, b);

		bool aclocked=false;
		bool bclocked = false;
		switch(output)
		{
		case outa:
			if (a == v1)
				aclocked=true;
			break;
		case outb:
			if (b == v1)
				bclocked = true;
			break;
		default:
			assert(false);
		}


		// change the input under test with gate still high
		_mut.go(false, x, y, z, a0, b0);
		switch(input)
		{
		case inx:
			x = v2;
			break;
		case iny:
			y = v2;
			break;
		default:
			assert(false);	// z can't be an input
		}
		_mut.go(false, x, y, z, a, b);

		switch(output)
		{
		case outa:
			if (aclocked)
			{
				if (a == v2)
				{
					isTH = true;
				}
				else if (a == v1)
				{
					isSH = true;
				}
				else assert(false);
			}
			else assert( a == a0);
			break;
		case outb:
			if (bclocked)
			{
				if (b == v2)
				{
					isTH = true;
				}
				else if (b == v1)
				{
					isSH = true;
				}
				else assert(false);
			}
			else assert( b == b0);
			break;
		default:
			assert(false);
		}

		printf("leaving with isTH=%d iTH=%d\n", isSH, isTH);
		assert(!isSH || !isTH);  // can't both be true
	}

	// TODO: search for spurious gates
	bool isGate(Inputs input)
	{
		assert(false);
		return false;
	}
	
};


void shmt1()
{
	printf("** test DMSampleAndHold\n");
	DMSampleAndHold m;
	std::set<SHMRunner::Expected> ex;
	ex.insert( SHMRunner::x_to_a_sh_ytrig );
	ex.insert( SHMRunner::x_to_b_th_ytrig );
	SHMRunner _(m, ex) ;
}

void shmt2()
{
	// clock is Z.
// X-> A ch1 SH
// Y -> B ch2 S
	printf("** test DMSampleAndHoldDual\n");
	DMSampleAndHoldDual m;
	std::set<SHMRunner::Expected> ex;
	ex.insert( SHMRunner::x_to_a_sh_ztrig );
	ex.insert( SHMRunner::y_to_b_sh_ztrig );
	SHMRunner _(m, ex) ;
}



void shmt3()
{
// clock 1 -> Z
// clock 2 -> x
// Y -> A (clock 1 sh)
// Y -> B (clock 2 sh)
	printf("** test DMSampleAndHoldDualClock\n");
	DMSampleAndHoldDualClock m;
	std::set<SHMRunner::Expected> ex;
	ex.insert( SHMRunner::y_to_a_sh_ztrig );
	ex.insert( SHMRunner::y_to_b_sh_xtrig );
	SHMRunner _(m, ex) ;
}

void shmt4()
{
	// clock is Z
// X -> A (ch1 S&H)
// Y -> B (ch2 T&H)
	printf("** test DMSampleAndHoldTrackAndHoldZ\n");
	DMSampleAndHoldTrackAndHoldZ m;
	std::set<SHMRunner::Expected> ex;
	ex.insert( SHMRunner::x_to_a_sh_ztrig );
	ex.insert( SHMRunner::y_to_b_th_ztrig );
	SHMRunner _(m, ex) ;
}

//	DMTrackAndHoldDualClock
	//DMSampleAnHoldProbaabalistic
	//DMTrackAnHoldProbaabalistic

// OH crap - I alredy have tests for this!!! this is redundant
void SampleAndHoldModuleTests()
{
	assert(false);	// these are dead
	shmt1();
	shmt2();
	shmt3();
	shmt4();
}