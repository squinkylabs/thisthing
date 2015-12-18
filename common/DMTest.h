/* a bunch of test modules
*/


#ifndef INCLUDE_DMTEST
#define  INCLUDE_DMTEST

#include "DModule.h"
#include "LowPassFilter_dbl.h"
#include "ExpPot.h"

// a fixed lpf
class DMFilter  : public DModule
{
public:

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{ 
		_f.go(x);
		const int temp = _f.get();
		a = temp;
		b = temp;

		const float f = _scalePot_to_us.map(z.value);
		//  _d.lfo = f;
		_f.set(f + 10, 10000);          // let's try updating ever sample, see what happens.
	}
	DMFilter() :  _scalePot_to_us(10.0f , 1000.0f)
	{
		_f.set(1, 10);
	}
private:
	LowPassFilter_f _f;
	ExpPotLookup _scalePot_to_us;
};
#endif
