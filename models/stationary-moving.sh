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
    echo "______________ Processing Case: $PREFIX"
    
    RESULT_FILE=$PREFIX.result
    RESULT_FILE_2=$PREFIX.solutionssent.result
    RESULT_FILE_3=$PREFIX.dropped.result

    pasteArg="$pasteArg $RESULT_FILE"
    pasteArg2="$pasteArg2 $RESULT_FILE_2"
    pasteArg3="$pasteArg3 $RESULT_FILE_3"
    
    rm -rf $RESULT_FILE && touch $RESULT_FILE
    rm -rf $RESULT_FILE_2 && touch $RESULT_FILE_2
    rm -rf $RESULT_FILE_3 && touch $RESULT_FILE_3

    for i in {1..12}
    do
        echo "________________ Running configuration $PREFIX$i"
        ../tutorial -r 0 -m -u Cmdenv -c $PREFIX$i -n ..:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../../Downloads/veins-veins-4.6/examples/veins:../../../Downloads/veins-veins-4.6/src/veins --image-path=../../inet/images:../../../Downloads/veins-veins-4.6/images -l ../../inet/src/INET -l ../../../Downloads/veins-veins-4.6/src/veins --cpu-time-limit=$TIMELIMIT omnetpp.ini   
    
        RESULT=results/$PREFIX$i-#0.vec
        echo "________________ Analysing $RESULT"

        scavetool -l -f "module(**.resultCollector)" $RESULT | grep sentrecv | head -1 | awk '{print $6}' | sed "s/mean=//" >> $RESULT_FILE
        scavetool -l -f "module(**.resultCollector)" $RESULT | grep totalSolution | head -1 | awk '{print $6}' | sed "s/mean=//" >> $RESULT_FILE_2
        scavetool -l -f "module(**.resultCollector)" $RESULT | grep dropped | head -1 | awk '{print $6}' | sed "s/mean=//" >> $RESULT_FILE_3

    done
    
done

paste $pasteArg > $FINAL_RECVSENT
paste $pasteArg2 > $FINAL_SOLUTIONS
paste $pasteArg3 > $FINAL_DROPPED
