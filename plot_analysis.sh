#!/bin/bash
PATH_NAME="benchmarks/circulations/"
GRAPH_PATH="scripts/figures/"

prefix=("two_node" "three_node" "four_node" "five_node") 
   #"sw_40_routers" "sf_40_routers")
    #"sw_400_routers" "sf_400_routers")
    #"sw_1000_routers" "sf_1000_routers")

arraylength=${#prefix[@]}

#general parameters that do not affect config names
simulationLength=1000
statCollectionRate=1
timeoutClearRate=1
timeoutEnabled=false



for (( i=0; i<${arraylength}; i++));
do 
    payment_graph_type=circ
    delay=1ms
    if [ "$timeoutEnabled" = true ] ; then timeout="timeouts"; else timeout="no_timeouts"; fi
    graph_op_prefix=${GRAPH_PATH}${timeout}/${prefix[i]}_${delay}_${simulationLength}_
    vec_file_prefix=${PATH_NAME}results/${prefix[i]}_${payment_graph_type}_net_

    #routing schemes where number of path choices doesn't matter
    for routing_scheme in shortestPath #silentWhispers
    do
        vec_file_path=${vec_file_prefix}${routing_scheme}-#0.vec

        python scripts/generate_analysis_plots_for_single_run.py \
          --vec_file ${vec_file_path} \
          --save ${graph_op_prefix}${routing_scheme} \
          --balance \
          --queue_info --timeouts --frac_completed \
          --inflight --path --timeouts_sender \
          --pending --bottlenecks
    done

    #routing schemes where number of path choices matter
    for routing_scheme in waterfilling #smoothWaterfilling #LP
    do
      for numPathChoices in 4
        do
            vec_file_path=${vec_file_prefix}${routing_scheme}_${numPathChoices}-#0.vec

            python scripts/generate_analysis_plots_for_single_run.py \
              --vec_file ${vec_file_path} \
              --save ${graph_op_prefix}${routing_scheme} \
              --balance \
              --queue_info --timeouts --frac_completed \
              --inflight --path --timeouts_sender \
              --pending --bottlenecks
        done
    done
done
