
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

	ts.clock();
	bool b = ts.getTrigger();
	assert(!b);
}

// test event at zero fires at zero
static void ts0b()
{
	printf("ts0b\n");
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 0 },
		{ TriggerSequencer::END, 100 }
	};
	TriggerSequencer ts(seq);

	ts.clock();
	assert(ts.getTrigger());

	ts.clock();
	assert(!ts.getTrigger());

}

// test trigger at 1 happens at 1
static void ts1()
{
	printf("ts1\n");
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 1 },
		{ TriggerSequencer::END, 0 }
	};
	TriggerSequencer ts(seq);

	ts.clock();
	assert(!ts.getTrigger());

	ts.clock();
	assert(ts.getTrigger());

	ts.clock();
	assert(!ts.getTrigger());

	ts.clock();
	assert(!ts.getTrigger());
}


// 8 clock loop: delay 8, trigger, end
static void ts2()
{
	printf("ts2\n");
#if 0
	printf("finish ts2\n");
	assert(false);
#else
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 8 },
//		{ TriggerSequencer::LOOP, 0 },
		{ TriggerSequencer::END, 0 }
	};
	TriggerSequencer ts(seq);

	for (int i=0; i< 4; ++i)
	{
		printf("--- loop ----\n");
		// 7 none, 1  trigger
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());

		ts.clock(); assert(ts.getTrigger());
		assert(ts.getEnd());

		ts.reset(seq);
	}
#endif

	
}

void GrammarTest()
{
	printf("skpping a bunch of grammr tests\n");
#if 0
	gtk();
	gt0();
	gt1();
	for (int i=0; i<10; ++i) gt2();
#endif


	ts0();
	ts0b();
	ts1();
	ts2();

}

