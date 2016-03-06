#ifndef OCTAVESCALEMANAGER
#define OCTAVESCALEMANAGER

#include "ScaleQuantizer.h"

/* a simple wrapper around a few pre-set scales
 * also hold a "current selection" as an expanded octave scale
 */
class OctaveScaleManager
{
public:
	static int getNumOctaveScales() { return 10; }

	// returns unexpanded scale: 0..11 -1
	static const char * getOctaveScale(int index);

	// pick one of the scales and expand it
	void select(int index)
	{
		const char * unexpanded = getOctaveScale(index);
		_size = ScaleQuantizer::expandScale(_expandedScale, unexpanded);
	}

	// pick one of the scales, shift it, and and expand it
	void selectAndShift(int index, int shift)
	{
		const char * unexpanded = getOctaveScale(index);
		_size = ScaleQuantizer::expandScaleShift(_expandedScale, unexpanded, shift);
	}

	const char * get(char& outSize) const
	{
		outSize = _size;
		return _expandedScale;
	}
	OctaveScaleManager()
	{
		select(0);
	}
private:
	char _expandedScale[ScaleQuantizer::expandedSize];
	char _size;
};

#endif
