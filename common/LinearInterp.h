#ifndef INCLUDE_LINTERINTERP
#define  INCLUDE_LINTERINTERP

#include <cmath>
/** a linear interp where the domain is pot values: 0.. 0x3ff

* however: we want all step even sized. so, for example
* if range is 1..4
* 0..3ff/4 -> 1/2 cutover
* 0..3ff/2 -> 2/3 cutover
* 0..3*3ff/4 -> 3/4 cutover
*/

// straight interp
#if 0
class LinearInterp
{
public:
	LinearInterp(int y0, int y1)
	{
		// we can get the desired even spacing by mapping x1+1 to y1+1
		//_a= (double)((y1+1) - y0) / (double)(0x3ff + 1);

		//const int delta_y = (y1 > y0) ? 1 : -1;
		//const int delta_y = 1;
		//_a= (double)((y1+delta_y) - y0) / (double)(0x3ff + 1);


		// simple way
		_a= (double)(y1 - y0) / (double)(0x3ff );
		_b=  y0;
		//printf("\ninit %d,%d a=%f b=%f\n", y0, y1, _a, _b);
	}

	int interp(int x) const {
		double d = _a * x + _b;
		int ret = (int) d;
		//printf("li (%d) ret %f (%d)\n", x, d, ret);
		return ret;
	}
	
private:
	double _a;
	double _b;
};
#endif

// attemp at even spacing
#if 0
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

		_a= (double)(y1 - y0) / (double)(x1 - x0 );
		_b=  y0 - _a * x0;
		printf("\ninit %d,%d a=%f b=%f\n", _y0, _y1, _a, _b);
		printf("   modified x: %d,%d  y:%d,%d\n", x0, x1, y0, y1);
	}

	int interp(int x) const {
		double d = _a * x + _b;
		int ret = (int)std::floor(d);
		//int ret = (int) d;
		printf("li (%d) ret %f (%d)\n", x, d, ret);
		return ret;
	}
	
private:
	double _a;
	double _b;
};
#endif

#include "FixedPoint.h"
// even spacing fixed point
// Meant for discrete steps (not continous re-map), althouth it will work fine for continuous
// Uses fixed point, so it's pretty fast
// Domain is z pot range 0..3ff
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
private:
	const static int precission =16;
	typedef FixedPoint<precission> fp;
	fp _a;
	fp _b;
};

class InterpForLED : public LinearInterp
{
public:
	InterpForLED() : LinearInterp(1, 15) {}
};


#endif



