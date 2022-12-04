#ifndef __GLOBALS__H__
#define __GLOBALS__H__

#include <simlib.h>
#include <vector>

extern Store warehouse;
extern Facility loadingDock;
extern Queue truckParkingQueue;

// Stats
extern int packagesDelivered;
extern int packagesManufactured;

// Ways
extern std::vector<Way> ways;

#endif //!__GLOBALS__H__