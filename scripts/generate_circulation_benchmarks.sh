#!/bin/sh
PATH="../benchmarks/"
# network=("hotnets_circ_net" "40_sw_routers_circ_net" "100_sw_routers_circ_net")
toponame=("two_nodes_topo.txt" "three_nodes_topo.txt" "four_nodes_topo.txt" "five_nodes_topo.txt"\
    "hotnets_topo.txt" "40_sw_routers_topo.txt" "100_sw_routers_topo.txt")
num_nodes=("2" "3" "4" "5" "10" "40" "100")
balance=100
workloadname=("two_node_circ" "three_node_circ" "four_node_circ" "five_node_circ" \
    "hotnets_circ" "40_sw_routers_circ" "100_sw_routers_circ")
graph_type=("simple_topologies" "simple_topologies" "simple_topologies" "simple_topologies"\
    "hotnets_topo" "small_world" "small_world")

arraylength=${#workloadname[@]}
PYTHON="/usr/bin/python"

for (( i=0; i<${arraylength}; i++ ));
do 
    # generate the graph first to ned file
    network="$PATH${workloadname[i]}_net"
    topofile="$PATH${toponame[i]}"
    workload="$PATH${workloadname[i]}"
    inifile="$PATH${workloadname[i]}.ini"

    echo $network
    echo $topofile
    echo $inifile
    
    $PYTHON create_topo_ned_file.py ${graph_type[i]}\
            --network-name $network\
            --topo-filename $topofile\
            --num-nodes ${num_nodes[i]}\
            --balance-per-channel $balance\
            --separate-end-hosts


    # create transactions corresponding to this experiment run
    $PYTHON create_workload.py $workload uniform\
            --graph-topo custom\
            --payment-graph-type circulation\
            --topo-filename $topofile\
            --experiment-time 30\
            --balance-per-channel $balance\
            --generate-json-also\

    # create the ini file
    $PYTHON create_ini_file.py \
            --network-name $network\
            --topo-filename $topofile\
            --workload-filename $workload\
            --ini-filename $inifile
done 
