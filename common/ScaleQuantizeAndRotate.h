#ifndef SCALEQUANTIZEANDROTATE
#define SCALEQUANTIZEANDROTATE

#include "ChromaticQuantizer.h"
#include "ScaleQuantizer.h"

/* handles quantizing to a scale and then rotating the output
 */
class ScaleQuantizeAndRotate
{
public:
	static int quantizeAndRotate(int scaleLength, const char * expandedScale, int rotation, int inputPitch)
	{
		assert(rotation>=0 && rotation < 12);
		// divide input into octaves
		int octave;
		int semi;
		ChromaticQuantizer::separate(inputPitch, octave, semi);
		++octave;		// TODO: make separate return less crazy octaves.

		//printf("separated %d into p=%d oct = %d\n", inputPitch, semi, octave);
		int quantizedSemi = ScaleQuantizer::quantize_semi_expanded(semi, expandedScale, scaleLength);
		reNormalize(octave, quantizedSemi);

		quantizedSemi += rotation;
		if (quantizedSemi > 11)
			quantizedSemi -= 12;
		
		return octave * 12 + quantizedSemi;
	}

	static void reNormalize(int& octave, int& semi)
	{
		if (semi >= 12)
		{
			++octave;
			semi -= 12;
		}
		else if (semi < 0)
		{
			--octave;
			semi += 12;
		}
		assert(semi >=0 && semi < 12);
	}
};
#endif