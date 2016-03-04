/* a bunch or random utilities for unit tests
 */

#ifndef TESTUTILS
#define TESTUTILS

#include "DACVoltage.h"
#include "DModule.h"

class Test
{
public:
	static void resetModule(DModule& mod)
	{
		int a;
		int x = DACVoltage::xcodeForMV(10000);
	
		mod.go(true, x, x, ZState(0x3ff, true), a, a);
	}

	template <typename T>
	bool withinLimit(T limit, T a, T b)
	{
		return std::abs(a -b ) <= limit;
	}



};
#endif
