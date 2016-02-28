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

int i_global;
int led_test;
int led_mode;
extern "C" void Led_setTempSelectorOverride(int value, int mode)
{
	led_test = value;
	led_mode = mode;
}


int _tmain(int argc, _TCHAR* argv[])
{
#if 0
	printf("starting unit tests\n");
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
	//LookupTableTest();
	
	
	//SlopeDetectorModuleTests();
	//CounterTests();
	//GateTriggerTests();
	SampleAndHoldTest();
	//DelayTest();
	//QuantizerTests();
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

