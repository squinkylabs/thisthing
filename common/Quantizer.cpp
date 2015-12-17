
// we need to avoid stdafx here and instead pull in common stuff
// this is the only cpp file we share between projects
#include "common_headers.h"
//#include <stdio.h>
#include <assert.h>

#include "Quantizer.h"

const int ChromaticQuantizer::middleCV = DACVoltage::xcodeForMV(2000);
const int ChromaticQuantizer::semiV = DACVoltage::xcodeForMV(1000) / 12;
const int ChromaticQuantizer::octaveV = DACVoltage::xcodeForMV(1000);
const int ChromaticQuantizer::middleCMIDI = 60;
const int ChromaticQuantizer::hysteresisV = (1 * semiV) / 8;