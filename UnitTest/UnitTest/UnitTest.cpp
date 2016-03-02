// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern void DelayTest();
extern void DACVoltageTest();
extern void GateTriggerTests();
extern void SampleAndHoldTest();
extern void LPFilter();
extern void RandomTest();
extern void SlopeDetectorTests();
extern void SlopeDetectorTests2();
extern void MiscTests();
extern void TestLFO();
extern void FixedPointTest();
extern void LookupTableTest();
extern void SlopeDetectorModuleTests();
extern void SampleAndHoldModuleTests();
extern void LinearInterpTests();
extern void CounterTests();
extern void QuantizerTests();
extern void AdderTests();
extern void Calibrate();
extern void Ala152Test();
extern void SwitchTests();
extern void BankTests();

int i_global;
int led_test;
int led_mode;
int global_bank_number=10;

extern "C" void Led_setTempSelectorOverride(int value, int mode)
{
	led_test = value;
	led_mode = mode;
	//printf("test set led test to %d, mode to %d\n", led_test, led_mode);
}


int _tmain(int argc, _TCHAR* argv[])
{
#if 1
	printf("starting unit tests\n");
	BankTests();
	FixedPointTest();
	LinearInterpTests();
	
	DACVoltageTest();
	GateTriggerTests();
	SampleAndHoldTest();
	Ala152Test();

	DelayTest();

	LPFilter();
	LookupTableTest();
	RandomTest();
	SlopeDetectorTests();
	SlopeDetectorTests2();
	SlopeDetectorModuleTests();
	MiscTests();
	TestLFO();
	CounterTests();
	QuantizerTests();
	AdderTests();
	SwitchTests();
#else
	printf("skipping most tests\n");
	BankTests();
	//LookupTableTest();
	
	
	//SlopeDetectorModuleTests();
	//CounterTests();
	//GateTriggerTests();

	//SampleAndHoldTest();

	//DelayTest();
	QuantizerTests();
	//LinearInterpTests();
	//DACVoltageTest();
	//AdderTests();
	
	//SlopeDetectorTests();
	//SlopeDetectorTests2();
	//TestLFO();

	//Ala152Test();
	//SwitchTests();

#endif
	printf("leaving unit tests\n");
	getchar();

	return 0;
}

