#ifndef SCALEQUANTIZEANDROTATE
#define SCALEQUANTIZEANDROTATE

#include "ChromaticQuantizer.h"
#include "ScaleQuantizer.h"

//#define _SQAR

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

#ifdef _SQAR
		printf("sqar: separated %d into p=%d oct = %d\n", inputPitch, semi, octave);
#endif
		int quantizedSemi = ScaleQuantizer::quantize_semi_expanded(semi, expandedScale, scaleLength);

#ifdef _SQAR
		printf("sqar quant to %d\n", quantizedSemi);
#endif
		reNormalize(octave, quantizedSemi);

#ifdef _SQAR
		printf("sqar renorm to oct %d, se %d\n", octave, quantizedSemi);
#endif
		quantizedSemi += rotation;

#ifdef _SQAR
		printf("sqar rotate to %d\n",  quantizedSemi);
#endif
		if (quantizedSemi > 11)
			quantizedSemi -= 12;
		
		const int ret =  octave * 12 + quantizedSemi;
#ifdef _SQAR
		printf("sqar after wrap to %d, ret=%d\n",  quantizedSemi, ret);
#endif
		return ret;
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