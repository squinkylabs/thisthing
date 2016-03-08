
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


/* Test DMScaleQuantizer2,  which quantizes to a rotated scale
 * will also test DMScaleQuanizer3, which uses a normal scale, then rotates after
 */
void testm2(DModule& mod, int rotation, char inPitch, char outPitch)
{
	ModuleTester mt(mod);

	int pitchv = ChromaticQuantizer::midi2CV(inPitch);
	int gatevlo =  DACVoltage::xcodeForMV(0);
	int gatevhi = DACVoltage::xcodeForMV(10 * 1000);
	int expectedv =  ChromaticQuantizer::midi2CV(outPitch);

	// first select shift
	mt.add( MTIn::z_interp(0, 11, rotation));



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

	testm2(m, shift, 0, 1);	// c -> c#
	testm2(m, shift, 1,1);  // c# -> c#
	testm2(m, shift, 2,1);
	testm2(m, shift, 3,3);
	testm2(m, shift, 4,3);
	testm2(m, shift, 5,5);
	testm2(m, shift, 6,5);
	testm2(m, shift, 7,8);
	testm2(m, shift, 8,8);
	testm2(m, shift, 9,8);
	testm2(m, shift, 10, 10);
	testm2(m, shift, 11, 10);
	testm2(m, shift, 12, 1+12);		// C# next octave
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




// no rotation, qm3
void qmc0()
{
	int rotation = 0;

	DMScaleQuantizer3 m;

	//void testm2(DModule& mod, int shift, char inPitch, char outPitch)
	//C, D, E, G, A
	// const char  s2[] = {0, 2, 4, 7, 9, -1 };
	testm2(m, rotation, 0, 0);
	testm2(m, rotation, 1, 0);
	testm2(m, rotation, 2, 2);
	testm2(m, rotation, 3, 2);
	testm2(m, rotation, 4, 4);
	testm2(m, rotation, 5, 4);
	testm2(m, rotation, 6, 7);
	testm2(m, rotation, 7, 7);
	testm2(m, rotation, 9, 9);
	testm2(m, rotation, 10, 9);
	testm2(m, rotation, 2*12, 2*12);
}


void qmc1()
{
	printf("qmc1\n");
	int rotation = 1;

	DMScaleQuantizer3 m;

	//void testm2(DModule& mod, int shift, char inPitch, char outPitch)
	//C, D, E, G, A
	// const char  s2[] = {0, 2, 4, 7, 9, -1 };
	// --> 1, 3, 5, 8, 10,

	testm2(m, rotation, 0, 1);	// c -> c -> c#
	testm2(m, rotation, 1,1);  // c# -> c -> c#
	testm2(m, rotation, 2,3);	// D -> D -> D#
	testm2(m, rotation, 3,3);
	testm2(m, rotation, 4,5);	// E -> E -> F
	testm2(m, rotation, 5,5);
	testm2(m, rotation, 6,8);	// F# -> G -> G#
	testm2(m, rotation, 7,8);
	testm2(m, rotation, 8,8);	// G# -> G -> G#
	testm2(m, rotation, 9,10);	//A -> A -> A#
	testm2(m, rotation, 10, 10); // A# -> A -> A#
	testm2(m, rotation, 11, 12+1); // B -> C -> C#
	testm2(m, rotation, 12, 1+12);	//  C -> C -> C#
}


void qmc9()
{
	printf("qmc9\n");
	int shift = 9;

	DMScaleQuantizer3 m;

	//void testm2(DModule& mod, int shift, char inPitch, char outPitch)
	//C, D, E, G, A
	// const char  s2[] = {0, 2, 4, 7, 9, -1 };
	// --> 9, 11, 13, 16, 18,
	// --> 1, 4, 6, 9, 11, 

	testm2(m, shift, 1,9);	// C# -> C -> A
	testm2(m, shift, 4,13-12);	// E ->  E -> 13 -> 1
	testm2(m, shift, 9, 18-12);	//A -> A -> 18 -> 6
	testm2(m, shift, 6, 16-12);	// F# -> G -> 16 -> 4
	testm2(m, shift, 11, 9+12);	// B -> C2 -> A2
}



void QuantizerModuleTests()
{
	qm0();
	qm1();
	qm9();

	qmb0();
	qmb1();
	qmb9();

	qmc0();
	qmc1();
	qmc9();
}