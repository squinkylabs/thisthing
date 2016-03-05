#ifndef OCTAVESCALEMANAGER
#define OCTAVESCALEMANAGER

#include "ScaleQuantizer.h"

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
