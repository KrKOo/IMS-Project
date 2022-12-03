#ifndef __GLOBALS__H__
#define __GLOBALS__H__

#include <simlib.h>

extern Store warehouse;
extern Facility loadingDock;
extern Queue truckParkingQueue;

// Stats
extern Stat truckParkingTime;
extern Histogram truckPackageCountHistogram;
extern int packagesDelivered;
extern int packagesManufactured;

#endif //!__GLOBALS__H__