#ifndef GENERATIVETRIGGERGENERATOR
#define GENERATIVETRIGGERGENERATOR

#include "StochasticGrammar.h"

/* wraps up some stochastic gnerative grammar stuff feeding
 * a trigger sequencer
 */
class GenerativeTriggerGenerator
{
public:
	GenerativeTriggerGenerator(ProductionRule * rules, GKEY initialState)
	{
	}
	bool clock()
	{
		return false;
	}
private:
};
#endif