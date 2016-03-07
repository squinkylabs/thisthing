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

	// pick one of the scales, rotate it, and and expand it
	// note that this algorithm is based on a mis-understanding of some requiremens, to not used.
	void selectAndRotate(int index, int rotation)
	{
		const char * unexpanded = getOctaveScale(index);
		_size = ScaleQuantizer::expandScaleAndRotate(_expandedScale, unexpanded, rotation);
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
