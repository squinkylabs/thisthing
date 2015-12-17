
#include "stdafx.h"
#include "DModule.h"
#include "DMSlopeDetector.h"
#include <set>

class SDMRunner
{
public:
	enum Expected {
		x_to_a_pos,
		x_to_a_neg,
		y_to_b_pos,
		y_to_b_neg
	};
	SDMRunner(DModule& mut, int duration, const std::set<Expected>& ex) :
		_mut(mut), _duration(duration), _expected(ex)
	{
		bool trigA, trigB;

		const int v5= DACVoltage::xcodeForMV(5000);
		// first run with no input
		run(0, 0, trigA, trigB);
		assert(!trigA);
		assert(!trigB);

		// run with X
		run(v5, 0,  trigA, trigB);
		assert(!trigB);		// we have no modules where x -> b (yet)
		assert(trigA == expect(x_to_a_pos));
			
		// keep x same, raise Y
		run(v5, v5,  trigA, trigB);
		assert(!trigA);		// we have no modules where y -> a (yet)
		assert(trigB == expect(y_to_b_pos));

		//lower  x , y same 
		run(0, v5,  trigA, trigB);
		assert(!trigB);		
		assert(trigA == expect(x_to_a_neg));

		// x same, lower y
		run(0, 0,  trigA, trigB);
		assert(!trigA);		
		assert(trigB == expect(y_to_b_neg));
	}

private:
	DModule& _mut;
	const int _duration;
	const std::set<Expected> _expected;

	bool expect(Expected e)
	{
		std::set<Expected>::const_iterator it = _expected.find(e);
		return it != _expected.end();
	}

	void run(int x, int y, bool& trigA, bool& trigB)
	{
		bool ta=false, tb=false;

		ZState z(0, true);
		int i=0;
		for (bool done=false; !done; ++i )
		{
			int a, b;
			_mut.go(false, x, y, z, a, b);
			if (a > 0) {
				ta = true;
				assert ( a == DACVoltage::xcodeForMV(8 * 1000));
			}
			if (b > 0) {
				tb = true;
				assert(b == DACVoltage::xcodeForMV(8 * 1000));
			}


			// loop terminates when we have gone long enough to detect everything,
			// and then long enough to recover
			if (i > _duration)
			{
				if( !a && !b)
					done = true;
			}
		}

		trigA = ta;
		trigB = tb;
	}
	
};


void sdmt1()
{

	DMSlopeDetectorDual m;
	std::set<SDMRunner::Expected> ex;

	ex.insert( SDMRunner::x_to_a_pos );
	ex.insert( SDMRunner::x_to_a_neg );
	ex.insert( SDMRunner::y_to_b_pos );
	ex.insert( SDMRunner::y_to_b_neg );
	SDMRunner _(m, 1000, ex) ;
}


void SlopeDetectorModuleTests()
{
	sdmt1();
	printf("sdm1 e\n");

}