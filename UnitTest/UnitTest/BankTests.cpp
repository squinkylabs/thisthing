
#include "stdafx.h"
#include "DMBankSelector.h"

static void bt0()
{
	DMBankSelector bs(2);		// dual bank
	global_bank_number = -1;

	int a=1, b=2;
	// bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b
	ZState z(0);

	bs.go(false, 0, 0, z, a, b);
	
	assert(a == 0);
	assert(b == 0);
	assert(global_bank_number == 0);

	z = ZState(0x3ff);

	bs.go(false, 0, 0, z, a, b);
	
	assert(a == 0);
	assert(b == 0);
	assert(global_bank_number == 1);

}


void BankTests()
{
	bt0();
}