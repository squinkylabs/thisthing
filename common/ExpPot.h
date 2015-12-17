#ifndef INCLUDE_EXPPOT
#define INCLUDE_EXPPOT

#include <cmath>
//#include <memory>
#include "LookupTable.h"

/** maps 0..3ff to various exponential functions
 */
class ExpPot
{
public:
	ExpPot(float min, float max) : _a(min)
	{
		const float temp = max / min;
		_b = std::log(temp ) / 0x3ff;
	}
	float map(float x)
	{
		const float temp = x * _b;
		return _a * std::exp(temp);
	}
private:
	const float _a;
	float _b;
};



// where map(0) = min
// map (0x3ff) = max
class ExpPotLookup : public LookupTable
{
public:
	ExpPotLookup(float min, float max)
	{
		ExpPot e(min, max);
		for (int i=0; i<= numEntries; ++ i)
		{
			data[i * 2] = e.map( (float)i * (0x400 / 16));
		}
		fillSlopes();
	}
	float map(int x)
	{
		return lookup(x);
	}
private:
};

#include "LowPassFilter_dbl.h"

/* maps from pot values wiht exp taper directly to filter coefficients
*/
class ExpPotFilterLookup
{
public:
	static const int precission = LowPassFilter_f::cp;			// our native type will be the low pass filter's internal data type
	typedef FixedPoint<precission> fp;
	ExpPotFilterLookup( float us_min, float us_max)
	{
		//printf("ctor of lookup %f, %f\n", us_min, us_max);
		ExpPot refPot(us_min, us_max);
		for (int i=0; i<=_kLook.numEntries; ++i)
		{
			double us =  refPot.map( (float)i * (0x400 / 16));
			auto k = LowPassFilter_f::designLPF_TC_US_s((int)us, 70000.0);	// is there a global for this???
			//printf("got the coeffs: %ff\n",k);
			_kLook.data[i*2] = k;
			//printf("filling entry %d, us=%f, k = %f\n", i, us, k.toFloat());
		}
		_kLook.fillSlopes();
	}
	fp lookup(int pot)		// returns <k:l>
	{
		//printf("lookup (%x) returning %f\n", pot,  _kLook.lookup(pot).toFloat()); 
		return  _kLook.lookup(pot);
	}


private:
	LookupTable_f<precission> _kLook;
};
#endif
