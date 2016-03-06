#ifndef INCLUDE_QUANTIZER
#define  INCLUDE_QUANTIZER

#include "DACVoltage.h"

/* quantized continuous voltage to a midi note number,
* and the oposite
*/
class ChromaticQuantizer
{
public:
	ChromaticQuantizer() { reset(); }

	void go(int);
	int getMIDI() const { return _curMIDI; } 

	static int midi2CV(int midi)
	{
		int midiDelta = midi - (ChromaticQuantizer::middleCMIDI);
		//printf("++ in midi2CV, midi in = %d,  middleC = %d midiDelta=%d midcmidi=%d\n", midi, ChromaticQuantizer::middleCV, midiDelta, ChromaticQuantizer::middleCMIDI);
		return ChromaticQuantizer::middleCV + (midiDelta * semiV);
	}

	/* separates a "midi" note number into octave and semitone
	* 0 <= semititone < 12
	*/
	static void separate(int midiIn, int& octave, int&semi);

	void reset()
	{
		 _curMIDI =0;
		 _lastCapturedV=0;
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

	// Offset away from zero, so that when we do the division
	// it will end out rounding evently to nearest
	if (deltaMidC >= 0)
	{
		deltaMidC += (semiV / 2);	// round
	}
	else
	{
		deltaMidC -= (semiV / 2);	// round
	}

	// add in an offset depending on direction voltage is trending,
	// to implement hysteresis
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

	// now do division to go from voltage to midi number 
	// (with correct rounding and hysteresis)
	const int semiFromMidC = deltaMidC / semiV;

	int midi =  semiFromMidC + middleCMIDI;
	if (midi != _curMIDI)
	{
		_curMIDI = midi; 
		_lastCapturedV = v;
	}

	//printf("delta=%d semiFrom=%d midi=%d semiV=%d goingup=%d\n", deltaMidC, semiFromMidC, _curMIDI, semiV, goingUp);
}

inline void ChromaticQuantizer::separate(int midiIn, int& octave, int&semi)
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


#endif