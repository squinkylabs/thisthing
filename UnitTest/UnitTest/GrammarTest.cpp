
#include "stdafx.h"
#include "StochasticGrammar.h"

#include "Random.h"

static const int numRules = 3;
//static Random r;



static ProductionRule rules[numRules]; 

static void init0()
{
	// always do zero

#if 1
	ProductionRule& r = rules[0];
	r.entries[0].probability = (unsigned char)255;
	r.entries[0].code = (unsigned char) 0;
#else
	ProductionRule * r = rules;
	r->entries[0].probability = (unsigned char)255;
	r->entries[0].code = (unsigned char) 0;

	printf("now %d\n", r->entries[0].probability);
#endif

	printf("just set prob to 255: %d\n", rules[0].entries[0].probability);


}



void gt0()
{
	init0();
//	rules[0].evaluate();
	Random r;
	ProductionRule::EvaluationState es(r);
	es.rules = rules;
	es.numRules = numRules;
	ProductionRule::evaluate(es, 0);
}
void GrammarTest()
{
	gt0();
}

