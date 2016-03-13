
#include "stdafx.h"

#include "TriggerSequencer.h"
#include "StochasticGrammar.h"

#include "Random.h"

static const int numRules = 3;
static Random r;



typedef GKEY (* INITFN)();

static ProductionRule rules[numRules]; 


// simplest possible grammar.

static GKEY init0()
{
	// This rule always generate sg-w2 (two whole notes tied together)
	ProductionRule& r = rules[sg_w2];

	// why isn't this entries[0]?
	r.entries[1].probability = (unsigned char)255;
	r.entries[1].code = sg_invalid;		// always terminate

	printf("just set prob to 255: %d\n", rules[0].entries[0].probability);

	return sg_w2;
}

INITFN x = init0;
static void testSub(INITFN f)
{
	GKEY init = f();

	Random r;
	ProductionRule::EvaluationState es(r);
	es.rules = rules;
	es.numRules = numRules;
	ProductionRule::evaluate(es, init);
}



void gt0()
{
	testSub(init0);
}


// simple grammar with a rule but no random
static GKEY init1()
{
	printf("in init1 making simple w,w %d\n", sg_w2);
	{
	// start with w2 durration
	ProductionRule& r = rules[sg_w2];

	// break into w,w prob 100

	r.entries[0].probability = 255;
	r.entries[0].code = sg_ww;		
	}

	{
	// now need rule for w hole
	printf("in init1 making 100 for %d\n", sg_w);
	 ProductionRule& r = rules[sg_w];
	 r.entries[1].probability = (unsigned char)255;
	 r.entries[1].code = sg_invalid;		// always terminate
	}
	 printf("leave init 1. rule 1 p0 = %d\n",  rules[sg_w2].entries[0].probability);
	 return sg_w2;
}

// simple grammar with randomness init
static GKEY init2()
{
	printf("in init2 making 50/50 for %d\n", sg_w2);
	{
	// start with w2 durration
	ProductionRule& r = rules[sg_w2];

	// break into w,w prob 50

	r.entries[0].probability = (unsigned char)255 /2;
	r.entries[0].code = sg_ww;		
	r.entries[1].probability = (unsigned char)255;
	r.entries[1].code = sg_invalid;		// always terminate
	}

	{
	// now need rule for w hole
	printf("in init1 making 100 for %d\n", sg_w);
	 ProductionRule& r = rules[sg_w];
	 r.entries[1].probability = (unsigned char)255;
	 r.entries[1].code = sg_invalid;		// always terminate
	}
	 printf("leave init 1. rule 1 p0 = %d\n",  rules[sg_w2].entries[0].probability);

	return sg_w2;
}


void gt1()
{
	testSub(init1);
}

// test all the gkeys
void gtk()
{
	const int siz = ProductionRuleKeys::bufferSize;
	GKEY buffer[siz];

	for (GKEY gk = sg_first; gk <= sg_last; ++gk)
	{
		printf("testing key %d\n", gk);
		printf("to string: %s\n", ProductionRuleKeys::toString(gk));
		const int dur = ProductionRuleKeys::getDuration(gk);
		ProductionRuleKeys::breakDown(gk, buffer );
		int sum =0;
		for ( GKEY * p = buffer ; *p != sg_invalid; ++p)
		{
			printf("adding to sum %d\n", ProductionRuleKeys::getDuration(*p));
			sum += ProductionRuleKeys::getDuration(*p);
		
		}
		printf("dur = %d sum = %d\n", dur, sum);
		assert(dur == sum);
	}
}

static void ts0()
{
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::END, 100 }
	};
	TriggerSequencer ts(seq);

	bool b = ts.clock();
	assert(!b);
}

static void ts1()
{
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 1 },
		{ TriggerSequencer::END, 0 }
	};
	TriggerSequencer ts(seq);

	bool b = ts.clock();
	assert(!b);

	 b = ts.clock();
	assert(b);

	 b = ts.clock();
	assert(!b);

	 b = ts.clock();
	assert(!b);
}
void GrammarTest()
{
#if 0
	gtk();
	gt0();
	gt1();
	for (int i=0; i<10; ++i) gt2();
#endif


	ts0();
	ts1();

}

