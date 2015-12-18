#ifndef INCLUDE_QUANTIZER
#define  INCLUDE_QUANTIZER

#include "DACVoltage.h"

/* quantized continuous voltage to a midi note number,
 * and the oposite
 */
class ChromaticQuantizer
{
public:
	ChromaticQuantizer() : _curMIDI(0), _lastCapturedV(0) {}

	void go(int);
	int getMIDI() const { return _curMIDI; } 

	static int midi2CV(int midi)
	{
		//assert( ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI ==
		//ChromaticQuantizer::middleCV));



		int midiDelta = midi - (ChromaticQuantizer::middleCMIDI);
		//printf("++ in midi2CV, midi in = %d,  middleC = %d midiDelta=%d midcmidi=%d\n", midi, ChromaticQuantizer::middleCV, midiDelta, ChromaticQuantizer::middleCMIDI);
		return ChromaticQuantizer::middleCV + (midiDelta * semiV);
	}

	const static int middleCV;
	const static int octaveV;
	const static int semiV;
	const static int middleCMIDI;
	const static int hysteresisV;
private:
	int _curMIDI;
	int _lastCapturedV;
};



inline void ChromaticQuantizer::go(int v)
{

	int deltaMidC = v - middleCV;

	if (deltaMidC >= 0)
	{
		deltaMidC += (semiV / 2);	// round
	}
	else
	{
		deltaMidC -= (semiV / 2);	// round
	}

	const bool goingUp = v > _lastCapturedV;
	if (goingUp)
	{
		// raise threshold goign up by reducting v
		deltaMidC -= hysteresisV;
	}
	else
	{
		deltaMidC += hysteresisV;
	}


	const int semiFromMidC = deltaMidC / semiV;


	int midi =  semiFromMidC + middleCMIDI;
	if (midi != _curMIDI)
	{
		_curMIDI = midi; 
		_lastCapturedV = v;
	}
	
	//printf("delta=%d semiFrom=%d midi=%d semiV=%d goingup=%d\n", deltaMidC, semiFromMidC, _curMIDI, semiV, goingUp);


}

#if 0 //version without hysteresis
class ChromaticQuantizer
{
public:
	void go(int);
	int getMIDI() const { return _curMIDI; } 

	const static int middleCV; // = DACVoltage::xcodeForMV(2000);
	const static int octaveV;
	const static int semiV;
	const static int middleCMIDI;
	const static int hysteresisV;
private:
	int _curMIDI;
};

const int ChromaticQuantizer::middleCV = DACVoltage::xcodeForMV(2000);
const int ChromaticQuantizer::semiV = DACVoltage::xcodeForMV(1000) / 12;
const int ChromaticQuantizer::octaveV = DACVoltage::xcodeForMV(1000);
const int ChromaticQuantizer::middleCMIDI = 60;
const int ChromaticQuantizer::hysteresisV = (7 * semiV) / 8;

inline void ChromaticQuantizer::go(int v)
{
	//printf("v=%d ", v);

	int deltaMidC = v - middleCV;

	if (deltaMidC >= 0)
	{
		deltaMidC += (semiV / 2);	// round
	}
	else
	{
		deltaMidC -= (semiV / 2);	// round
	}
	const int semiFromMidC = deltaMidC / semiV;

	_curMIDI = semiFromMidC + middleCMIDI;
	
	//printf("adj=%d delta=%d semiFrom=%d midi=%d semiV=%d\n", v, deltaMidC, semiFromMidC, _curMIDI, semiV);


}
#endif


class DiatonicPitchMapper
{
public:
	static int map(int midiIn)
	{
	//	int octave = (midiIn - middleCMIDI) / 12;
		int octave, semi;
		separate(midiIn, octave, semi);
		
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

	// semi will always be 0..11

	static void separate(int midiIn, int& octave, int&semi)
	{
	//	printf("** separate %d\n", midiIn);
		int xdeltaMiddleC = midiIn - ChromaticQuantizer::middleCMIDI;

		int oct = 4;		// assume we are at middle C
		while (xdeltaMiddleC < 0)
		{
			xdeltaMiddleC+=12;
			oct--;
		}

		while (xdeltaMiddleC > 11)
		{
			xdeltaMiddleC-=12;
			oct++;
		}
		semi = xdeltaMiddleC;
		octave = oct;
		assert(semi>= 00 && semi <= 11);
		//printf("reutrning semi %d octave = %d xdelta = %d adjdelta=%d\n", 
		//	semi, octave, xdeltaMiddleC, adjDelta); 
	}

#if 0
	static void separate(int midiIn, int& octave, int&semi)
	{
		printf("** separate %d\n", midiIn);
		int xdeltaMiddleC = midiIn - ChromaticQuantizer::middleCMIDI;

		int adjDelta = xdeltaMiddleC;

		if (adjDelta >= 0)
			adjDelta += 6;
		else
			adjDelta -= 6;

		octave = 4 + adjDelta / 12;

		// maybe this convesino need rounding, too?
		//semi = xdeltaMiddleC % 12;

		// try this
		semi= midiIn - (octave * 12);	// what's left over?

		if (semi < 0) {
			printf("adj oct from %d to %d semi from %d to %d\n", octave, octave-1, semi, semi + 12);
			semi += 12;
			
			octave--;
		}

		assert(semi>= 00 && semi <= 11);
		printf("reutrning semi %d octave = %d xdelta = %d adjdelta=%d\n", 
			semi, octave, xdeltaMiddleC, adjDelta); 
	}
#endif
};


#endif