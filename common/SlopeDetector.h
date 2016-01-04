
#ifndef INCLUDE_SLOPEDETECTOR
#define INCLUDE_SLOPEDETECTOR

#include "LowPassFilter_dbl.h"
#include "DACVoltage.h"

class SlopeDetector
{
public:
	typedef LowPassFilter_f::fp fp;			// for fixed point nunmbers

	SlopeDetector(int thresholdMillivolts, int tcUS, int sampleRate, bool tcIsUS) :
		_threshold(DACVoltage::xcodeForMV(thresholdMillivolts)),
		_sampleRate(sampleRate),
		_pos(false), _neg(false), _both(false)
	{
#if defined(_MSC_VER) && (_VERBOSE > 1)
		printf("ctor of sd, tcUS = %d thresh=%d sr=%d\n", tcUS, thresholdMillivolts, sampleRate);
#endif
		assert(tcIsUS);
		assert(tcUS > 0);
		assert(tcUS > 100);		// probably someone is mixed up on units? not necessarily a problem
		setTC_US(tcUS);
	}
	// set time constant in micsoseconds
	void setTC_US(int us)
	{
#if defined(_MSC_VER) && (VERBOSE > 1)
		printf("SD::setTC_US tcMS = %d\n", us);
#endif
		assert(us > 0);
		_filter.setTC_US(us, _sampleRate);
	}

	void setTC_coeffDirect(fp k)
	{
		_filter.set_k(k);
	}

	void go(int input)
	{
		_filter.go(input);
		_pos =  ((input - _threshold) > _filter.get());
		_neg = ((input + _threshold) < _filter.get());
		_both = _pos || _neg;
#if defined(_MSC_VER) && 0
		printf("in sd.go(%d), pos=%d neg=%d\n", input, _pos, _neg);
#endif
			
	}
	bool getPositiveSlope() const { return _pos; }
	bool getNegativeSlope() const { return _neg; }
	bool getBothSlope() const { return _both; }
#if defined(_MSC_VER) 
	void print_go(const char * msg, int input)
	{
		go(input);
		printf("%s(%d) in=%d lpf=%d in-filter= %d, thr=%d\n",
			msg, getBothSlope(), input, _filter.get(), input - _filter.get(), _threshold);
	}
#endif
private:
	LowPassFilter_f _filter;
	const int _threshold;
	const int _sampleRate;
	bool _pos, _neg, _both;

};

#endif
