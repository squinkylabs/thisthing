
#include "stdafx.h"
#include "DMBankSelector.h"

extern int led_test;
extern int led_mode;

extern int getBankNumber();


static void bt0()
{
	DMBankSelector bs(2);		// dual bank
	//global_bank_number = -1;
	setBankNumber(-1);
	led_test = -1;
	led_mode = -2;

	int a=1, b=2;
	// bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b
	ZState z(0);

	bs.go(false, 0, 0, z, a, b);
	
	assert(a == 0);
	assert(b == 0);
	assert(getBankNumber() == 0);
	assert(led_test == 1);
	assert(led_mode == 1);

	z = ZState(0x3ff);

	bs.go(false, 0, 0, z, a, b);
	
	assert(a == 0);
	assert(b == 0);
	assert(getBankNumber() == 1);
	assert(led_test == 2);

}


void BankTests()
{
	bt0();
}