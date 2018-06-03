#!/bin/bash

TIMELIMIT=300s

declare -a cases=("Coord_Data_Ratio_Sync_Fix_" 
                  "Wide_Angle_Coord_Data_Interval_" 
                  "Low_Speed_Coord_Data_Interval_" 
                  "High_Speed_Coord_Data_Interval_" 
                  "Coordinator_Dynamic_Speed_" 
                  "Coordinator_Stationary_" 
                )

for j in "${cases[@]}"
do
    PREFIX=$j
    echo "______________ Processing Case: $PREFIX"
    
    RESULT_FILE=$PREFIX.result
    echo "______________ Will write result to $RESULT_FILE"
    rm -rf $RESULT_FILE && touch $RESULT_FILE

    for i in {1..12}
    do
        echo "________________ Running configuration $PREFIX$i"
        ../tutorial -r 0 -m -u Cmdenv -c $PREFIX$i -n ..:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../../Downloads/veins-veins-4.6/examples/veins:../../../Downloads/veins-veins-4.6/src/veins --image-path=../../inet/images:../../../Downloads/veins-veins-4.6/images -l ../../inet/src/INET -l ../../../Downloads/veins-veins-4.6/src/veins --cpu-time-limit=$TIMELIMIT omnetpp.ini   
    
        RESULT=results/$PREFIX$i-#0.vec
        echo "________________ Analysing $RESULT"

        scavetool -l -f "module(**.centralizedCoordinator[0])" $RESULT | grep sentrecv | head -1 | awk '{print $6}' | sed "s/mean=//" >> $RESULT_FILE

    done
    
done

