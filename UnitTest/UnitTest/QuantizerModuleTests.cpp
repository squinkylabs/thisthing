
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

	// first select scale
	mt.add( MTIn::z_interp(0, OctaveScaleManager::getNumOctaveScales()-1, index));



	// now run pitch in
	mt.add( MTIn::xy(pitchv, gatevlo));

	// trigger with pitch, check output
	mt.add(
		MTIn::xy(pitchv, gatevhi),
		MTCond::ab( expectedv, expectedv)
	);

	// reset, check for zero
	mt.add(
		MTIn::reset(),
		MTCond::ab(0,0)
		);
	assert(mt.run());
}


void testm2(DModule& mod, int shift, char inPitch, char outPitch)
{
	ModuleTester mt(mod);

	int pitchv = ChromaticQuantizer::midi2CV(inPitch);
	int gatevlo =  DACVoltage::xcodeForMV(0);
	int gatevhi = DACVoltage::xcodeForMV(10 * 1000);
	int expectedv =  ChromaticQuantizer::midi2CV(outPitch);

	// first select shift
	mt.add( MTIn::z_interp(0, 11, shift));



	// now run pitch in
	mt.add( MTIn::xy(pitchv, gatevlo));

	// trigger with pitch, check output
	mt.add(
		MTIn::xy(pitchv, gatevhi),
		MTCond::ab( expectedv, expectedv)
	);

		// reset, check for zero
	mt.add(
		MTIn::reset(),
		MTCond::ab(0,0)
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

// no shift, 
void qmb0()
{
	int shift = 0;

	DMScaleQuantizer2 m;

	//void testm2(DModule& mod, int shift, char inPitch, char outPitch)
	//C, D, E, G, A
	// const char  s2[] = {0, 2, 4, 7, 9, -1 };
	testm2(m, shift, 0, 0);
	testm2(m, shift, 1, 0);
	testm2(m, shift, 2, 2);
	testm2(m, shift, 3, 2);
	testm2(m, shift, 4, 4);
	testm2(m, shift, 5, 4);
	testm2(m, shift, 6, 7);
	testm2(m, shift, 7, 7);
	testm2(m, shift, 9, 9);
	testm2(m, shift, 10, 9);
	testm2(m, shift, 2*12, 2*12);
}

void qmb1()
{
	printf("qmb1\n");
	int shift = 1;

	DMScaleQuantizer2 m;

	//void testm2(DModule& mod, int shift, char inPitch, char outPitch)
	//C, D, E, G, A
	// const char  s2[] = {0, 2, 4, 7, 9, -1 };
	// --> 1, 3, 5, 8, 10,

	testm2(m, shift, 1,1);
	testm2(m, shift, 3,3);
	testm2(m, shift, 10, 10);
	testm2(m, shift, 11, 10);
	testm2(m, shift, 12, 1+12);
}

void qmb9()
{
	printf("qmb9\n");
	int shift = 9;

	DMScaleQuantizer2 m;

	//void testm2(DModule& mod, int shift, char inPitch, char outPitch)
	//C, D, E, G, A
	// const char  s2[] = {0, 2, 4, 7, 9, -1 };
	// --> 9, 11, 13, 16, 18,
	// --> 1, 4, 6, 9, 11, 

	testm2(m, shift, 1,1);
	testm2(m, shift, 4,4);
	testm2(m, shift, 9,9);
	testm2(m, shift, 6, 6);
	testm2(m, shift, 11, 11);
}
void QuantizerModuleTests()
{
	qm0();
	qm1();
	qm9();

	qmb0();
	qmb1();
	qmb9();
}