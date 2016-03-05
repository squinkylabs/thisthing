
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
	testm1(m, 0, 1, 0);		// c# -> c in scale0
}

void QuantizerModuleTests()
{
	qm0();
}