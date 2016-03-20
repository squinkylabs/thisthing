#ifndef DMSGTRIGGERGENERATOR
#define DMSGTRIGGERGENERATOR

#include "GenerativeTriggerGenerator.h"
#include "TriggerOutput.h"
#include "DModule.h"

/*
 * x = clock input
 * a = trigger output
 */
class  DM_SGTriggerGenerator : public DModule
{
public:
	DM_SGTriggerGenerator() : _clockIn(false)
	{
		// set up generator with first grammar
		StochasticGrammarDictionary::Grammar g = StochasticGrammarDictionary::getGrammar(0);
		_generator = new GenerativeTriggerGenerator(g.rules, g.numRules, g.firstRule);
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_clockIn.go(x);

		bool trig = false;
		if (_clockIn.trigger())
		{
			bool trig = _generator->clock();
			if (trig)
			{
				trig = true;
			}
		}
		_trigOut.go(trig);
		a = b = _trigOut.get();
	}
private:
	TriggerOutput _trigOut;
	GateTrigger _clockIn;
	GenerativeTriggerGenerator*  _generator;

};
#endif