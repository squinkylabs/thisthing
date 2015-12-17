
#ifndef INCLUDE_DMTUNINGREF
#define INCLUDE_DMTUNINGREF

#include "DModule.h"
#include "DiscreteLookup.h"
#include "DACVoltage.h"

class DMTuningReference : public DModule
{
public:
	DMTuningReference() : 
		_threeV( DACVoltage::xcodeForuV(3000000)),
		_vn8(DACVoltage::xcodeForuV(-8000000)),
		_v8(DACVoltage::xcodeForuV(8000000)),
		_pt25(DACVoltage::xcodeForuV((int)(.25 * 1000000.0))),
		_pt75(DACVoltage::xcodeForuV((int)(.75 * 1000000.0))),
		_overPulse(0),
		_underPulse(0)
	{
		_lookup.setValue(0, DACVoltage::xcodeForuV((int)(.75 * 1000000.0)));
		_lookup.setValue(1, DACVoltage::xcodeForuV((int)(1.75 * 1000000.0)));
		_lookup.setValue(2, DACVoltage::xcodeForuV((int)(2.75 * 1000000.0)));
		_lookup.setValue(3, DACVoltage::xcodeForuV((int)(3.75 * 1000000.0)));
		_lookup.setValue(4, DACVoltage::xcodeForuV((int)(4.75 * 1000000.0)));

		_lookup.setStepNumber(0);
		_voltage = _lookup.getStepValue(0);
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_lookup.setStepNumber(0);
			_voltage = _lookup.getStepValue(0);
			_overPulse = 0;
			_underPulse = 0;
		}
		if (z.changed)
		{
			_voltage = _lookup.process(z.value);
			Led_setTempSelectorOverride(1 + _lookup.getStepNumber(), 1);
		}
		//printf("adder: z = %d oct = %d\n", z, oct);
		a = _voltage;
		b = _voltage + _threeV;

		if (x > _v8)
		{
			_overPulse = Constants::DISTING_SAMPLE_RATE / 4;
			_underPulse = 0;
		}
		else if (x < _vn8)
		{
			//printf("less than n8\n");
			_underPulse = Constants::DISTING_SAMPLE_RATE / 4;
			_overPulse = 0;
		}

		if (_overPulse)
		{
			b += _pt25;
			_overPulse--;
		}
		else if (_underPulse)
		{
			b -= _pt75;
			_underPulse--;
		}
		
	}
private:
	int _voltage;
	DiscreteLookup<5> _lookup;
	int _overPulse;
	int _underPulse;
	
	const int _threeV;
	const int _vn8;
	const int _v8;
	const int _pt25;
	const int _pt75;
	
};


/* voltage source in one volt increments
 */

class DMVoltage
 : public DModule
{
public:
	DMVoltage()
	{
		for (int i=0; i<9; ++i)
			_lookup.setValue(i, DACVoltage::xcodeForuV(i * 1000000));
		
	}
	virtual void go(int x, int y, int z, volatile int& a, volatile int&b)
	{
		int v = _lookup.process(z);
		//printf("adder: z = %d oct = %d\n", z, oct);
		a = v;
		b = v;

		Led_setTempSelectorOverride( _lookup.getStepNumber(), 1);
	}
private:
	DiscreteLookup<9> _lookup;
};

#endif