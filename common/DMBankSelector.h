
#ifndef INCLUDE_DMBANKSELECTOR
#define INCLUDE_DMBANKSELECTOR

#include "LinearInterp.h"
#include "DModule.h"

//extern int global_bank_number;	// this module changes a global. I'm sorry.

extern void setBankNumber(int bank);
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
			int bank_number = _interp.interp(z.value);
			Led_setTempSelectorOverride(1 + bank_number, 1);
			setBankNumber(bank_number);
		}
		a = b = 0;	// we output nothing
	}
private:
	LinearInterp _interp;
};

#endif

