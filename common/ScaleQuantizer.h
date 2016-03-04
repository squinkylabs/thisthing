
#ifndef SCALE_QUANTIZER
#define SCALE_QUANTIZER
class ScaleQuantizer
{
public:
	/*  semi is 0..11 normalized semitone.
	 * scale is a list of normalized semitones, terminated with < 0 entry
	 * scale is "expanded" such that the the lowest entry is <=0 and the highest is >=11
	 *
	 * return is nearest semi in scale, NOT normalized
	 * if note not in scale, will pick nearest.
	 * if equal distance, will round down
	 */
	static char quantize_semi_expanded(char semi, const char * _scale, int scale_length)
	{
		assert(check_expanded_scale(_scale, scale_length));

		for (int scale_index = 0; true; ++scale_index)
		{
			
			// if we went off the end of the scale, something is wrong
			if (scale_index >= scale_length)
			{
				assert(false);
				return 0;			

			}

			const char scale_semi = _scale[scale_index];

			// if we are on the target, we are golden
			if (scale_semi == semi)
			{
	
				printf("terminate on match, ret = %d\n", semi);
				return semi;
			}

			// if we need to look further, there better be a further
			if (scale_index >= (scale_length-1))
			{
				assert(false);
				return 0;
			}

			const char next_scale_semi = _scale[scale_index+1];

			// if our semi is now in betwee, current and next, we can pick one
			if (semi < next_scale_semi)
			{

				const int dprev = semi - scale_semi;
				const int dnext = next_scale_semi - semi;
				//printf("dprev=%d dnext=%d last=%d, scale=%d semi=%d\n", dprev, dnext, scale_semi, , semi); 
				assert(dprev > 0 && dnext > 0);

				return (dprev <= dnext) ? scale_semi : next_scale_semi;
				//done = true;
				//printf(" leave on in between %d\n", ret);
			}

			//last_scale_entry = *scale;
		}
		assert(false);
		return 0;
	}

	static bool check_expanded_scale(const char * scale, int length)
	{
		// make sure it's increasing
		int last = -100000000;

		for (int i=0; i<length; ++i)
		{
			int x = scale[i];
			if (x <= last)
			{
				printf("scale not increasing x=%d last=%d\n", x, last);
				return false;
			}
			
			last = x;
		}

		if (scale[0] > 0)
		{
			printf("not expanded on bottom\n");
			return false;
		}
		if (last < 11)
		{
			printf("not expanded on top, last = %d\n", last);
			return false;
		}
		
		return true;
	}
};
#endif