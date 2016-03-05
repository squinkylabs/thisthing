
#include "stdafx.h"
#include "DMScaleQuantizer.h"
#include "ModuleTester.h"
#include "OctaveScaleManager.h"

/* X = CV
 * y = trigger
 * z = scale selector
 */
void testm1(DModule& mod, int index, char inPitch, char outPitch)
{
	ModuleTester mt(mod);

	int pitchv = ChromaticQuantizer::midi2CV(inPitch);
	int gatevlo =  DACVoltage::xcodeForMV(0);
	int gatevhi = DACVoltage::xcodeForMV(10 * 1000);
	int expectedv =  ChromaticQuantizer::midi2CV(outPitch);

	// first select alg
	mt.add( MTIn::z_interp(index, OctaveScaleManager::getNumOctaveScales()));



	// now run pitch in
	mt.add( MTIn::xy(pitchv, gatevlo));

	// trigger with pitch, check output
	mt.add(
		MTIn::xy(pitchv, gatevhi),
		MTCond::ab( expectedv, expectedv)
	);
	assert(mt.run());
}

void qm0()
{
	DMScaleQuantizer1 m;
	testm1(m, 0, 1, 0);		// everything -> c in scale0
	testm1(m, 0, 12, 12);	
	testm1(m, 0, 4, 0);	
	testm1(m, 0, 6, 0);	
	testm1(m, 0, 7, 12);	
}

void qm1()
{
	// C and G
	DMScaleQuantizer1 m;
	testm1(m, 1, 1, 0);		
	testm1(m, 1, 3, 0);		
	testm1(m, 1, 4, 7);		
	testm1(m, 1, 5, 7);		
	testm1(m, 1, 7, 7);
	testm1(m, 1, 9, 7);
	testm1(m, 1, 10, 12);

}

// { 2, 4, 7, 9, 10, -1 };
void qm9()
{
	DMScaleQuantizer1 m;
	testm1(m, 9, 1, 2);	
	testm1(m, 9, 2, 2);
	testm1(m, 9, 3, 2);
	testm1(m, 9, 4, 4);
	testm1(m, 9, 5, 4);
	testm1(m, 9, 6, 7);
	testm1(m, 9, 7, 7);
	testm1(m, 9, 8, 7);
	testm1(m, 9, 9, 9);
	testm1(m, 9, 10, 10);
	testm1(m, 9, 11, 10);
	testm1(m, 9, 12, 10);
	testm1(m, 9, 13, 2+12);
	//testm1(m, 9, 0, 2); //NO, NOT UP 2
	testm1(m, 9, 0, 10 - 12);

	//testm1(m, 9, 4, 4);
	//testm1(m, 9, 0, 12);
}


void QuantizerModuleTests()
{
	qm0();
	qm1();
	qm9();
}