
using namespace std;
using namespace omnetpp;
#include "routerMsg_m.h"

struct PathInfo{
    public:
        // waterfilling pertaining info
        vector<int> path = {};
        double probability = -1;
        simtime_t lastUpdated = -1;
        double bottleneck = -1;
        vector<double> pathBalances = {};

        // signals and statistics
        simsignal_t bottleneckPerDestPerPathSignal;
        simsignal_t probabilityPerDestPerPathSignal;
        simsignal_t probeBackPerDestPerPathSignal;
        simsignal_t rateCompletedPerDestPerPathSignal;
        simsignal_t rateAttemptedPerDestPerPathSignal;

        // signal for dctcp
        simsignal_t fractionMarkedSignal;
        simsignal_t smoothedFractionMarkedSignal;
        simsignal_t rateOfAcksSignal;
        double markedPackets = 0;
        double unmarkedPackets = 0;
        double amtAcked = 0;
        double totalMarkedPacketsForInterval = 0;
        double totalPacketsForInterval = 0;
        double lastMarkedFraction = 0;
        
        int statRateCompleted = 0;
        int statRateAttempted = 0;

        //additional parameters for price scheme
        double rateToSendTrans = 1;
        double yRateToSendTrans = 1;
        simtime_t timeToNextSend = 0;
        double sumOfTransUnitsInFlight = 0;
        double rttMin = 0.0;
        double window = 100;
        double priceLastSeen = 0;
        double oldSumOfRates = 0;
        routerMsg * triggerTransSendMsg;
        simtime_t lastSendTime = 0;
        simtime_t lastRateUpdateTime = 0;
        double lastTransSize = 0.0;
        double amtAllowedToSend = 0.0;
        bool isSendTimerSet = false;

        // number and rate of txns sent to a particular destination on this path
        double nValue = 0;
        double rateSentOnPath = 1;
        
        bool isProbeOutstanding = false;

        //signals for price scheme per path
        simsignal_t rateToSendTransSignal;
        simsignal_t rateActuallySentSignal;
        simsignal_t timeToNextSendSignal;
        simsignal_t sumOfTransUnitsInFlightSignal;
        simsignal_t windowSignal;
        simsignal_t priceLastSeenSignal;
        simsignal_t isSendTimerSetSignal;


};
