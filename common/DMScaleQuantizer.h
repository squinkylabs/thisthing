
#ifndef DMSCALEQUANTIZER
#define  DMSCALEQUANTIZER

#include "OctaveScaleManager.h"
#include "DModule.h"
#include "LinearInterp.h"
#include "GateTrigger.h"

/* X = CV
 * y = trigger
 * z = scale selector
 */
class DMScaleQuantizer1 : public DModule
{
public:
	DMScaleQuantizer1() : 
	  _interp(0, OctaveScaleManager::getNumOctaveScales()),
	 _trigger(false)
	{
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (z.changed)
		{
			int index = _interp.interp(z.value);
			Led_setTempSelectorOverride(1 + index, 1);
			_scales.select(index);
		}
		_trigger.go(y);
		if (_trigger.trigger())
		{
			printf("need to do some quantizing\n");
			assert(false);
		}
		a = b = 0;
	}

private:
	OctaveScaleManager _scales;
	LinearInterp _interp;
	GateTrigger _trigger;
};
#endif