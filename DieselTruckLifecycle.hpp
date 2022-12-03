#ifndef __DIESELTRUCKLIFECYCLE__H__
#define __DIESELTRUCKLIFECYCLE__H__

#include <simlib.h>

#include "Configuration.hpp"
#include "TruckLifecycle.hpp"

class DieselTruckLifecycle : public TruckLifecycle
{
	void travel(double distance);
	void Behavior();

public:
	DieselTruckLifecycle(int id, TruckParams params, Configuration config);
};

#endif //!__DIESELTRUCKLIFECYCLE__H__