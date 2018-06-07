#!/bin/bash

TIMELIMIT=300s

declare -a cases=( 
                  "stationary_coordinators_longer_solver_time_"
                  "stationary_c_less_c_frequent_"
                  "stationary_coordinators_"
                  "frequent_moving_"
                  "fast_mooving_"
                 )

FINAL_RECVSENT=recvsent.result
FINAL_SOLUTIONS=solutions.result
FINAL_DROPPED=dropped.result

for j in "${cases[@]}"
do
    PREFIX=$j
    
    RESULT_FILE=$PREFIX.result
    RESULT_FILE_2=$PREFIX.solutionssent.result
    RESULT_FILE_3=$PREFIX.dropped.result

    echo "recvsent" > $RESULT_FILE
    echo "solutions" > $RESULT_FILE_2
    echo "dropped" > $RESULT_FILE_3

    pasteArg="$pasteArg $RESULT_FILE"
    pasteArg2="$pasteArg2 $RESULT_FILE"
    pasteArg3="$pasteArg3 $RESULT_FILE"
done

paste $pasteArg > recvsent.final
