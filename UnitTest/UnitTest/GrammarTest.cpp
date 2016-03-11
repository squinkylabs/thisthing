
#include "stdafx.h"

#include "Random.h"

static const int numRules = 3;
static Random r;

class ProductionRuleEntry
{
public:
	ProductionRuleEntry() : probability(0), code(0) {}
	unsigned char probability;	// 1..256
	unsigned char code;			// what to do if this one fires
};

class ProductionRule
{
public:
	static const int numEntries=3;
	ProductionRule() {}
	void evaluate()
	{
		assert(_isValid());
		int rand = r.get() & 0xff;
		printf("rand is %d\n", rand);
		for (int i=0; i<numEntries; ++i)
		{
			printf("prob is %d\n",  entries[i].probability);
			if ( entries[i].probability >= rand)
			{
				// rule fired!
				printf("rule fired! execute code %d\n",  entries[i].code);
				return;
			}
		}
		// no rule fired
		//printf("no rule fired, so will exectue code %d\n", code);
		assert(false);

	}

	ProductionRuleEntry entries[numEntries];

	bool _isValid() const 
	{
		int last = -1;
		for (int i=0; i<numEntries; ++i)
		{
			const ProductionRuleEntry& e = entries[i];
			if (e.probability <= last)			// probabilites grow
			{
				printf("probability not growing is %d was %d\n", e.probability, last);
				return false;
			}
			if (e.probability == 0xff)
				return true;					// must have a 255 to end it			
		}
		printf("not terminated\n");
		return false;
	}
};


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
	rules[0].evaluate();
}
void GrammarTest()
{
	gt0();
}

