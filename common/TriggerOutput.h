

#ifndef INCLUDE_TRIGGEROUTPUT
#define INCLUDE_TRIGGEROUTPUT

#include "DACVoltage.h"
#include "GateTrigger.h"

/* output processing for triggers. outputs a finite durration trigger when gate changes
 */
class TriggerOutput
{
public:
	TriggerOutput() :
		_gateProcessor(false),
		_counter(0),
		_duration( (Constants::TRIGGER_OUT_TIME_MS * Constants::DISTING_SAMPLE_RATE) / 1000)
	{
	}
	void go(bool gate)
	{
		if (_counter)
		{
			--_counter;
			return;
		}
		_gateProcessor.go(gate ? DACVoltage::xcodeForGateHi() : 0 );
		if (_gateProcessor.trigger())
		{
			_counter = _duration;
		}

	}
	int get() const
	{
		return (_counter > 0) ? DACVoltage::xcodeForGateHi() : 0;
	}
private:
	const int _duration;
	int _counter;
	GateTrigger _gateProcessor;
};
#endif
