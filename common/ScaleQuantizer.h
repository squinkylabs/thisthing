
#ifndef SCALE_QUANTIZER
#define SCALE_QUANTIZER
class ScaleQuantizer
{
public:
	/*  semi is 0..11 normalized semitone.
	 * scale is a list of normalized semitones, terminated with < 0 entry
	 * return is nearest semi in scale, NOT normalized
	 * if note not in scale, will pick nearest.
	 * if equal distance, will round down
	 */
	static char quantize_semi(char semi, char * scale)
	{
#ifdef _DEBUG
		assert(semi >= 0 && semi <= 11);
		int last = -1;
		for (char * p = scale; *p >= 0; ++p)
		{
			int x = *p;
			assert (x >= 0 && x <= 11);
			assert (x > last);
			last = x;

		}
#endif
		char ret = 0;
		for (bool done=false; !done; ++scale)
		{
			if (*scale < 0)
			{
				printf("terminate on end of scale, ret = %d\n", ret);
				done = true;
			}
			else if (*scale == semi)
			{
				ret = semi;
				done = true;
				printf("terminate on match, ret = %d\n", ret);
			}
		}
		return ret;
	}
};
#endif