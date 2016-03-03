
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
		char last_scale_entry = -1;
		for (bool done=false; !done; ++scale)
		{
			if (*scale < 0)
			{
				// if scale over and we found nothing, pick last entry.
				// will not handle wrap around, but OK for now
				if (last_scale_entry >= 0)
				{
					ret = last_scale_entry;
				}
				printf("terminate on end of scale, ret = %d\n", ret);
				done = true;
			}
			else if (*scale == semi)
			{
				ret = semi;
				done = true;
				printf("terminate on match, ret = %d\n", ret);
			}
#if 1
			else if (semi < *scale)
			{
				printf("scale search went past semi note\n");
				if (last_scale_entry >= 0)
				{
					printf("but there was a previous one\n");

					const int dprev = semi - last_scale_entry;
					const int dnext = *scale - semi;
					printf("dprev=%d dnext=%d last=%d, scale=%d semi=%d\n", dprev, dnext, last_scale_entry, *scale, semi); 
					assert(dprev > 0 && dnext > 0);

					ret = (dprev <= dnext) ? last_scale_entry : *scale;
					done = true;
					printf(" leave on in between %d\n", ret);


				}
			}
#endif
			last_scale_entry = *scale;
		}
		return ret;
	}
};
#endif