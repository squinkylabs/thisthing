
#ifndef DMSCALEQUANTIZER
#define  DMSCALEQUANTIZER

#include "DModule.h"
class DMScaleQuantizer1 : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		a = b = 0;
	}
};
#endif