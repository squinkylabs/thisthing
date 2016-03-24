#ifndef DMSGTRIGGERGENERATOR
#define DMSGTRIGGERGENERATOR

#include "GenerativeTriggerGenerator.h"
#include "TriggerOutput.h"
#include "DModule.h"

/*
 * x = clock input
 * a = trigger output a
 * b = trigger output b
 */
class  DM_SGTriggerGenerator : public DModule
{
public:
	DM_SGTriggerGenerator() : _clockIn(false)
	{
	//	Nop();
		// set up generator with first grammar
		StochasticGrammarDictionary::Grammar g = StochasticGrammarDictionary::getGrammar(1);
		_generatora = new GenerativeTriggerGenerator(_r, g.rules, g.numRules, g.firstRule);
		_generatorb = new GenerativeTriggerGenerator(_r, g.rules, g.numRules, g.firstRule);
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_clockIn.go(x);

		bool triga = false;
		bool trigb = false;
		if (_clockIn.trigger())
		{
			triga = _generatora->clock();
			trigb = _generatorb->clock();
		}
		_trigOuta.go(triga);
		_trigOutb.go(trigb);
		a = _trigOuta.get();
		b = _trigOutb.get();
	}
private:
	TriggerOutput _trigOuta;
	TriggerOutput _trigOutb;
	GateTrigger _clockIn;
	GenerativeTriggerGenerator*  _generatora;
	GenerativeTriggerGenerator*  _generatorb;
	Random _r;

};
#endif