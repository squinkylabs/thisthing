#ifndef INCLUDE_DMCOUNTER
#define INCLUDE_DMCOUNTER

#include "LinearInterp.h"
#include "GateTrigger.h"
#include "DModule.h"
#include "DACVoltage.h"
#include "Counter.h"

// x = clock
// z = counter period
class DMCounter : public DModule
{
public:
	DMCounter(int minPeriod, int maxPeriod) :
		_trig(false),
		_8v(DACVoltage::xcodeForMV(8000)),
		_potInterp(minPeriod, maxPeriod)
	{
		_ct.setPeriod(minPeriod);
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (z.changed)
		{
			int period = _potInterp.interp(z.value);
			_ct.setPeriod(period);
		}

		_trig.go(x);		// run clock into trigger detector
		if (_trig.trigger())
		{
			_ct.clock();	// clock the counter on every input trigger
		}

		a = b =  _ct.get() ? _8v : 0;
	}
private:
	Counter _ct;
	GateTrigger _trig;
	LinearInterp _potInterp;
	const int _8v;
};

#endif