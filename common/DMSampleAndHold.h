
#ifndef INCLUDE_DMSAMPELANDHOLD
#define INCLUDE_DMSAMPELANDHOLD

#include "GateTrigger.h"
#include "DModule.h"
#include "SampleAndHold.h"
#include "LinearInterp.h"


/**
	Sample and hold module

	input x = CV
	input z = gate

	output a = s&h
	output b = t&h
*/
class DMSampleAndHold : public DModule
{
public:
	DMSampleAndHold() : _trig(true)
	{
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_trig.reset();
			_sh.reset();
		}
		_trig.go(z.value);	// ignore "changed" on continuous z
		_sh.go(_trig.gate(), _trig.trigger(), x);

		a = _sh.sampleAndHold();
		b = _sh.trackAndHold();
	}
private:
	SampleAndHold _sh;
	GateTrigger _trig;
};

//input x = CV ch1
//input y = CV ch2
//input z = clock

//output a = s&h ch1
//output b = s&h ch2
class DMSampleAndHoldDual : public DModule
{
public:
	DMSampleAndHoldDual() : _trig(true) {}		// z input is clock

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_trig.reset();
			_sh1.reset();
			_sh2.reset();
		}
		_trig.go(z.value);  // ignore "changed" on continuous z
		_sh1.go(_trig.gate(), _trig.trigger(), x);
		a = _sh1.sampleAndHold();
		_sh2.go(_trig.gate(), _trig.trigger(), y);
		b = _sh2.sampleAndHold();
	}
private:
	SampleAndHold _sh1;
	SampleAndHold _sh2;
	GateTrigger _trig;
};

//input x= clock ch2
//input y = CV (both channels)
//input z = clock ch1
//
//output a = s&h ch1
//output b = s&h ch2 

class DMSampleAndHoldDualClock : public DModule
{
public:
	DMSampleAndHoldDualClock() : _trig1(true), _trig2(false) {}		// z input is clock

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_trig1.reset();
			_trig2.reset();
			_sh1.reset();
			_sh2.reset();
		}
		_trig1.go(z.value);  // ignore "changed" on continuous z
		_trig2.go(x);

		_sh1.go(_trig1.gate(), _trig1.trigger(), y);
		_sh2.go(_trig2.gate(), _trig2.trigger(), y);

		a = _sh1.sampleAndHold();
		b = _sh2.sampleAndHold();
	}
private:
	SampleAndHold _sh1;
	SampleAndHold _sh2;
	GateTrigger _trig1;
	GateTrigger _trig2;
};

//input x = CV ch1
//input y = CV ch2
//input z = clock
//
//output a = s&h ch1
//output b = t&h ch2

class DMSampleAndHoldTrackAndHoldZ : public DModule
{
public:
	DMSampleAndHoldTrackAndHoldZ() : _trig(true) {}		// clock Z

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_trig.reset();
			_sh1.reset();
			_sh2.reset();
		}

		_trig.go(z.value);  // ignore "changed" on continuous z
		_sh1.go(_trig.gate(), _trig.trigger(), x);
		a = _sh1.sampleAndHold();
		_sh2.go(_trig.gate(), _trig.trigger(), y);
		b = _sh2.trackAndHold();
	}
private:
	SampleAndHold _sh1;
	SampleAndHold _sh2;
	GateTrigger _trig;
};

// x = clock ch2
// y = CV
// z = clock ch1
//
// a = t&h ch1
// b = t&h ch2

class DMTrackAndHoldDualClock : public DModule
{
public:
	DMTrackAndHoldDualClock() : _trig1(true), _trig2(false) {}		// z input is clock

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_trig1.reset();
			_trig2.reset();
			_sh1.reset();
			_sh2.reset();
		}
		_trig1.go(z.value); // ignore "changed" on continuous z
		_trig2.go(x);

		_sh1.go(_trig1.gate(), _trig1.trigger(), y);
		_sh2.go(_trig2.gate(), _trig2.trigger(), y);

		a = _sh1.trackAndHold();
		b = _sh2.trackAndHold();
	}
private:
	SampleAndHold _sh1;
	SampleAndHold _sh2;
	GateTrigger _trig1;
	GateTrigger _trig2;
};

//x = clock
//y = CV
//z = probability (linear taper, 10% to 100%)
//
//a = s&h
//b = 1 s&h (inverted output)
class DMSampleAndHoldProbabalistic  : public DModule
{
public:
    DMSampleAndHoldProbabalistic() :
		_trig(false),
		_inversionPoint(DACVoltage::xcodeForuV(Constants::INVERSION_VOLTAGE * 1000 * 1000)),
		_potScale(10, 100)
	{
		doReset();
	}

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		//printf("dmsh.go(%d, %d, %d)\n", reset, x, y);
		if (reset)
		{
			doReset();
		}
		if (z.changed)
		{
			// need to scale z to 10..100
			const int pct =  _potScale.interp(z.value);
			//printf("zchg (%d) interp to %d\n", z.value, pct);
			_trig.setPercent(pct);

			Led_setTempSelectorOverride(_led.interp(z.value), 1);
		}
		_trig.go(x);		// clock x, pot z
		//printf("in prob, g=%d t = %d\n", _trig.gate(), _trig.trigger());
		_sh.go(_trig.gate(), _trig.trigger(), y);
		a = _sh.sampleAndHold();

		// keep outputing zero until real trigger
		b = _trig.isReset() ? 0 : _inversionPoint - a;

		//printf("leaving module.go(reset=%d)\n", reset);
	}
private:
	SampleAndHold _sh;
	RandomGateTrigger _trig;
	LinearInterp _potScale;

	InterpForLED _led;

	const int _inversionPoint;

	void doReset()
	{
		_trig.setPercent(100);
		_trig.reset();
		_sh.reset();
	}
};

//x = clock
//y = CV
//z = probability (linear taper, 10% to 100%)
//
//a = t&h
//b = 1 t&h (inverted output)
class DMTrackAndHoldProbabalistic  : public DModule
{
public:
	DMTrackAndHoldProbabalistic() :
		_trig(false),
		_inversionPoint(DACVoltage::xcodeForuV(Constants::INVERSION_VOLTAGE * 1000 * 1000)),
		_potScale(10, 100)
	{
			doReset();
	}

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			doReset();
		}
		if (z.changed)
		{
			// need to scale z to 10..100
			const int pct =  _potScale.interp(z.value);
			_trig.setPercent(pct);
			Led_setTempSelectorOverride(_led.interp(z.value), 1);
		}

		_trig.go(x);		// clock x, pot z
		if (_trig.gate())
		{
			_gateHasGoneHigh = true;
		}
	
		_sha.go(_trig.gate(), _trig.trigger(), y);
		a = _sha.trackAndHold();
	
		// B gets the inverted gate from A, except in reset neither gets gate
		const bool bGate = _gateHasGoneHigh ? !_trig.gate() : false;

		_shb.go(bGate, _trig.trigger(), y);
		b = _shb.trackAndHold();
	}
private:
	bool _gateHasGoneHigh;
	SampleAndHold _sha;
	SampleAndHold _shb;
	RandomGateTrigger _trig;
	LinearInterp _potScale;
	InterpForLED _led;

	const int _inversionPoint;

	void doReset()
	{
		_trig.setPercent(100);
		_trig.reset();
		_sha.reset();
		_shb.reset();
		_gateHasGoneHigh = false;
	}
};

#endif