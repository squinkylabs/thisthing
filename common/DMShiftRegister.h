
#ifndef INCLUDE_DMDIGITALSHIFTREGISTER
#define INCLUDE_DMDIGITALSHIFTREGISTER

#include "DModule.h"
#include "GateTrigger.h"
#include "LinearInterp.h"

//z = clock division
//x = clock
//y = CV in

//a = CV delayed by 1 unit of clock division
//b = CV delayed by 2 units of clock division

class DMDigitalShiftRegister : public DModule
{
public:
	DMDigitalShiftRegister() :
		_z0(0),
		_z1(0),
		_trig(false),
		_potInterp(1, 8),			// div 1..8
		_clockDiv(0),
		_period(1)
	{
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		bool fire = false;

		if (reset)
		{
			_period = 1;
			_z0=_z1=0;
			_trig.reset();
			_clockDiv = 0;
		}

		if (z.changed)
		{
			_period = _potInterp.interp(z.value);
		}
		_trig.go(x);
		if (_trig.trigger())
		{
			_clockDiv++;
			//printf("triggered, div=%d, per=%d\n", _clockDiv, _period);
			if (_clockDiv >= _period)
			{
				fire=true;
				_clockDiv = 0;
			}
		}

		if (fire)
		{
			_z1 = _z0;
			_z0 = y;
		}
		a = _z0;
		b = _z1;

		Led_setTempSelectorOverride(_period, 1);
	}

	// this is just for test
	int getDivider() const { return _period; }
private:
	int _z0;
	int _z1;		// for only two stages we can have a really dumb SR
	GateTrigger _trig;
	LinearInterp _potInterp;
	int _clockDiv;
	int _period;
};
#endif

