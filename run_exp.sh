#!/bin/sh
network=$1
topofile=$2
num_nodes=$3
balance=$4
workload=$5
inifile=$6
graph_type=$7

# generate the graph first to ned file
python scripts/create_topo_ned_file.py $graph_type\
        --network-name $network\
        --topo-filename $topofile\
        --num-nodes $num_nodes\
        --balance-per-channel $balance\
        --separate-end-hosts


# create transactions corresponding to this experiment run
python scripts/create_workload.py $workload uniform\
        --graph-topo custom\
        --payment-graph-type circulation\
        --topo-filename for_workload_$topofile\
        --experiment-time 30\
        --generate-json-also\

# create the ini file
python scripts/create_ini_file.py \
        --network-name $network\
        --topo-filename $topofile\
        --workload-filename $workload\
        --ini-filename $inifile

# run the omnetexecutable with the right parameters
#TODO: only run the executable from here, rest can be done separately
./spiderNet -u Cmdenv -f $inifile -c $network -n .

# cleanup?
