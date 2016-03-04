
#ifndef SCALE_QUANTIZER
#define SCALE_QUANTIZER


/* Definition: "expanded scale" is a one octave scale that:
 *		starts at 0, or lower
 *		ends at 11, or higher
 * note that expanded scales are octave scales - they represent scales that repeat each octave
 *
 * "normalized semitone" is 0..11.
 *
 * expaneded scale makes it easy to quatnize any "normalized" semitone.
 */
class ScaleQuantizer
{
public:
	static const int expandedSize = 13;
	/*  semi is 0..11 normalized semitone.
	* scale is a list of normalized semitones, terminated with < 0 entry
	* scale is "expanded" such that the the lowest entry is <=0 and the highest is >=11
	*
	* return is nearest semi in scale, NOT normalized
	* if note not in scale, will pick nearest.
	* if equal distance, will round down
	*/
	static char quantize_semi_expanded(char semi, const char * _scale, int scale_length);

	/* will quantize "any" pitch to an expanded octave scale
	 */
	static char quantize_expanded(char semi, const char * _scale, int scale_length);


	// returns true if the scale is a valid expanded scale
	static bool check_expanded_scale(const char * scale, int length);

	static int getNumOctaveScales() { return 10; }


	// returns unexpanded scale: 0..11 -1
	static const char * getOctaveScale(int index);

	// returns length
	static int expandScale(char * expanded, const char * scale);

};

inline char ScaleQuantizer::quantize_expanded(char semi, const char * scale, int scale_length)
{
	return quantize_semi_expanded(semi, scale, scale_length);
}

inline char ScaleQuantizer::quantize_semi_expanded(char semi, const char * _scale, int scale_length)
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
			}
		}
		assert(false);
		return 0;
	}


inline bool ScaleQuantizer::check_expanded_scale(const char * scale, int length)
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

inline int ScaleQuantizer::expandScale(char * expanded, const char * scale) 
{
	char first = *scale;
	char last = 0;

	//	printf("init scale: ");
	for (const char *p=scale; *p != -1; ++p)
	{
		last = *p;
		//printf("%d,", last);
	}
	//	printf("\n");

	char * outp = expanded;

	int len = 0;
	// expand first entry, if needed
	if (first > 0)
	{
		//printf("first > 0\n");
		*outp++ = last - 12;
		++len;
	}

	for (const char *p=scale; *p != -1; ++p)
	{
		*outp++ = *p;
		++len;
	}

	if (last < 11)
	{
		*outp++ = first + 12;
		++len;
	}

	//printf("expanded: ");

	for (int i=0; i<len; ++i)
	{
		//printf("%d,", expanded[i]);

	}
	//printf("\n len=%d\n", len);
	assert(check_expanded_scale(expanded, len));
	return len;
}



#endif