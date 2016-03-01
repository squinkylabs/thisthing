
#ifndef INCLUDE_LOWPASSFILER_DBL
#define INCLUDE_LOWPASSFILER_DBL


#include <cmath>
//#include <memory>
#include "Constants.h"

/* Simple one pole filter, implemented using double precission 
 * floating point math.
 *
 * Note: this may be too slow to run in the pic


  ---> K ----> + ----> Z-1 ------
               ^             |
               |             |
			   |--< L <-------

 */
class LowPassFilter_dbl
{
public:
	LowPassFilter_dbl() : _z(0)
	{
		set(1, 4);
	}
	void set( double fc, double fs)
	{
		//printf("fc = %f\n", fc);
		assert(fc > 0);
		designLPF(fc, fs);
	}
	// time constatnt in microseconds, sample rate in hz
	void setTC_US( int tcUS, int fs)
	{
#if defined(_MSC_VER) && (VERBOSE > 1)
		if (tcUS <= 0) printf("in setTC, tcus=%d, fs=%d\n", tcUS, fs);
#endif
		assert(tcUS > 0);
		double fc = (1000.0 * 1000.0) / (2.0 *  Constants::PI * tcUS);	// microsec to normalized freq
#if defined(_MSC_VER) && (VERBOSE > 1)
		printf("LPFilter, setTC %d us -> %f hz\n", tcUS, fc);
#endif
		set(fc, fs);
	}
	void go(int x)
	{
		return go_dbl( double(x));
	}
	int get() 
	{
		return (int) _z;
	}
private:
#if 1
	double _k;
	double _l;
#else
	float _k;
	float _l;
#endif
	double _z;

	void designLPF(double fc, double fs)
	{
		_k = 1.0 - ( std::exp( - 2.0 * Constants::PI * fc / fs));
		_l = 1.0 -_k;
#if defined(_MSC_VER) && (VERBOSE > 1)
		printf("design lpf fc=%f, fs=%f _k = %f _l = %f\n", fc, fs, _k, _l);
#endif
	}
	void go_dbl(double x)
	{
		_z = _z * _l + _k * x;
	}


};



#endif
