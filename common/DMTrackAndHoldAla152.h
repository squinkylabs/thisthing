#ifndef INCLUDE_DNTRACKANDHAOLDALA152
#define INCLUDE_DNTRACKANDHAOLDALA152

#include "DModule.h"
#include "GateTrigger.h"
#include "DiscreteLookup.h"
// x = cv
// z = trigger
// a = t&h "odd"
// b = t&h "even"
class DMTrackAndHoldAla152 : public DModule
{
public:
	DMTrackAndHoldAla152() : _trig(true),
		_even(false),		// start "odd" becuase first trigger is ignored by reset
		_a(0), _b(0), _reset(true)
	{
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		//printf("go x = %d  even = %d reset = %d _reset=%d\n", x, _even, reset, _reset);
		if (reset)
		{
			//printf("ala2 was reset\n");
			_reset = true;
			_a = _b =0;
			_even = false;
			_trig.reset();
		}
		
		_trig.go(z.value);		// ignore "changed", this is continuous
		if (_trig.trigger())
		{
			_even = !_even;
			_reset = false;
		}
		

		if (!_reset)
		{
			if (_even)
			{
				_a = x;
			}
			else
			{
				_b = x;
			}
		}
		a = _a;
		b = _b;
		//printf("returning a = %d, b=%d\n", a, b);
	}
private:
	GateTrigger _trig;
	int _a;
	int _b;
	bool _even;
	bool _reset;
};


//input x = clock
//input y = CV
//input z - selects from 12 presets

//output a = t&h ch1 - clocking set by preset
//output b = t&h ch2 - clocking set by preset
class DMTrackAndHoldExtendedAla152 : public DModule
{
public:
	DMTrackAndHoldExtendedAla152() : _trig(false), _count(-1), _a(0), _b(0)
	{
		//	lookup values are "a track step" | "b track step" << 8
		//1   a b - -
		_lookup.setValue(0,  0 | 1<<8);

		//2   a - b -
		_lookup.setValue(1,  0 | 2<<8);
		//3   a - - b
		_lookup.setValue(2,  0 | 3<<8);
		//4   - a b -
		_lookup.setValue(3,  1 | 2<<8);
		//5   - a - b
		_lookup.setValue(4,  1 | 3<<8);
		//6   b a - -
		_lookup.setValue(5,  1 | 0<<8);
		//7   - - a b
		_lookup.setValue(6,  2 | 3<<8);
		//8   b - a -
		_lookup.setValue(7,  2 | 0<<8);
		//9   - b a -
		_lookup.setValue(8,  2 | 1<<8);
		//10 b - - a
		_lookup.setValue(9,  3 | 0<<8);
		//11 - b - a
		_lookup.setValue(10,  3 | 1<<8);
		//12 - - b a
		_lookup.setValue(11,  3 | 2<<8);

		_lookup.setStepNumber(0);
		_pattern = _lookup.getStepValue(0);

	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			 _count=-1;
			 _a= _b =0;
			 _trig.reset();
		}
		if (z.changed)
		{
			_pattern = _lookup.process(z.value);
			Led_setTempSelectorOverride(1 + _lookup.getStepNumber(), 1);
		}
		const int acap = _pattern & 0xf;
		const int bcap = _pattern >> 8;
		//printf("step = %d z=%d acap = %d, bcap = %d\n", _lookup.getStepNumber(), z, acap, bcap);

		_trig.go(x);
		if (_trig.trigger())
		{
			++_count;
			if (_count > 3)
				_count = 0;
		}

		if (_count == acap)
		{
			_a = y;
			//printf("dm captured a %d\n", y);
		}
		else if (_count == bcap)
		{
			_b = y;
			//printf("dm captured b %d\n", y);
		}

		a = _a;
		b = _b;

		
	}
private:
	int _pattern;
	GateTrigger _trig;
	int _a;
	int _b;
	int _count;

	DiscreteLookup<12> _lookup;
};

#endif
