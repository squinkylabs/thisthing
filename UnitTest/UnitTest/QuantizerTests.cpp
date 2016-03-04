#include "stdafx.h"

#include "DMQuantizer.h"
#include "DACVoltage.h"
#include "DMQuantizer.h"
#include "DMAdders.h"
#include <algorithm>




void qt0()
{
	printf("qt0\n");
	ChromaticQuantizer q;
	const int v = ChromaticQuantizer::middleCV;
	q.go(v);

	assert(q.getMIDI() == ChromaticQuantizer::middleCMIDI);

	assert(ChromaticQuantizer::midi2CV( ChromaticQuantizer::middleCMIDI ) ==
		ChromaticQuantizer::middleCV);
}


void qt1()
{
	printf("qt1\n");
	ChromaticQuantizer q;
	const int c5 = ChromaticQuantizer::middleCV + ChromaticQuantizer::octaveV;
	q.go(c5);

	// middle C = C4
	assert(q.getMIDI() == (ChromaticQuantizer::middleCMIDI + 12));
}


void qt2()
{
	printf("qt2\n");
	ChromaticQuantizer q;
	const int c5 = ChromaticQuantizer::middleCV - ChromaticQuantizer::semiV;
	q.go(c5);

	// middle C = C4
	assert(q.getMIDI() == (ChromaticQuantizer::middleCMIDI - 1));
}


// test a range of "on the money" pitches, pos and negatie
void qt3()
{
	printf("qt3\n");
	ChromaticQuantizer q;
	// semis all around middle C
	for (int i = -100; i < 200; ++i)
	{
		const int v =  ChromaticQuantizer::middleCV  + i * ChromaticQuantizer::semiV;
		q.go(v);
		assert(q.getMIDI() == i + ChromaticQuantizer::middleCMIDI);

	}
}


// test rounding, pos and neg
void qt4()
{
	printf("qt4\n");
	ChromaticQuantizer q;
	// semis all around middle C
	const int delta =  (ChromaticQuantizer::semiV / 2) - 1;
	for (int i = -100; i < 200; ++i)
	{
		const int v =  ChromaticQuantizer::middleCV  + i * ChromaticQuantizer::semiV;
		const int expectedMIDI = i + ChromaticQuantizer::middleCMIDI;
		q.go(v);
		assert(q.getMIDI() == expectedMIDI);

		q.go(v + delta);
		assert(q.getMIDI() == expectedMIDI);

		q.go(v - delta);
		assert(q.getMIDI() == expectedMIDI);
	}
}

// simple hysteresis
void qt5()
{
	printf("qt5\n");
	assert(ChromaticQuantizer::hysteresisV > 0);
	ChromaticQuantizer q;

	//int offset = 10;

	for (int offset = -100; offset < 100; ++offset)
	{
	q.go( ChromaticQuantizer::middleCV  - 50);	// prime with super low v

	const int base = ChromaticQuantizer::middleCV + offset *  ChromaticQuantizer::semiV;
	const int expect = ChromaticQuantizer::middleCMIDI + offset;

	q.go( base );
	assert(q.getMIDI() == expect);

	// go up halfway, should not triger
	q.go( base + ChromaticQuantizer::semiV / 2);
	assert(q.getMIDI() == expect);

	// go up all way, should  triger
	q.go( base + ChromaticQuantizer::semiV );
	assert(q.getMIDI() ==expect+1);

	// go down halfway, should not triger
	q.go( base + ChromaticQuantizer::semiV / 2);
	assert(q.getMIDI() == expect + 1);

	// back down
	q.go( base );
	assert(q.getMIDI() == expect);
	}
}



static bool within(int a, int b, int limit)
{
	return std::abs(a -b ) <= limit;		
}

void qt6()
{
	const int c1 = ChromaticQuantizer::middleCV;
	const int c2 = ChromaticQuantizer::middleCV + ChromaticQuantizer::octaveV;
	const int c3 = ChromaticQuantizer::middleCV + 2 * ChromaticQuantizer::octaveV;
	const int c5 = ChromaticQuantizer::middleCV + 4 * ChromaticQuantizer::octaveV;

	ChromaticQuantizer q;

	q.go(c1);
	int midi = q.getMIDI();
	const int c1b = q.midi2CV(midi);
	assert(c1 == c1b);

	q.go(c2);
	midi = q.getMIDI();
	const int c2b = q.midi2CV(midi);
	// code of 8 is about 16 uv. that's pretty good.
	assert(within(c2, c2b, 8));


	q.go(c3);
	midi = q.getMIDI();
	const int c3b = q.midi2CV(midi);
	assert(within(c3, c3b, 16));

	q.go(c5);
	midi = q.getMIDI();
	const int c5b = q.midi2CV(midi);
	assert(within(c5, c5b, 32));

};

// TODO: this is very bad test coverate
void dq0()
{
	DMDiatonicQuantizer q;
	int a, b;

	
	const int v = ChromaticQuantizer::middleCV;
	ZState z(0, true);
	q.go(false, v,0,z, a, b);
//	assert(a == ChromaticQuantizer::middleCMIDI);

	assert( ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI ==
		ChromaticQuantizer::middleCV));

	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI));
}



	
void dq1()
{
	printf("dq1\n");
	DMFPrecisionFMHelper q;
	int a, b;

	ZState z(0, true);
	int y = ChromaticQuantizer::middleCV;
	q.go(false, 0,y,z, a, b);

	assert( ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI ==
		ChromaticQuantizer::middleCV));

	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI));
	assert(b == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI));

	// D is second -> D
	y = ChromaticQuantizer::middleCV + 2 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 2));

	// C# -> D
	y = ChromaticQuantizer::middleCV + 1 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 2));

	// D# is m3 -> D#
	y = ChromaticQuantizer::middleCV + 3 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 3));

	//E->E
	y = ChromaticQuantizer::middleCV + 4 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 4));

//f -> f
	y = ChromaticQuantizer::middleCV + 5 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 5));

//f#->g
	y = ChromaticQuantizer::middleCV + 6 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 7));

	//g->g
	y = ChromaticQuantizer::middleCV + 7 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 7));

	//g#->g
	y = ChromaticQuantizer::middleCV + 8 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 7));

	//a -> c
	y = ChromaticQuantizer::middleCV + 9 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 12));

	//a# -> c
	y = ChromaticQuantizer::middleCV + 10 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 12));

		//b -> c
	y = ChromaticQuantizer::middleCV + 11 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 12));

	// higher octaves
	y = ChromaticQuantizer::middleCV + 24 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI + 24));

	// higher octaves
	y = ChromaticQuantizer::middleCV - 24 *ChromaticQuantizer::semiV;
	q.go(false, 0,y,z, a, b);
	assert(a == ChromaticQuantizer::midi2CV(ChromaticQuantizer::middleCMIDI - 24));


}

// test division to octave semi
void pm0()
{
	int oct, semi;
	ChromaticQuantizer::separate(ChromaticQuantizer::middleCMIDI, oct, semi);
	assert(semi == 0);
	assert( oct == 4);

	ChromaticQuantizer::separate(ChromaticQuantizer::middleCMIDI + 1, oct, semi);
	assert(semi == 1);
	assert( oct == 4);

	ChromaticQuantizer::separate(ChromaticQuantizer::middleCMIDI - 1, oct, semi);
	assert(semi == 11);
	assert( oct == 3);


	//int xposeOct = -10;

	for (int xposeOct = 0; xposeOct > -10; --xposeOct)
	{
		//printf("xpose oct = %d\n", xposeOct);
		int shift = 12 * xposeOct;
		ChromaticQuantizer::separate(ChromaticQuantizer::middleCMIDI + shift, oct, semi);
		assert(semi == 0);
		assert( oct == 4+xposeOct);

		ChromaticQuantizer::separate(ChromaticQuantizer::middleCMIDI + 1 + shift, oct, semi);
		assert(semi == 1);
		assert( oct == 4+xposeOct);

		ChromaticQuantizer::separate(ChromaticQuantizer::middleCMIDI - 1 + shift, oct, semi);
		assert(semi == 11);
		assert( oct == 3+xposeOct);
	}
}


void pm1()
{
	assert(DiatonicPitchMapper::map(ChromaticQuantizer::middleCMIDI) == ChromaticQuantizer::middleCMIDI);
	assert(DiatonicPitchMapper::map(ChromaticQuantizer::middleCMIDI + 12) == ChromaticQuantizer::middleCMIDI + 12);
	assert(DiatonicPitchMapper::map(ChromaticQuantizer::middleCMIDI - 12) == ChromaticQuantizer::middleCMIDI - 12);

}



void pm2()
{
	assert(DiatonicPitchMapper::map(ChromaticQuantizer::middleCMIDI+1) == ChromaticQuantizer::middleCMIDI+2);
}

void QuantizerTests()
{
#if 1
	qt0();
	qt1();
	qt2();
	qt3();
	qt4();

	qt5();
	qt6();

	dq0();
	dq1();

	pm0();
#endif
	pm1();
	pm2();
}