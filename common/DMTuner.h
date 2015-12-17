/* a simple tuner and voltage source
 */

#ifndef INCLUDE_DMTUNER
#define INCLUDE_DMTUNER
#include "DModule.h"
#include "LFO.h"
#include "Constants.h"


class DMTuner : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_osc.go();
		a = b = _osc.getTri2v();
	}
	DMTuner()
	{
		_osc.set(440.0, Constants::DISTING_SAMPLE_RATE);
	}
private:
	LFO _osc;
};

#endif