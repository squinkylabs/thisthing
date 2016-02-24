
#ifndef INCLUDE_DMBANKSELECTOR
#define INCLUDE_DMBANKSELECTOR

#include "LinearInterp.h"
#include "DModule.h"

extern int global_bank_number;	// this module changes a global. I'm sorry.
class DMBankSelector : public DModule
{
public:
	DMBankSelector(int numBanks) : _interp(0, numBanks-1)
	{
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (z.changed)
		{
			global_bank_number = _interp.interp(z.value);
		}
		a = b = 0;	// we output nothing
	}
private:
	LinearInterp _interp;
};

#endif

