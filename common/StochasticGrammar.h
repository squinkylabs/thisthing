#ifndef STOCHASTIC_GRAMMAR
#define  STOCHASTIC_GRAMMAR

#include "Random.h"

/***********************************************
 ********** rhytmic grouping codes *************
************************************************/
typedef unsigned char GKEY;

// TODO: need to re-think these. Go back to haveing the keys be the table index, too?
// in that case, don't need terminalValue any more....

const GKEY sg_invalid = 0;		// either uninitialized rule, or return value that stops recursion.
								// Note that this means table of production rules must have a dummy entry up front
const GKEY sg_w2	= 1;		// duration of two whole notes
const GKEY sg_w		= 2;		// whole
const GKEY sg_ww	= 3;		// w,w
const GKEY sg_h		= 4;		
const GKEY sg_hh	= 5;
const GKEY sg_q		= 6;
const GKEY sg_e		= 7;
const GKEY sg_ee	= 8;

const int PPQ = 96;

class ProductionRuleKeys
{
public:
	// turn a key into a 0 terminated list of keys for individual notes
	static void breakDown(GKEY key, GKEY * outKeys, int outSize);

	// get the duration in clocks for a "simple" key (like q, not q+q)
	static int getDuration(GKEY key);

	static const char * toString(GKEY key);
};

inline const char * ProductionRuleKeys::toString(GKEY key)
{
	const char * ret;
	switch(key)
	{
	case sg_w2:
		ret = "2xw";
		break;

	default:
		printf("can't print key %d\n", key);
		assert(false);
		ret="error";
	}
	return ret;

}


inline int ProductionRuleKeys::getDuration(GKEY key)
{
	int ret;
	switch(key)
	{
	case sg_w2:
		ret = 2 * 4 * PPQ;
		break;

	default:
		assert(false);
		ret=0;
	}
	return ret;

}
/**************************************************************************

**************************************************************************/

class ProductionRuleEntry
{
public:
	ProductionRuleEntry() : probability(0), code(sg_invalid) {}
	unsigned char probability;	// 1..256
	GKEY code;			// what to do if this one fires
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

	ProductionRule()  {}

	/* the data
	 */

	// each possible production rule for this state
	ProductionRuleEntry entries[numEntries];
//	GKEY terminalValue;		// if nothing produced, this is the output (entire time duration of interval)

	static void evaluate(const EvaluationState& es, int ruleToEval);
private:
	static int _evaluateRule(const ProductionRule& rule, int random);
	bool _isValid(int index) const;
};


	// generate production, return code for what happened
inline int ProductionRule::_evaluateRule(const ProductionRule& rule, int random)
{
	//int rand = r.get() & 0xff;
	int rand = random & 0xff;
	printf("evaluateRule called with rand is %d\n", rand);
		//for (int i=0; i<numEntries; ++i)

	int i=0;
	for (bool done2=false; !done2; ++i )
	{
		assert(i<numEntries);
		printf("prob[%d] is %d\n", i,  rule.entries[i].probability);
		if ( rule.entries[i].probability >= rand)
		{
			GKEY code = rule.entries[i].code;
			printf("rule fired on code abs val=%d\n", code);
			// rule fired!
			//printf("rule fired! execute code %s\n",  ProductionRuleKeys::toString(code));
			//printf("TODO: generate note dur %d\n",  ProductionRuleKeys::getDuration(code));
			return code;
		}
	}
	assert(false);	// no rule fired
	return 0;
}

inline void ProductionRule::evaluate(const EvaluationState& es, int ruleToEval)
{
	printf("\n evaluate called on rule #%d\n", ruleToEval);
	const ProductionRule& rule = es.rules[ruleToEval];
	assert(rule._isValid(ruleToEval));
	GKEY result = _evaluateRule(rule, es.r.get());
	if (result == sg_invalid)		// request to remindate recrsion
	{
		GKEY code = ruleToEval;		// our "real" terminal code is our table index
		printf("production rule #%d terminated\n", ruleToEval);
		printf("rule terminated! execute code %s\n",  ProductionRuleKeys::toString(code));
		printf("TODO: generate note dur %d\n",  ProductionRuleKeys::getDuration(code));
		printf("TODO: need to produce something\n");
	}
	else
	{
		printf("production rule #%d expanded to %d\n", ruleToEval, result);
		// need to expand,then eval all of the expanded codes
		assert(false);
	}
}
			
// is the data self consistent, and appropriate for index
inline bool ProductionRule::_isValid(int index) const 
{
	if (index == sg_invalid)
	{
		printf("rule not allowed in first slot\n");
		return false;
	}
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
	// TODO: test that all rules add up to terminalValue
	return false;
}



#endif
