

#ifndef INCLUDE_LOWPASSFILTERF
#define INCLUDE_LOWPASSFILTERF

#include "FixedPoint.h"
#include <cmath>
#include "Constants.h"

/* simple one pole low-pass filter
 * attempt at making a fixed point filter with all mixed precission numbers
 */
class LowPassFilter_f
{
public:
	const static int cp=2;  // filter coeff k and l are always less than one...
	typedef FixedPoint<cp> fp;

	LowPassFilter_f()
	{
		set(1, 4);
	}

	void go(int x)
	{
		//	_z = _z * _l + _k * x;
		const FixedPoint<bits> _x = FixedPoint<bits>::fromInt(x);
		_z = _z.mulMixed(_l).add( _x.mulMixed(_k));
	}

	int get()  const
	{
		return _z.toInt();
	}

	/* directly set the filter coefficients. For use by clients who
	 * know how to compute these themselves (presumably with a lookup table).
	 * Very fast
	 */
	void set_k(fp k)
	{
		_k = k;
		fp one = fp::fromInt(1);
		_l = one.sub(k);			// l = 1 - k
	}


	/* The remaining functions all calculate / set filter coefficients
	 * using very slow floating point / trancendental functions.
	 */
	void set( double fc, double fs)
	{
		//printf("fc = %f\n", fc);
		assert(fc > 0);
		designLPF(fc, fs);
	}

	// returns k
	static  fp designLPF_s(double fc, double fs)
	{
		const double k = 1.0 - ( std::exp( - 2.0 * Constants::PI * fc / fs));
		return	FixedPoint<cp>::fromFloat((float)k);
	}
	// returns k
	static fp  designLPF_TC_US_s(int tcUS, double fs)
	{
#if defined(_MSC_VER) && (VERBOSE > 1)
		if (tcUS <= 0) printf("in setTC, tcus=%d, fs=%d\n", tcUS, fs);
#endif
		assert(tcUS > 0);
		double fc = (1000.0 * 1000.0) / (2.0 *  Constants::PI * tcUS);	// microsec to normalized freq
#if defined(_MSC_VER) && (VERBOSE > 1)
		printf("LPFilter, setTC %d us -> %f hz\n", tcUS, fc);
#endif
		return designLPF_s(fc, fs);
	}

	void setTC_US( int tcUS, int fs)
	{
		fp k = designLPF_TC_US_s(tcUS, fs);
		set_k(k);
	}

private:
	const static int bits=24; // this filter takes 24 bit input

	FixedPoint<cp> _k;			// k and l are always less than one....
	FixedPoint<cp> _l;			
	FixedPoint<bits> _z;		// delay memory	

	void designLPF(double fc, double fs)
	{
		set_k( designLPF_s(fc, fs));
	}

};


class TwoPoleLPF_f
{
public:
	void set( double fc, double fs)
	{
		_inputFiltera.set(fc, fs);
		_inputFilterb.set(fc, fs);
	}
	void go(int x)
	{
		_inputFiltera.go(x);
		x = _inputFiltera.get();
		_inputFilterb.go(x);
	}
	int get() const
	{
		return _inputFilterb.get();
	}
private:
	LowPassFilter_f _inputFiltera;	
	LowPassFilter_f _inputFilterb;	

};
#endif
