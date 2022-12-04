#ifndef __ELECTRICTRUCKLIFECYCLE__H__
#define __ELECTRICTRUCKLIFECYCLE__H__

#include <simlib.h>

#include "Configuration.hpp"
#include "TruckLifecycle.hpp"

#define FACTORY_CHARGE_TIME 150 // 2.5 hours
#define DEPO_CHARGE_TIME 570	// 9.5 hours

class ElectricTruckLifecycle : public TruckLifecycle
{
	void Behavior();
	int checkFuelAndFuelUpIfNeeded(double distance, bool atFactory);
	int fillFuel(double distance);
	double fuelingTime(double initialLevel, double finalLevel, bool atFactory);

public:
	ElectricTruckLifecycle(int id, TruckParams params, Configuration config);
	Stat electricityChargedAtFactory;
	Stat electricityChargedAtDestination;
};
#endif //!__ELECTRICTRUCKLIFECYCLE__H__