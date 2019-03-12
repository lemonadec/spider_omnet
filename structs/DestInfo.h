
using namespace std;
using namespace omnetpp;
#include "routerMsg_m.h"

class DestInfo{
public:
    vector<routerMsg *> transWaitingToBeSent; // stack in reality
    

    // units are txns per second
    double demand = 0;

    // number of transactions since the last interval
    double transSinceLastInterval = 0;
};
