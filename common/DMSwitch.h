#ifndef INCLUDE_DMSWITCH
#define INCLUDE_DMSWITCH

#include "Quantizer.h"
#include "GateTrigger.h"
#include "DModule.h"

/* test module for accurate CV switch
*/
class DMSwitch : public DModule
{
public:
	DMSwitch() : _gt(true), _state(0) {}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			doReset();
		}
		_gt.go(z.value);
		if (_gt.trigger())
		{
			switch(_state)
			{
			case 0:
				_state = 1;
				break;
			case 1:
				_state = 2;
				break;
			case 2:
				_state = 1;
				break;
			default:
				assert(0);
			}
			Led_setTempSelectorOverride(_state, 1);
		}

		switch(_state)
		{
		case 0:
			a = 0;
			break;
		case 1:
			a = x;
			break;
		case 2:
			a = y;
			break;
		default:
			assert(0);
		}

		_qt.go(a);
		const int midi = _qt.getMIDI();
		b = _state ? _qt.midi2CV(midi) : 0;
	}
private:
	GateTrigger _gt;
	int			_state; // 0: nothing 1: x, 2: y
	ChromaticQuantizer _qt;
	

	void doReset()
	{
		_state = 0;
		_gt.reset();
	}
};

#endif
