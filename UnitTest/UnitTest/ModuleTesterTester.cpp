
#include "stdafx.h"
#include "ModuleTester.h"
#include "DMMisc.h"
#include "DMAdders.h"


static void mt0()
{
	DMConst dm(55,66);
	ModuleTester mt(dm);

	// no matter what input, should output 55,66
	mt.add(
		MTIn::xy(3,4),
		MTCond::ab(55, 66)
	);
	assert(mt.run());
}

static void mt1()
{
	DMOctaveAdderSubtractor dm;
	ModuleTester mt(dm);

	const int minus5 = DACVoltage::xcodeForMV(-5000);
	const int plus5 = DACVoltage::xcodeForMV(5000);

	// set octave all the way down
	mt.add(
		// z zero, down 5 oct
		MTIn::z_interp(0, 11, 0)
		);

	mt.add(
		MTIn::xy( 0, 0),
		MTCond::ab(minus5, minus5)
		);

	mt.add(
		MTIn::xy( 100, 200),
		MTCond::ab(minus5 +300, minus5 - 100)
		);
	assert(mt.run());
}

void ModuleTesterTest()
{
	mt0();
	mt1();

}