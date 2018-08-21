#!/bin/bash

TIMELIMIT=300s

declare -a cases=( 
                  "stationary_coordinators_"
                  "frequent_moving_"
                  "fast_mooving_"
                 )

declare -a metrics=(
    "totalSolution"
    "sentrecv"
    "dropped"
    "bskipped"
    "reused"
)

for j in "${cases[@]}"
do
    PREFIX=$j
    echo "______________ Processing Case: $PREFIX"
    
    for m in "${metrics[@]}"
    do
        >$PREFIX$m
    done

    for i in {1..12}
    do
        echo "________________ Running configuration $PREFIX$i"
        ../tutorial -r 0 -m -u Cmdenv -c $PREFIX$i -n ..:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../../Downloads/veins-veins-4.6/examples/veins:../../../Downloads/veins-veins-4.6/src/veins --image-path=../../inet/images:../../../Downloads/veins-veins-4.6/images -l ../../inet/src/INET -l ../../../Downloads/veins-veins-4.6/src/veins --cpu-time-limit=$TIMELIMIT omnetpp.ini   
    
        RESULT=results/$PREFIX$i-#0.vec
        echo "________________ Analysing $RESULT"

        for m in "${metrics[@]}"
        do
            scavetool -l -f "module(**.resultCollector)" $RESULT | grep $m | head -1 | awk '{print $6}' | sed "s/mean=//" >> $PREFIX$m
        done

    done
    
done

for m in "${metrics[@]}"
do
    pasteArg=""

    for j in "${cases[@]}"
    do
        pasteArg="$pasteArg $j$m"
    done

    paste $pasteArg > $m.csv
done
