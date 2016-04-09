
#include "stdafx.h"

#include "DM_SGTriggerGenerator.h"
#include "GenerativeTriggerGenerator.h"
#include "TriggerSequencer.h"
#include "StochasticGrammar.h"

#include "Random.h"
#include <set>
#include <map>

static const int numRules = fullRuleTableSize;
static Random r;



typedef GKEY (* INITFN)();

static ProductionRule rules[numRules]; 

class TestEvaluator : public ProductionRule::EvaluationState
{
public:
	TestEvaluator(Random& xr) : ProductionRule::EvaluationState(xr) {}
	virtual void writeSymbol(GKEY key)
	{
		keys.push_back(key);
	}

	int getNumSymbols() { 
		printf("final keys: ");
		for (size_t i=0; i< keys.size(); ++i) printf("%s, ",  ProductionRuleKeys::toString(keys[i]));
		printf("\n");
		return keys.size();
	}
private:
	std::vector<GKEY> keys;
};


// simplest possible grammar.

static GKEY init0()
{
	printf("called init0\n");
	// This rule always generate sg-w2 (two whole notes tied together)
	ProductionRule& r = rules[sg_w2];

	r.entries[0].probability = (unsigned char)255;
	r.entries[0].code = sg_invalid;		// termindate expansion		


	return sg_w2;
}

INITFN x = init0;
static void testSub(INITFN f)
{
	GKEY init = f();


	bool b = ProductionRule::isGrammarValid(rules, numRules, init);
	assert(b);

	printf("test sub finihsed validating grammar\n");

	Random r;
	TestEvaluator es(r);
	es.rules = rules;
	es.numRules = numRules;
	ProductionRule::evaluate(es, init);

	assert(es.getNumSymbols() > 0);
}



void gt0()
{
	printf("gt0\n");
	testSub(init0);
}


// simple grammar with a rule but no random
static GKEY init1()
{

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
		 r.entries[0].probability = (unsigned char)255;
		 r.entries[1].code = sg_invalid;		
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
	printf("gt1\n");
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
		printf("dur = %d sum = %d (should be the same)\n", dur, sum);
		assert(dur == sum);
	}
}

static void ts0()
{
	printf("ts0\n");
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


// 4 clock loop: delay 4, trigger, end
static void ts2()
{
	printf("ts2\n");
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 4 },
		{ TriggerSequencer::END, 0 }
	};
	TriggerSequencer ts(seq);

	bool firstTime = true;
	// first time through, 4 clocks of nothing. then clock, 0,0,0
	for (int i=0; i< 4; ++i)
	{
		printf("--- loop ----\n");
		
		ts.clock();
		if (firstTime)
		{
			 assert(!ts.getTrigger()); assert(!ts.getEnd());
			 firstTime = false;
		}
		else
		{
			printf("second time around, t=%d e=%d\n", ts.getTrigger(), ts.getEnd());
			// second time around we finally see the trigger

			assert(ts.getTrigger());

			
			// second time around, need to clock the end of the last time
			assert(ts.getEnd());
			ts.reset(seq);				// start it up again

			printf("e\n");
			assert(!ts.getTrigger());	// resetting should not set us up for a trigger
		}
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		

		ts.clock(); assert(!ts.getTrigger());
	//	assert(ts.getEnd());

	//	ts.reset(seq);
	}
}

// test trigger seq qith
// 4 clock loop: trigger, delay 4 end
static void ts3()
{
	printf("ts3\n");
	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 0 },
		{ TriggerSequencer::END, 4 }
	};
	TriggerSequencer ts(seq);


	bool firstLoop = true;
	for (int i=0; i< 4; ++i)
	{
		printf("--- loop ----\n");
	
		// 1
	
		ts.clock();
		if (firstLoop)
		{
			 assert(ts.getTrigger());
			// we just primed loop at top, so it's got a ways
			assert(!ts.getEnd());
			firstLoop = false;
		}
		else
		{
			// second time around, need to clock the end of the last time
			assert(ts.getEnd());
			ts.reset(seq);				// start it up again

			printf("e\n");
			assert(ts.getTrigger());	// resetting should have set us up for a trigger
		}
		// 2
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		// 3
		ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
		// 4
		ts.clock(); 
		assert(!ts.getTrigger());
		assert(!ts.getEnd());
	}	
}

// test trigger seq with straight ahead 4/4 as generated by a grammar
static void ts4()
{
	printf("ts4\n");

	TriggerSequencer::Event seq[] =
	{
		{ TriggerSequencer::TRIGGER, 0 },
		{ TriggerSequencer::TRIGGER, 4 },
		{ TriggerSequencer::TRIGGER, 4 },
		{ TriggerSequencer::TRIGGER, 4 },
		{ TriggerSequencer::END, 4 }
	};
	TriggerSequencer ts(seq);

	
	//bool firstLoop = true;
	for (int i=0; i< 100; ++i)
	{
		bool firstTime = (i==0);
		// repeating pattern of trigg, no, no, no
		for (int j=0; j<4; ++j)
		{
			for (int k=0; k<4; ++k)
			{
			//	printf("test loop, i=%d, j=%d, k=%d\n", i, j, k);
				ts.clock();

				bool expectEnd = (k==0) && (j==0) && !firstTime;
				assert(ts.getEnd() == expectEnd);
				if (ts.getEnd())
				{
					ts.reset(seq);
				}
				assert(ts.getTrigger() == (k==0));
			}
		}
	}
}
/********************************************************************************************

**********************************************************************************************/


// test that we get some clocks and some not
static void gtg0()
{
	printf("gtg0\n");
	GKEY key = init1();
	Random r;
	GenerativeTriggerGenerator gtg(r, rules, numRules, key);
	bool yes = false;
	bool no = false;
	for (int i=0; i<100000; ++i)
	{
	   if (gtg.clock())
		   yes = true;
	   else
		   no = true;

	   if (yes && no)
	   {
		   //printf("clocked at %d\n", i);
		   return;
	   }
	}
	assert(false);
	
}


// test that we get everything in even quarter notes
static void gtg1()
{
	printf("gtg1\n");
	GKEY key = init1();
	std::set<int> counts;

	Random r;
	GenerativeTriggerGenerator gtg(r, rules, numRules, key);

	int ct = 0;
	for (int i=0; i<10000; ++i)
	{
		bool b = gtg.clock();
		if (b)
		{
		   //printf("clocked at %d\n", ct);
		   counts.insert(ct);
		   ct = 0;
		}
		ct++;
	}
	//counts.insert(50);
	assert(!counts.empty());
	for (std::set<int>::iterator it=counts.begin(); it != counts.end(); ++it)
	{
		int c = *it;
		printf("got count %d\n", c);


		 if ((c % PPQ) != 0)
		 {
			 printf("PPQ=%d, c%PPQ=%d\n", PPQ, (c % PPQ));
			 printf("2ppq = %d, 4ppq=%d\n", 2*PPQ, 4*PPQ);
			 assert(false);
		 }
		//assert(false);		// finish me
	}
	
}

void gdt0()
{
	printf("gdt0\n");
	{
		printf("gdt0a\n");
		static ProductionRule rules[numRules]; 
		bool b = ProductionRule::isGrammarValid(rules, numRules, sg_invalid);
		assert(!b);
	}
	{
		// throw in a positive case
		printf("gdt0b\n");
		static ProductionRule rules[numRules]; 
		ProductionRule& r = rules[sg_w];
		r.entries[0].probability = 255;
		r.entries[0].code = sg_invalid;		

		bool b = ProductionRule::isGrammarValid(rules, numRules, sg_w);
		assert(b);
	}
	{
		// terminal code wrong
		printf("gdt0c\n");
		static ProductionRule rules[numRules]; 
		ProductionRule& r = rules[sg_w];
		r.entries[0].probability = 255;
		r.entries[0].code = sg_q;		

		bool b = ProductionRule::isGrammarValid(rules, numRules, sg_w);
		assert(!b);
	}
	{
		// bad order of proability
		printf("gdt0c\n");
		static ProductionRule rules[numRules]; 
		ProductionRule& r = rules[sg_w];
		r.entries[0].probability = 255;
		r.entries[0].code = sg_q;		

		bool b = ProductionRule::isGrammarValid(rules, numRules, sg_w);
		assert(!b);
	}
	{
		// rule branches to nowhere
		printf("gdt0d\n");

		static ProductionRule rules[numRules]; 

	// break w2 into w,w prob 100
	ProductionRule& r = rules[sg_w2];
	r.entries[0].probability = 255;
	r.entries[0].code = sg_ww;
	bool b = ProductionRule::isGrammarValid(rules, numRules, sg_w);
		assert(!b);
	}
}


void gdt1()
{
	printf("--gdt1--\n");
	assert( StochasticGrammarDictionary::getNumGrammars() > 0);
	for (int i=0; i< StochasticGrammarDictionary::getNumGrammars(); ++i)
	{
		printf("-gdt1 test grammar %d\n", i);
		StochasticGrammarDictionary::Grammar g = StochasticGrammarDictionary::getGrammar(i);
		bool b = ProductionRule::isGrammarValid(g.rules, g.numRules, g.firstRule);
		assert(b);
	}
}

//looks up a grammar in the dictionary, makes a sequencer based on it,
// then runs it. returns historgram of durations
static std::map<int, int> runGrammarSeq(int grammarIndex)
{
	
	std::map<int, int> counts;		// key == num ck, val == num
	Random r;

	StochasticGrammarDictionary::Grammar g = StochasticGrammarDictionary::getGrammar(0);
	GenerativeTriggerGenerator gtg(r, g.rules, g.numRules, g.firstRule);

	int ct = 0;
	for (int i=0; i<1000; ++i)
	{
		bool b = gtg.clock();
		if (b)
		{
		   //printf("clocked at %d\n", ct);

		   if (counts.find(ct) == counts.end())
		   {
			   counts[ct] = 1;
		   }
		   else
		   {
			   ++counts[ct];
		   }
		  // counts.insert(ct);
		   ct = 0;
		}
		ct++;
	}
	return counts;
}

// test that we get something from grammar dictionary 0
static void gdt2_0()
{
	printf("gdt2+0\n");


	std::map<int, int> counts;		// key == num ck, val == num
	counts = runGrammarSeq(0);
	
	assert(!counts.empty());
	for (std::map<int, int>::iterator it=counts.begin(); it != counts.end(); ++it)
	{
		int c = it->first;
		printf("got count %d\n", c);


		 if ((c % PPQ) != 0)
		 {
			// printf("PPQ=%d, c%PPQ=%d\n", PPQ, (c % PPQ));
			// printf("2ppq = %d, 4ppq=%d\n", 2*PPQ, 4*PPQ);
			 assert(false);
		 }

		 if (c != PPQ)	// expect all but one to be quarters
		 {
			 assert(c == 0);	// inital zero dealy
			 assert(it->second == 1); // only once
		 }
	}
}

static void gdt2_1()
{
	printf("gdt2_1\n");


	std::map<int, int> counts;		// key == num ck, val == num
	counts = runGrammarSeq(1);
	
	assert(!counts.empty());
}


static void gdt2_2()
{
	printf("gdt2_2\n");


	std::map<int, int> counts;		// key == num ck, val == num
	counts = runGrammarSeq(2);
	
	assert(!counts.empty());
}

static void gdt2()
{
	assert( StochasticGrammarDictionary::getNumGrammars() == 3);
	gdt2_0();
	gdt2_1();
	gdt2_2();
}


void gdm0()
{
	printf("dgm0\n");
	DM_SGTriggerGenerator m;
	DModule& dm = m;

	const int xlo = DACVoltage::xcodeForMV(0);
	const int xhi = DACVoltage::xcodeForMV(10 * 1000);
	const int x1 = DACVoltage::xcodeForMV(1 * 1000);
	int a, b;

	bool gotLow = false;
	bool gotHigh = false;
	for (int i=0; i< 10000; ++i)
	{
		dm.go(false, xlo, 0, ZState(), a, b);
		if (a > x1) gotHigh = true; 
		else gotLow = true;

		dm.go(false, xhi, 0, ZState(), a, b);
		if (a > x1) gotHigh = true; 
		else gotLow = true;
	}
	assert(gotLow);
	assert(gotHigh);

	
	//DModule * p = new DM_SGTriggerGenerator();
	//delete p;
}

void GrammarTest()
{
	
#if 1
	gtk();
	gt0();
	gt1();
	//for (int i=0; i<10; ++i) gt2();



	ts0();
	ts0b();
	ts1();

	ts2();

	ts3();
	ts4();

	gtg0();
#else
	printf("skpping a bunch of grammr tests\n");
#endif
	
	gdt1();
	gdt2();

	gdm0();
}

