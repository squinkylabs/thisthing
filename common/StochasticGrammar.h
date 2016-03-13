#ifndef STOCHASTIC_GRAMMAR
#define  STOCHASTIC_GRAMMAR

#include "Random.h"

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
	class EvaluationState
	{
	public:
		EvaluationState(Random& xr) : r(xr) {}
		const ProductionRule * rules;
		int numRules;
		Random& r;		//random number generator to use 
	};

	ProductionRule() {}
	ProductionRuleEntry entries[numEntries];
	static void evaluate(const EvaluationState& es, int ruleToEval);
private:
	static int _evaluateRule(const ProductionRule& rule, int random);
	bool _isValid() const;
};


	// generate production, return code for what happened
inline int ProductionRule::_evaluateRule(const ProductionRule& rule, int random)
{
	//int rand = r.get() & 0xff;
	int rand = random & 0xff;
	printf("rand is %d\n", rand);
		//for (int i=0; i<numEntries; ++i)

	int i=0;
	for (bool done2=false; !done2; ++i )
	{
		assert(i<numEntries);
		printf("prob is %d\n",  rule.entries[i].probability);
		if ( rule.entries[i].probability >= rand)
		{
			// rule fired!
			printf("rule fired! execute code %d\n",  rule.entries[i].code);
			return rule.entries[i].code;
		}
	}
	assert(false);	// no rule fired
	return 0;
}

inline void ProductionRule::evaluate(const EvaluationState& es, int ruleToEval)
{
	const ProductionRule& rule = es.rules[ruleToEval];
	assert(rule._isValid());
	int result = _evaluateRule(rule, es.r.get());
	if (result == 0)
	{
		printf("production rule #%d terminated\n", ruleToEval);
		printf("TODO: need to produce something\n");
	}
	else
	{
		printf("production rule #%d expanded to %d\n", ruleToEval, result);
		assert(false);
	}
}
			
inline bool ProductionRule::_isValid() const 
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



#endif
