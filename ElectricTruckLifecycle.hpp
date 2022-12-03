#ifndef __ELECTRICTRUCKLIFECYCLE__H__
#define __ELECTRICTRUCKLIFECYCLE__H__

#include <simlib.h>

#include "Configuration.hpp"
#include "TruckLifecycle.hpp"

class ElectricTruckLifecycle : public TruckLifecycle
{
	void Behavior();
	int checkFuelAndFuelUpIfNeeded(double distance);
	int fillFuel(double distance);

public:
	ElectricTruckLifecycle(int id, Configuration config);
	Stat electricityChargedAtFactory;
	Stat electricityChargedAtDestination;
};
#endif //!__ELECTRICTRUCKLIFECYCLE__H__