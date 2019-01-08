

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>
#include "routerMsg_m.h"
#include "transactionMsg_m.h"
#include "ackMsg_m.h"
#include "updateMsg_m.h"
#include "timeOutMsg_m.h"
#include "probeMsg_m.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <deque>
#include <map>
#include <fstream>
#include "structs/PaymentChannel.h"
#include "structs/PathInfo.h"
#include "structs/TransUnit.h"
#include "structs/CanceledTrans.h"
#include "structs/AckState.h"

#define MSGSIZE 100
using namespace std;

//global parameters
extern vector<TransUnit> _transUnitList;
extern int _numNodes;
//number of nodes in network
extern int _numRouterNodes;
extern int _numHostNodes;
extern map<int, vector<pair<int,int>>> _channels; //adjacency list format of graph edges of network
extern map<tuple<int,int>,double> _balances;
extern double _statRate;
extern double _clearRate;
extern double _maxTravelTime;
//map of balances for each edge; key = <int,int> is <source, destination>
//extern bool withFailures;
extern bool _waterfillingEnabled;
extern bool _timeoutEnabled;
extern int _kValue; //for k shortest paths
extern double _simulationLength;
