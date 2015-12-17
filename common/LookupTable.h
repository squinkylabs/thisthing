
#ifndef INCLUDE_LOOKUPTABLE
#define INCLUDE_LOOKUPTABLE


/** lookup table where index is 0..3ff
*/
class LookupTable
{
public:
	float lookup(int x)
	{
		int index = x >> 6;		// top four bits are index
		assert(index < 16);
		int fraction = x & 0x3f;		// bottom 6 bits are fraction

		assert(x == (index << 6) + fraction);

		//printf("lookup(%d), index = %d, fraction = %d\n", x, index, fraction);

		// now look in table
		int i = index * 2;
		float y0 = data[i];	
		float delta = data[i + 1] * fraction;
		//printf("y0 = %f, delta = %f slope = %f\n", y0, delta, data[i+1]); 
		float ret = y0 + delta;
		//printf("in lookup x = %d index = %d, fraction = %d\n   y0 = %f delta = %f ret = %f\n",
		//	x, index, fraction, y0, delta, ret);
		return ret;
	}
protected:
	static const int numEntries = 16;

	float data[2 * (numEntries + 1)]; 

	void fillSlopes()
	{
		for (int i=0; i<= numEntries; ++ i)
		{
			// map 6 bits "fraction" into this slot
			data[2*i + 1] = 0;		// TODO

			float dy = data[ 2 * (i+1)] - data[2 * i];
			float slope = dy / 0x40;
			//printf("in fill, slope is %f\n", slope);
			data[2*i + 1] = slope;

			//printf("entry[%d]=%f,%f\n", i, data[2*i], data[2*i+1]);
		}

	}
private:

};

#include "FixedPoint.h"

template <int precission> 
class LookupTable_f
{
public:
	FixedPoint<precission> lookup(int x)
	{
		int index = x >> 6;		// top four bits are index
		assert(index < 16);
		int fraction = x & 0x3f;		// bottom 6 bits are fraction
	
		// TODO: can we use less bits?
		FixedPoint<7> fraction_f = FixedPoint<7>::fromInt(fraction); 

		assert(x == (index << 6) + fraction);

		// now look in table
		int i = index * 2; 
		FixedPoint<precission> y0 = data[i];	
		FixedPoint<precission> delta = data[i + 1].mulMixed(fraction_f);
		FixedPoint<precission> ret = y0.add(delta);
		//printf("in lookup x = %d index = %d, fraction = %d\n   y0 = %f delta = %f ret = %f\n",		x, index, fraction, y0.toFloat(), delta.toFloat(), ret.toFloat());
		//printf("   slope=%f, fraction_f = %f\n", data[i + 1].toFloat(), fraction_f.toFloat());
		return ret;
	}


	static const int numEntries = 16;
	// make this public so we can construct one
	FixedPoint<precission> data[2 * (numEntries + 1)]; 

	void fillSlopes()
	{
		for (int i=0; i<= numEntries; ++ i)
		{
			// map 6 bits "fraction" into this slot
			data[2*i + 1] = FixedPoint<precission>::fromInt(0);		// TODO

			double dy = data[ 2 * (i+1)].toFloat() - data[2 * i].toFloat();
			float slope = (float) (dy / 0x40);
			//printf("in fill_f, slope[%d] is %f\n", i, slope);
#if 0 // experiment
			{
				FixedPoint<precission> dy = data[ 2 * (i+1)].sub(data[2 * i]);
				FixedPoint<precission> slope = dy.shr(6);
				//float slope = dy / 0x40;
				printf("fp slope = %f\n", slope.toFloat());
			}
#endif
			data[2*i + 1] = FixedPoint<precission>::fromFloat(slope);

			//printf("entry[%d]=%f,%f\n", i, data[2*i].toFloat(), data[2*i+1].toFloat());
		}

	}
private:

};

#endif
