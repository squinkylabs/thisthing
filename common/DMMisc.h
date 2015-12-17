// a bunch of silly modules. mostly for test

#ifndef INCLUDE_DMMISC
#define  INCLUDE_DMMISC

#include "DModule.h"
#include "GateTrigger.h"
#include "TriggerOutput.h"
//#include <algorithm>

class DMConst  : public DModule
{
public:
	DMConst(int a, int b) : _a(a), _b(b) {}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		a = _a;
		b = _b;
	}
private:
	const int _a;
	const int _b;
};

class DMZero : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		a = 0;
		b = 0;
	}
};


// a = x
// b = y
class DMThru : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		a = x;
		b = y;
	}
};

#if 0
class DMZGate  : public DModule
{
public:
	DMZGate() : _g(true), _v8(DACVoltage::xcodeForMV(8 * 1000))
	{
	}
	void go(int _x, int _y, int z, volatile int& a, volatile int& b)
	{
		_g.go(z);
		int g = _g.gate() ? _v8 : 0;
		a = g;
		b = g;

	}
private:
	GateTrigger _g;
	const int _v8;
};
#endif


class DMZ :  public DModule
{
public:
	DMZ() : _sawMin(false), _sawMax(false),  _v8(DACVoltage::xcodeForMV(8 * 1000)) {}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (z.value==0) _sawMin = true;
		if (z.value==0x3ff) _sawMax = true;

		a = _sawMin ? _v8 : 0;
		b = _sawMax ? _v8 : 0;
	}
private:
	bool _sawMin;
	bool _sawMax;
	const int _v8;
};

class DMGate :  public DModule
{
public:
	DMGate() : _gz(true), _gx(false) {}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_gz.go(z.value);
		const bool gatez = _gz.gate();
		b = DACVoltage::xcodeForMV( gatez ? 10000 : -10000);

		_gx.go(x);
		const bool gatex = _gx.gate();
		a = DACVoltage::xcodeForMV( gatex ? 10000 : -10000);
	}
private:
	GateTrigger _gz;
	GateTrigger _gx;
};


// b out is lfo with edge trigger out
// x->a is trigger processor
class DMTriggerDebug  :  public DModule
{
public:
	DMTriggerDebug() : _in(false), _count(0)
	{
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (++_count > 7000)
		{
			_count = 0;
			_tob.go(true);
		}
		else _tob.go(false);

		_in.go(x);
		_toa.go( _in.trigger());

		a = _toa.get();
		b = _tob.get();
	}
private:
	GateTrigger _in;
	TriggerOutput _toa;
	TriggerOutput _tob;
	int _count;
};

#if 0
class DMZDebug  :  public DModule
{
public:
	DMZDebug() : _ct(0), _min(0x3ff), _max(0) {}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		const int zvoltage = DACVoltage::zMVFromCode(z.value);
		const int acode = DACVoltage::xcodeForMV(zvoltage);
		const int bcode = DACVoltage::xcodeForMV(zvoltage / 2);

		a = acode;
		b = bcode;

		// TODO: fix me (don't include <memory>
		_min = std::min(z.value, _min);
		_max = std::max(z.value, _max);
		if (++_ct > 20000)
		{
#if 1
			int disp = _max - _min;
			disp = std::min(disp, 255);
			if (_even) disp |= 0x80;		// flip the high bit to keep display active

			_even = !_even;
#else
			int disp = z & 0xff;
#endif

			Led_setTempSelectorOverride( disp, 2);
			_ct = 0;
			_max= 0;
			_min = 0x3ff;
		}
	}
private:
	int _ct;
	int _min;
	int _max;
	bool _even;
};
#endif


#endif
