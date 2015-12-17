/* Fixed point number class, based on int 32
*/

#ifndef INCLUDE_FIXEDPOINT
#define INCLUDE_FIXEDPOINT

template <int precission>
class FixedPoint
{
public:

	static const int _fractionalBits = 32 - precission;

	// TODO: should this be 1 insead of 2?
	//static const int _scaleFactor = 2 << (_fractionalBits-1);
	static const int _scaleFactor = 1 << (_fractionalBits);
	static const int _max = (1 << (precission-1)) -1;
	static const int _min = -_max;//  + 1;


	FixedPoint() 
	{
		_val = 0;
	}
	static FixedPoint fromFloat(float f)
	{
	//	printf("in fromFloat %f, min=%d, max=%d prec=%d\n", f, _min, _max, precission);
		assert(f <= _max);
		assert(f >= _min);
		 int32_t val = (int32_t)( f * _scaleFactor);
		 return FixedPoint(val);
	}
	static FixedPoint fromInt(int i)
	{
	//	printf("from val=%d, _max=%d min=%d prec = %d\n", i, _max, _min, precission);
		assert(i <= _max);
		assert(i >= _min);
		int32_t val = i * _scaleFactor;
		//printf("ctor with int %d, _val = %d (%x)\n", i, _val, _val);
	    return FixedPoint(val);
	}

	float toFloat() const {
		float ret =  (float)_val / _scaleFactor;
		//printf("to float (%x) will ret %f\n", _val, ret);
		return ret;
	}

	int toInt() const {
		int ret =  _val >> _fractionalBits;
		//printf("to int (%x) will ret %d\n", _val, ret);
		return ret;
	}

	FixedPoint shr(int shift) const
	{
		int32_t temp = _val >> shift;
		return FixedPoint(temp);
	}

	// ret = this * other
	FixedPoint mul( const FixedPoint& other) const
	{
		long long temp = (long long) _val * (long long) other._val;
		int newInit = (int) (temp >> _fractionalBits);	
		return FixedPoint(newInit);
	}

	template <int otherPrecission>
	FixedPoint mulMixed( const FixedPoint<otherPrecission>& other) const
	{
		long long temp = (long long) _val * (long long) other.getRaw();
		int newInit = (int) (temp >> other._fractionalBits);	
		return FixedPoint(newInit);
	}

	// ret = this + other
	FixedPoint add( const FixedPoint& other) const
	{
		return FixedPoint(_val + other._val);
	}

	// ret = this - other
	FixedPoint sub( const FixedPoint& other) const
	{
		return FixedPoint(_val - other._val);
	}

	int32_t getRaw() const { return _val; }
private:
	FixedPoint(int32_t rawBits) : _val(rawBits)
	{
	
	}
	int32_t _val;
};

#endif




