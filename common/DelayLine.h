/* 
 * File:   DelayLine.h
 * Author: bruce
 *
 * Created on March 21, 2015, 11:35 AM
 */

#ifndef DELAYLINE_H
#define	DELAYLINE_H

/** DelayLine
 * A simple ring buffer for doing integral delays.
 * template arguments are for numeric type stored, and for max size
 */
template <typename T, int MAXD>
class DelayLine
{
public:
    DelayLine(int currentDelay, T initialValue=0)
	{
		assert(currentDelay <= MAXD);
		_outp = 0;
		_inp = currentDelay;
		for (int i=0; i< MAXD; ++i)
		{
			_memory[i] = initialValue;
		}
		//dump("ctor");
	}

	// shifts 'x' into the delay line, makes next output ready
	void clock(T x)		// add another input
	{
		advance(_inp);
		advance(_outp);
		_memory[_inp] = x;
		//dump("afterclock");
	}
	T get() const		// get the current output
	{
		return _memory[_outp];
		//dump("get");
	}

	void setDelay(int count)
	{
		_outp = _inp - count;
		if (_outp < 0)
			_outp += MAXD;
	}
private:
	T _memory[MAXD];
	char _inp;
	char _outp;

	// move up 'p' (the delay index), wrap around if we hit the end
	// (this is the core of the circular ring buffer
	void advance(char &p)
	{
		if (++p >= MAXD) p =0;
	}

#ifdef _MSC_VER
	void dump(const char *s) const
	{
		printf("DelayLine(%s) in=%d out=%d\n", s, _inp, _outp);
		for (int i=0; i<MAXD; ++i)
		{
			printf(" %d", _memory[i]);
		}
		printf("\n");
	}
#endif
};

#endif	/* DELAYLINE_H */

