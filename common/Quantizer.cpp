
// we need to avoid stdafx here and instead pull in common stuff
// this is the only cpp file we share between projects
#include "common_headers.h"
#include <stdio.h>
#include <assert.h>

#include "ChromaticQuantizer.h"
#include "OctaveScaleManager.h"


#ifndef _MSC_VER
#define printf (void *)
#endif
#include "ScaleQuantizer.h"
#include "StochasticGrammar.h"

const int ChromaticQuantizer::middleCV = DACVoltage::xcodeForMV(2000);
const int ChromaticQuantizer::semiV = DACVoltage::xcodeForMV(1000) / 12;
const int ChromaticQuantizer::octaveV = DACVoltage::xcodeForMV(1000);
const int ChromaticQuantizer::middleCMIDI = 60;
const int ChromaticQuantizer::hysteresisV = (1 * semiV) / 8;


// C
const char  s0[] = {0, -1 }; 

//C, G
const char  s1[] = {0, 7, -1 };

//C, D, E, G, A
const char  s2[] = {0, 2, 4, 7, 9, -1 };

//C, D, E, F, G, A, B
const char  s3[] = {0, 2, 4, 5, 7, 9, 11, -1 };

//C, Eb, G, Bb
const char  s4[] = {0, 3, 7, 10, -1 };

//C, D, Eb, F, G, Ab, Bb
const char  s5[] = {0, 2, 3, 5, 7, 8, 10, -1 };

//C, D, F, G, A
const char  s6[] = {0, 2, 5, 7, 9, -1 };

//C, D, Eb, F, G, Ab
const char  s7[] = {0, 2, 3, 5, 7, 8, -1 };

//D, E, G, A, B
const char  s8[] = { 2, 4, 7, 9, 11, -1 };

//D, E, F, G, A, Bb
const char  s9[] = { 2, 4, 5, 7, 9, 10, -1 };

static const char * octave_scales[10] = {
	s0, s1, s2, s3, s4, s5, s6, s7, s8, s9
};

const char * OctaveScaleManager::getOctaveScale(int index)
{
	return octave_scales[index];
}

//////////////////////////////////////////////////////////////////

static ProductionRule rules0[fullRuleTableSize];
static ProductionRule rules1[fullRuleTableSize];
static ProductionRule rules2[fullRuleTableSize];

bool StochasticGrammarDictionary::_didInitRules = false;

void StochasticGrammarDictionary::initRules()
{
	initRule0();
	initRule1();
	initRule2();
}




// super dumb one - makes quarter notes
void StochasticGrammarDictionary::initRule0()
{
	// break w2 into w,w prob 100
	{
	ProductionRule& r = rules0[sg_w2];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_ww;	
	}

	// break w into h, h
	{
	ProductionRule& r = rules0[sg_w];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_hh;
	}

	// break h into q,q
	{
	ProductionRule&r = rules0[sg_h];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_qq;
	}
	// stop on q
	{
	ProductionRule&r = rules0[sg_q];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_invalid;
	}
}

void StochasticGrammarDictionary::initRule1()
{
	
	// break w2 into w,w prob 100
	{
		ProductionRule& r = rules1[sg_w2];
		r.entries[0].probability = 255;
		r.entries[0].code = sg_ww;	
	}

	// break w into h, h
	{
		ProductionRule& r = rules1[sg_w];
		r.entries[0].probability = 255;
		r.entries[0].code = sg_hh;
	}

	// break h into q,q, or h
	{
		ProductionRule&r = rules1[sg_h];
		r.entries[0].probability = 180;
		r.entries[0].code = sg_qq;

		r.entries[1].probability = 255;
		r.entries[1].code = sg_invalid;
	}

	// stop on q, or make e
	{
		
		ProductionRule&r = rules1[sg_q];
		r.entries[0].probability = 80;
		r.entries[0].code = sg_ee;

		r.entries[1].probability = 255;
		r.entries[1].code = sg_invalid;
	}

	// stop on e, or make sx
	{
		
		ProductionRule&r = rules1[sg_e];
		r.entries[0].probability = 80;
		r.entries[0].code = sg_sxsx;

		r.entries[1].probability = 255;
		r.entries[1].code = sg_invalid;
	}
	{

		ProductionRule&r = rules1[sg_sx];
		r.entries[0].probability = 255;
		r.entries[0].code = sg_invalid;
	}

}

void StochasticGrammarDictionary::initRule2()
{
	// break w2 into 7+9/8 prob 100
	{
	ProductionRule& r = rules2[sg_w2];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_798;	
	}



	// stop at 9/8
#if 0
	{
	ProductionRule& r = rules2[sg_98];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_invalid;	
	}
#else

	// 9/8 -> different combos
	{
	ProductionRule& r = rules2[sg_98];
	r.entries[0].probability = 128;
	r.entries[0].code = sg_q78;	
	r.entries[1].probability = 255;
	r.entries[1].code = sg_qe68;	
	}

	// 6/8 ->
	{
	ProductionRule& r = rules2[sg_68];
	r.entries[0].probability = 128;
	r.entries[0].code = sg_hq;	
	r.entries[1].probability = 255;
	r.entries[1].code = sg_qh;	
	}

#endif

	//78 -> different combos
	{
	ProductionRule& r = rules2[sg_78];
	r.entries[0].probability = 128;
	r.entries[0].code = sg_qhe;	
	r.entries[1].probability = 255;
	r.entries[1].code = sg_hdq;	
	}



	// terminate on these
	rules2[sg_hdq].makeTerminal();
	rules2[sg_qhe].makeTerminal();
	rules2[sg_q].makeTerminal();
	rules2[sg_dq].makeTerminal();
	rules2[sg_h].makeTerminal();
	rules2[sg_e].makeTerminal();
}


 int StochasticGrammarDictionary::getNumGrammars()
 {
	 return 3;
 }

StochasticGrammarDictionary::Grammar StochasticGrammarDictionary::getGrammar(int index)
 {
	 if (!_didInitRules)
		 initRules();

	assert(index >= 0 && index < getNumGrammars());

	
	Grammar ret;
	ret.firstRule = sg_w2;
	ret.numRules = fullRuleTableSize;

	switch(index)
	{
	case 0:
		ret.rules = rules0;	
		break;
	case 1:
		ret.rules = rules1;	
		break;
	case 2:
		ret.rules = rules2;	
		break;
	default:
		assert(false);
	}
	return ret;
 }

	