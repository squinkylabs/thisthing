#ifndef INCLUDE_LINTERINTERP
#define  INCLUDE_LINTERINTERP

#include "FixedPoint.h"

/** a linear interp where the domain is pot values: 0.. 0x3ff
* however: we want all step even sized. so, for example
* if range is 1..4
* 0..3ff/4 -> 1/2 cutover
* 0..3ff/2 -> 2/3 cutover
* 0..3*3ff/4 -> 3/4 cutover
*
* Meant for discrete steps (not continous re-map), althouth it will work fine for continuous
* Uses fixed point, so it's pretty fast
* Domain is z pot range 0..3ff
*/


class LinearInterp
{
public:
	LinearInterp(int y0, int y1)
	{
		const int _y0 = y0;
		const int _y1 = y1;
		int x0 = 0;
		int x1 = 0x3ff;

		// adjust one end so we get even spacing when we round down
		if (y1 > y0)
		{
			y1++;
			x1++;
		}
		else
		{
			y0++;
			x0--;
		}

		float a= (float)((double)(y1 - y0) / (double)(x1 - x0 ));
		float  b=  y0 - a * x0;

		_a = fp::fromFloat(a);
		_b = fp::fromFloat(b);
	}
	int interp(int x) const
	{
		fp _x = fp::fromInt(x);
		fp d = _a.mul( _x).add(_b);

		int ret = d.toInt();

	//	printf("li (%d) ret %f (%d)\n", x, d.toFloat(), ret);
		return ret;
	}

	// just for testing - do an inverse lookup
	int _inverse(int val) const
	{
		// search for the right answer. slow and dumb, but it works
		for (int x=0; x <0x3fff; ++x)
		{
			int i = interp(x);
			if (i == val)
				return x;
		}
		assert(false);
		return -1;
	}
private:
	const static int precission =16;
	typedef FixedPoint<precission> fp;
	fp _a;
	fp _b;
};


/* interpolator for mapping Z knob range to 1..15 for the way we display things on the LEDs
 */
class InterpForLED : public LinearInterp
{
public:
	InterpForLED() : LinearInterp(1, 15) {}
};
#endif



