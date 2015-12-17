
#ifndef INCLUDE_DISCRETELOOKUP
#define INCLUDE_DISCRETELOOKUP

#include "LinearInterp.h"

// A version of linear interp that then maps the outputs to a lookup table
template<int size>
class DiscreteLookup
{
public:
	DiscreteLookup() : _interp(0, size-1), _step(0)
	{
	}
	int process(int x) 
	{
		_step = _interp.interp(x);
		assert(_step < size && _step >=0);
		//printf("in lookup x = %d, step=%d\n", x, _step);
		return _table[_step];
	}
	void setValue(int step, int value)
	{
		assert(step < size && step >=0);
		_table[step] = value;
	}
	int getStepValue(int step)
	{
		assert(step < size && step >=0);
		return _table[step];
	}
	int getStepNumber() const { return _step; }
	void setStepNumber(int x)
	{ 
		assert(x >= 0 && x < size);
		_step = x; 
	}
private:
	LinearInterp _interp;
	int _table[size];
	int _step;			// current quantized value
};
	


#endif