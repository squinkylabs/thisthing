
#ifndef INCLUDE_GATETRIGGER
#define INCLUDE_GATETRIGGER

#include "SchmidtTrigger.h"
#include "DACVoltage.h"
#include "Random.h"

//#define _LOG

class GateTrigger
{
public:
	// scaleForZ if false, we use normal 24-bit X/Y scaling, if true, use 10 bit Z
	// also, we bump the low thresh up to 2 v becuase Bruce's disting has a 1v offset on the Z
	// update: actually the z doens't have an offset if you put the pot in the middle.
	// let's use 2V for everyone, though
	GateTrigger(bool scaleForZ) : _sc( DACVoltage::codeForMV(scaleForZ, 2000),
						 DACVoltage::codeForMV(scaleForZ, 3000)),
					_gate(false),
					_trigger(false),
					_reset(true)
	{
	}
	void go(int v)
	{
		const bool newGate = _sc.go(v);
		if (_reset)
		{
			if (newGate)		// in reset state need to wait for low
				return;
			else
				_reset = false;
		}
		_trigger = newGate && !_gate;
		_gate = newGate;
	}
	void reset()
	{
		_gate = false;
		_trigger = false;
		_reset = true;
	}

	bool gate() const 
	{
		return _gate;
	}
	bool trigger() const
	{
		return _trigger;
	}
	int thhi() const { return _sc.thhi(); }
	int thlo() const { return _sc.thlo(); }
private:
	SchmidtTrigger _sc;
	bool _gate;
	bool _trigger;
	bool _reset;		// just reset - gate must go low before high to trigger
};

class ConditionalGateTrigger
{
public:
	ConditionalGateTrigger(bool scaleForZ) :  _sc( DACVoltage::codeForMV(scaleForZ,  500),
					 DACVoltage::codeForMV(scaleForZ, 3000)),
					_gateOut(false),
					_triggerOut(false),
					_unconditionalGate(false),
					_reset(true)
	{
		reset();
	}
	virtual bool allowGate()=0;		// pot is a 10 bit value that can be used to "decide"

	// same as GetTrigger.go();
	void go(int v)
	{
#if VERBOSE > 1
		printf("random gate trigger, _reset=%d\n", _reset);
#endif
		_triggerOut = false;		// assume we won't generate a new trigger

		// check for low to high on enderlying gate
		const bool newGate = _sc.go(v);
		if (_reset)
		{
			if (newGate)		// if gate still high, keep waiting
			{
				#if VERBOSE > 1
				printf("reset supress, _gate=%d trig=%d\n", _gateOut, _triggerOut);
#endif
				return;
			}
			_reset = false;	// first time we see a low gate we are good.
			#if VERBOSE > 1
			printf("trigger resetting _reset from low gate\n");
#endif
		}
		const bool newTrigger = newGate && !_unconditionalGate;

		_unconditionalGate = newGate;
		
		#if VERBOSE > 1
		printf("ng = %d _uncond gate = %d nt = %d\n", newGate, _unconditionalGate, newTrigger);
#endif
		if (!newGate)
		{
			_gateOut = false;		// any time underlying gate goes down, cond one does, too
		}
 
		if (newTrigger)
		{
			const bool allow = allowGate();
			#if VERBOSE > 1
			printf("got nt, allow = %d\n", allow);
#endif
			if (allow)
			{
				_gateOut = true;
				_triggerOut = true;
			}
		}
		#if VERBOSE > 1
		printf("leaving gatetrig with g=%d, t=%d\n", _gateOut, _triggerOut);
#endif
	}
	bool gate() const 
	{
		return _gateOut;
	}
	bool trigger() const
	{
		return _triggerOut;
	}

	void reset()
	{
		_reset = true;
		_gateOut = _triggerOut = false;
	}

	bool isReset() const
	{
		return _reset;
	}

private:
	SchmidtTrigger _sc;
	bool _unconditionalGate;
	bool _gateOut;
	bool _triggerOut;
	bool _reset;
};

class RandomGateTrigger : public ConditionalGateTrigger
{
public:
	RandomGateTrigger(bool scaleForZ) : ConditionalGateTrigger(scaleForZ), _percent(50)
	{
		assert(!scaleForZ);		// only becuase so far we aren't using/testing it
	}
	virtual bool allowGate()
	{

		int r = _r.get();
		
		// map random to 0..100)
		r &= 0xffff;
		//printf("masked = %d\n", r);
		r *= 100;
	//	printf("masked100 = %d\n", r);
		r >>= 16;

		assert(r >= 0 && r <= 100);
		bool ret = (_percent >= 100) ? true : (r <= _percent);
		//printf("allow gate, percent = %d ret = %d\n", _percent, ret);
		return ret;
	}

	void setPercent(int percent)
	{
		assert(percent>=0 && percent <= 100);
		_percent = percent;
	}


private:
	int _percent;
	Random _r;
};



#if 0	// first try
// If condition is true, acts like normal gate trigger.
// If condition is false, gate and trigger are ignored
class ConditionalGateTrigger
{
public:
	ConditionalGateTrigger(bool scaleForZ) :  _sc( DACVoltage::codeForMV(scaleForZ,  500),
					 DACVoltage::codeForMV(scaleForZ, 3000)),
					_gateOut(false),
					_triggerOut(false),
					_unconditionalGate(false)
	{
	}
	virtual bool allowGate(int pot)=0;		// pot is a 10 bit value that can be used to "decide"
	void go(int v, int pot)
	{
		_triggerOut = false;		// assume we won't generate a new trigger

		// check for low to high on enderlying gate
		const bool newGate = _sc.go(v);
		const bool newTrigger = newGate && !_unconditionalGate;

		_unconditionalGate = newGate;
		
		if (!newGate)
		{
			_gateOut = false;		// any time underlying gate goes down, cond one does, too
		}

		 
		if (newTrigger)
		{
			const bool allow = allowGate(pot);
			if (allow)
			{
				_gateOut = true;
				_triggerOut = true;
			}
		}
	}
	bool gate() const 
	{
		return _gateOut;
	}
	bool trigger() const
	{
		return _triggerOut;
	}
private:
	SchmidtTrigger _sc;
	bool _unconditionalGate;
	bool _gateOut;
	bool _triggerOut;
};

extern int i_global;
//#define _LOG
class RandomGateTrigger : public ConditionalGateTrigger
{
public:
	RandomGateTrigger(int min, int max) : ConditionalGateTrigger(false)
	{
		assert(min < max);
		assert(min >=0 );
		assert(max <= 100);

		
		_b = min;
		_a = (max - min) << 6;

#ifdef _LOG
		printf("RandomGateTrigger in ctor min=%d max=%d _a=%d _b = %d\n", min, max, _a, _b);
#endif
	}

	virtual bool allowGate(int pot)
	{
		// map "pot" linearly to <min, max>

		int r = _r.get();
		
		// map random to 0..100)
		r &= 0xffff;
		//printf("masked = %d\n", r);
		r *= 100;
	//	printf("masked100 = %d\n", r);
		r >>= 16;

	


		assert(r >= 0 && r <= 100);

		int pot2 = ((_a * pot) >> 16) + _b;

		i_global = pot2;

	
		bool ret = (pot2 >= 100) ? true : (r <= pot2);
#ifdef _LOG
		printf("RandomGateTrigger r(0.100)=%d p2 (scaled pot)= %d will ret %d\n", r, pot2, ret);
#endif
		return ret;
	}

private:
	Random _r;
	int _a;
	int _b;

	
};
#endif


#endif