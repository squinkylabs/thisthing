
#ifndef INCLUDE_DIATONICPITCHMAPER
#define INCLUDE_DIATONICPITCHMAPER

#include "ChromaticQuantizer.h"

/** maps midi notes to neareast c major pitches
 */

class DiatonicPitchMapper
{
public:
	static int map(int midiIn)
	{
		int octave, semi;
		ChromaticQuantizer::separate(midiIn, octave, semi);
		
		int ret = midiIn;
		switch(semi)
		{
		case 0:			// c-> c
		case 2:			// d->d
		case 4:			// e -> e;
		case 5:			// f -> f
		case 7:			// g -> g
		case 9:			// a -> a
		case 11:		// b -> b
			ret = midiIn;
			break;
		case 1:			// c# -> d
		case 3:			// d# -> e
		case 6:			// f# -> g
		case 8:			// g# -> a
		case 10:		// a# -> b
			ret = midiIn+1;
			break;
		default:
			assert(0);
		}
		return ret;
	}
};

#endif

