#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>
#include "simpleMsg_m.h"
#include <deque>
#include <map>

using namespace omnetpp;

using namespace std;

class transUnit{
   public:
      double amount;
      double timeSent;  //time delay after start time of simulation that trans-unit is active
      int sender;
      int receiver;
      vector<int> route; //includes sender and reciever as first and last entries
      int priorityClass;

      transUnit(double amount1, double timeSent1, int sender1, int receiver1, int priorityClass1){
         amount = amount1;
         timeSent = timeSent1;
         sender = sender1;
         receiver = receiver1;
         priorityClass=  priorityClass1;
      }
};

//global parameters
vector<transUnit> trans_unit_list; //list of all transUnits
int numNodes = 4; //number of nodes in network
map<int, vector<int>> channels; //adjacency list format of graph edges of network
map<tuple<int,int>,double> balances;
//map of balances for each edge; key = <int,int> is <source, destination>

vector<int> breadthFirstSearch(int sender, int receiver){
    deque<vector<int>> nodesToVisit;
    bool visitedNodes[numNodes];
    for (int i=0; i<numNodes; i++){
        visitedNodes[i] =false;
    }
    visitedNodes[sender] = true;

    vector<int> temp;
    temp.push_back(sender);
    nodesToVisit.push_back(temp);

    while ((int)nodesToVisit.size()>0){

        vector<int> current = nodesToVisit[0];
         nodesToVisit.pop_front();
        int lastNode = current.back();
        for (int i=0; i<(int)channels[lastNode].size();i++){

            if (!visitedNodes[channels[lastNode][i]]){
                temp = current; // assignment copies in case of vector
                temp.push_back(channels[lastNode][i]);
                nodesToVisit.push_back(temp);
                visitedNodes[channels[lastNode][i]] = true;

                if (channels[lastNode][i]==receiver){

                    return temp;
                }
            }
         }
      }
   }
   vector<int> empty;
   return empty;
}

/*get_route- take in sender and receiver graph indicies, and returns
 *  BFS shortest path from sender to receiver in form of node indicies,
 *  includes sender and reciever as first and last entry
 */
vector<int> get_route(int sender, int receiver){
  //do searching without regard for channel capacities, DFS right now

  // printf("sender: %i; receiver: %i \n [", sender, receiver);
   vector<int> route =  breadthFirstSearch(sender, receiver);
/*
   for (int i=0; i<(int)route.size(); i++){
        printf("%i, ", route[i]);
    }
    printf("] \n");
*/
    return route;

}


class routerNode : public cSimpleModule
{
   private:
      simsignal_t arrivalSignal;
      vector< transUnit > my_trans_units; //list of transUnits that have me as sender
      map<int, cGate*> node_to_gate; //map that takes in index of node adjacent to me, returns gate to that node
      map<int, double> node_to_balance; //map takes in index of adjacent node, returns outgoing balance
      map<int, deque<tuple<int, double , simpleMsg *>>> node_to_queued_trans_units;
          //map takes in index of adjacent node, returns queue of trans_units to send to that node
   protected:
      virtual simpleMsg *generateMessage(transUnit transUnit);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void forwardMessage(simpleMsg *msg);
      virtual void processTransUnits(int dest, deque<tuple<int, double , simpleMsg *>>& q);
      virtual string string_node_to_balance();
};
Define_Module(routerNode);

/* string_node_to_balance - helper function, turns the node_to_balance map into printable string
 *
 */
string routerNode:: string_node_to_balance(){
   string result = "";
   for(map<int,double>::iterator iter = node_to_balance.begin(); iter != node_to_balance.end(); ++iter)
   {
      int key =  iter->first;
      int value = iter->second;
      result = result + "("+to_string(key)+":"+to_string(value)+") ";
   }
   return result;
}

/* initialize() -
 *  if node index is 0:
 *      - initialize global parameters: instantiate all transUnits, and add to global list "trans_unit_list"
 *      - create "channels" map - adjacency list representation of network
 *      - create "balances" map - map representing initial balances; each key is directed edge (source, dest)
 *          and value is balance
 *  all nodes:
 *      - find my transUnits from "trans_unit_list" and store them into my private list "my_trans_units"
 *      - create "node_to_gate" map - map to identify channels, each key is node index adjacent to this node
 *          value is gate connecting to that adjacent node
 *      - create "node_to_balances" map - map to identify remaining outgoing balance for gates/channels connected
 *          to this node (key is adjacent node index; value is remaining outgoing balance)
 *      - create "node_to_queued_trans_units" map - map to get transUnit queue for outgoing transUnits,
 *          one queue corresponding to each adjacent node/each connected channel
 *      - send all transUnits in my_trans_units as a message (using generateMessage), sent to myself (node index),
 *          scheduled to arrive at timeSent parameter field in transUnit
 */

void routerNode::initialize()
{

   if (getIndex() == 0){  //main initialization for global parameters
      // instantiate all the transUnits that need to be sent
      transUnit j1 = transUnit(3,0,0,3,0);
      transUnit j2 = transUnit(5,0.25,2,0,0);
      transUnit j3 = transUnit(5,0.25,2,0,0);
      transUnit j4 = transUnit(5,0.25,2,0,0);
      transUnit j5 = transUnit(3,0.5,0,3,0);
      transUnit j6 = transUnit(3,0.75,0,3,0);
      // add all the transUnits into global list
      trans_unit_list.push_back(j1);
      trans_unit_list.push_back(j2);
      trans_unit_list.push_back(j3);
      trans_unit_list.push_back(j4);
      trans_unit_list.push_back(j5);
      trans_unit_list.push_back(j6);

      //create "channels" map
      vector<int>v0 = {1};
      vector<int>v1 = {0,2};
      vector<int>v2 = {1,3};
      vector<int>v3 = {2};
      channels[0] = v0;
      channels[1] = v1;
      channels[2] = v2;
      channels[3] = v3;
      //create "balances" map
      balances[make_tuple(0,1)] = 10;
      balances[make_tuple(1,0)] = 10;

      balances[make_tuple(1,2)] = 10;
      balances[make_tuple(2,1)] = 10;

      balances[make_tuple(2,3)] = 10;
      balances[make_tuple(3,2)] = 10;
   }

   //iterate through the global trans_unit_list and find my transUnits
   for (int i=0; i<(int)trans_unit_list.size(); i++){
      if (trans_unit_list[i].sender == getIndex()){
         my_trans_units.push_back(trans_unit_list[i]);
      }
   }

   //create "node_to_gate" map
   const char * gateName = "out";
   cGate *destGate = NULL;

   int i = 0;
   int gateSize = gate(gateName, 0)->size();

   do {
      destGate = gate(gateName, i++);
      cGate *nextGate = destGate->getNextGate();
      if (nextGate ) {
         node_to_gate[nextGate->getOwnerModule()->getIndex()] = destGate;
      }
   } while (i < gateSize);

   //create "node_to_balance" map
   for(map<int,cGate *>::iterator iter = node_to_gate.begin(); iter != node_to_gate.end(); ++iter)
   {
      int key =  iter->first;
      node_to_balance[key] = balances[make_tuple(getIndex(),key)];
   }

   WATCH_MAP(node_to_balance);

   //create "node_to_queued_trans_units" map
   for(map<int,cGate *>::iterator iter = node_to_gate.begin(); iter != node_to_gate.end(); ++iter)
   {
      int key =  iter->first;
      deque<tuple<int, double , simpleMsg *>> temp;
      node_to_queued_trans_units[key] = temp;
   }

   arrivalSignal = registerSignal("arrival");

   //implementing timeSent parameter, send all msgs at beginning
   for (int i=0 ; i<(int)my_trans_units.size(); i++){
      transUnit j = my_trans_units[i];
      double timeSent = j.timeSent;
      simpleMsg *msg = generateMessage(j);
      scheduleAt(timeSent, msg);
   }

}

/* generateMessage - takes in a transUnit object and returns corresponding simpleMsg message;
 *      transUnit and simpleMsg have the same fields except simpleMsg has extra field "hopCount",
 *      representing distance traveled so far
 *      note: calls get_route function to get route from sender to receiver
 */

simpleMsg *routerNode::generateMessage(transUnit transUnit)
{
   char msgname[20];
   sprintf(msgname, "tic-%d-to-%d", transUnit.sender, transUnit.receiver);
   simpleMsg *msg = new simpleMsg(msgname);
   msg->setAmount(transUnit.amount);
   msg->setTimeSent(transUnit.timeSent);
   msg->setSender(transUnit.sender);
   msg->setReceiver(transUnit.receiver);
   msg->setRoute(get_route(transUnit.sender,transUnit.receiver));
   msg->setPriorityClass(transUnit.priorityClass);
   msg->setHopCount(0);
   return msg;
}

/*
 * sortFunction - helper function used to sort queued transUnit list by ascending priorityClass, then by
 *      ascending amount
 */
bool sortFunction(const tuple<int,double, simpleMsg*> &a,
      const tuple<int,double, simpleMsg*> &b)
{
   if (get<0>(a) < get<0>(b)){
      return true;
   }
   else if (get<0>(a) == get<0>(b)){
      return (get<1>(a) < get<1>(b));
   }
   return false;
}

/* handleMessage -
 *    1. checks if message is self-message (sent from initialize function); if not self-message, re-adjust
 *      (increment) balance, send out jobs if possible to node that just sent me units (call processTransUnits)
 *    2. checks if message is has arrived. If has arrived delete message. If not arrived, add new job to queue,
 *       send out jobs if possible to node that is the next node of message I received
 *          - deals with checking if transUnit queue is empty, then only send if enough balance left
 */
void routerNode::handleMessage(cMessage *msg)
{
   simpleMsg *ttmsg = check_and_cast<simpleMsg *>(msg);
   int hopcount = ttmsg->getHopCount();
   deque<tuple<int, double , simpleMsg *>> q;

   if (!(msg -> isSelfMessage())){
      int hopcount = ttmsg->getHopCount();
      int sender = ttmsg->getRoute()[hopcount-1];
      node_to_balance[sender] = node_to_balance[sender] + ttmsg->getAmount();
      //see if any new messages can be sent out based on priority for the channel that just came in
      q = node_to_queued_trans_units[sender];
      processTransUnits(sender, q);
   }

   if(getIndex() == ttmsg->getReceiver()){
      emit(arrivalSignal, hopcount);
      EV << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
      bubble("Message arrived!");
      delete ttmsg;
   }
   else{
      //displays string of balances remaining on connected channels
      bubble(string_node_to_balance().c_str());
      int nextStop = ttmsg->getRoute()[hopcount+1];
      q = node_to_queued_trans_units[nextStop];
      q.push_back(make_tuple(ttmsg->getPriorityClass(), ttmsg->getAmount(),
               ttmsg));
      // re-sort queued transUnits for next stop based on lowest priority, then lowest amount
      sort(q.begin(), q.end(), sortFunction);
      processTransUnits(nextStop, q);
      bubble(string_node_to_balance().c_str());
   }
}

/*
 * processTransUnits - given an adjacent node, and transUnit queue of things to send to that node, sends
 *  transUnits until channel funds are too low by calling forwardMessage on message representing transUnit
 */
void routerNode:: processTransUnits(int dest, deque<tuple<int, double , simpleMsg *>>& q){
   while((int)q.size()>0 && get<1>(q[0])<=node_to_balance[dest]){
      forwardMessage(get<2>(q[0]));
      q.pop_front();
   }
}

/*
 *  forwardMessage - given a message representing a transUnit, increments hopCount, finds next destination,
 *      adjusts (decrements) channel balance, sends message to next node on route
 */
void routerNode::forwardMessage(simpleMsg *msg)
{
   // Increment hop count.
   msg->setHopCount(msg->getHopCount()+1);
   //use hopCount to find next destination
   int nextDest = msg->getRoute()[msg->getHopCount()];
   EV << "Forwarding message " << msg << " on gate[" << nextDest << "]\n";
   int amt = msg->getAmount();
   node_to_balance[nextDest] = node_to_balance[nextDest] - amt;
   send(msg, node_to_gate[nextDest]);

}