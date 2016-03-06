
#ifndef SCALE_QUANTIZER
#define SCALE_QUANTIZER

#include "ChromaticQuantizer.h"

//#define _SCVERBOSE

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

	// for unexpanded, terminated with -1
	static bool check_scale(const char * scale);

	static void dumpScale(const char * scale);;


	// returns length
	static int expandScale(char * expanded, const char * scale);
	static int expandScaleShift(char * expanded, const char * scale, char shift);

};

inline char ScaleQuantizer::quantize_expanded(char pitch, const char * scale, int scale_length)
{
	int octave;
	int semi;
	ChromaticQuantizer::separate(pitch, octave, semi);
	++octave;		// TODO: make separate return less crazy octaves.
	//printf("separated %d into p=%d oct = %d\n", pitch, semi, octave);
	char ret = quantize_semi_expanded(semi, scale, scale_length);
	ret += 12 * octave;
	return ret;
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
			//printf("terminate on match, ret = %d\n", semi);
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

#ifdef _MSC_VER
inline void ScaleQuantizer::dumpScale(const char * scale)
{
	printf("dump_scale: ");
	for (const char * p=scale; *p >= 0; ++p) printf("%d, ", *p);
	printf("\n");
}
#endif

inline bool  ScaleQuantizer::check_scale(const char * scale)
{

#ifdef _SCVERBOSE
	// dump it
	printf("check_scale: ");
	dumpScale(scale);
#endif

	// make sure it's increasing
	int last = -100000000;

	if (scale[0] < 0) return false;	// not empty

	for ( ;*scale >= 0; ++scale)
	{
		int x = *scale;
		if (x <= last) return false;
		if (x > 11) return false;

		last = x;
	}

	if (*scale != -1)
	{
#ifdef _SCVERBOSE
		printf("bad terminator is %d\n", *scale);
#endif
		return false;
	}
	return true;

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
			//printf("scale not increasing x=%d last=%d\n", x, last);
			return false;
		}		
		last = x;
	}

	if (scale[0] > 0)
	{
		//printf("not expanded on bottom\n");
		return false;
	}
	if (last < 11)
	{
		//printf("not expanded on top, last = %d\n", last);
		return false;
	}

	return true;
}

inline int ScaleQuantizer::expandScale(char * expanded, const char * scale) 
{
	char first = *scale;
	char last = 0;

	for (const char *p=scale; *p != -1; ++p)
	{
		last = *p;
		//printf("in expand loop, last = %d\n", last);
	}

	char * outp = expanded;

	int len = 0;
	// expand first entry, if needed
	if (first > 0)
	{
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

#if 0
	{
		printf("expanded len=%d: ", len);
		for (int i=0; i<len; ++i) printf("%d, ");
		printf("\n");
	}
#endif

	assert(check_expanded_scale(expanded, len));
	return len;
}

inline int ScaleQuantizer::expandScaleShift(char * expandedOut, const char * scale, char shift)
{
	//printf("exps 1\n");
	//dumpScale(scale);
	assert(shift >= 0 && shift < 12);
	assert(check_scale(scale));

	int unexpandedLen;
	for (unexpandedLen=0; scale[unexpandedLen] >= 0; ++unexpandedLen)
	{
	}
	//printf("unexpanded len = %d\n", unexpandedLen);
	//let's shift the scale into temp
	char temp[expandedSize];
	char temp2[expandedSize];

	// now shift into temp, and remember where we cross octaves
	int overflowIndex=-1;
	for (int i=0; i< unexpandedLen; ++i)
	{
		temp[i] = scale[i] + shift;
		//printf("initialshitf, shift = %d, shifted[%d] = %d\n", shift, i, temp[i]);
		if ((temp[i] > 12) && (overflowIndex < 0))
			overflowIndex = i;

		temp[i+1]=-1;
	} 

#ifdef _SCVERBOSE
	printf("exps 2 overflow index=%d \n", overflowIndex);
	dumpScale(temp);
#endif

	// now let's rotate and normalize into temp2
	// if we shifted past octave
	if (overflowIndex >= 0)
	{
		int i, j;
		// shift the top, overflow, section down, and normalize it
		for (j=0, i= overflowIndex; i<unexpandedLen; ++i, ++j)
		{
			temp2[j] = temp[i] - 12;
#ifdef _SCVERBOSE
			printf("first shift loop, temp[%d]=%d -12 to temp2[%d] is %d\n", i, temp[i], j, temp2[j]);
#endif
		}
		for (i=0, j= unexpandedLen-overflowIndex; i< overflowIndex; ++i, ++j)
		{
			temp2[j] = temp[i];
#ifdef _SCVERBOSE
			printf("second shift loop, temp[%d]=%d  to temp2[%d] is=%d\n", i, temp[i], j, temp2[j]);
#endif
		}
		//printf("exps 3\n");
	}
	else
	{
		for (int i=0; i<unexpandedLen; ++i)
		{
			temp2[i] = temp[i];
			//printf("temp2[%d]=%d\n", i, temp2[i]);
		}
		//printf("exps 4\n");
	}
	temp2[unexpandedLen]=-1;

#ifdef _SCVERBOSE
			printf("terminated at temp2[%d]\n", unexpandedLen);
#endif

	assert( check_scale(temp2));
	const int len = expandScale(expandedOut, temp2);
	return len;
}




#endif